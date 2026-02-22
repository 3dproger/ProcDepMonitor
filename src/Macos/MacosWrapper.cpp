#include "MacosWrapper.h"
#include "../VMMapParser.h"
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <sys/proc_info.h>
#include <libproc.h>
#include <mach-o/dyld.h>
#include <mach-o/dyld_images.h>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>

MacosWrapper::MacosWrapper() {}

QList<OSProcessInfo> MacosWrapper::getProcesses(const bool includeDeps) const
{
    QList<OSProcessInfo> result;

    const auto pids = processesPids();
    result.reserve(pids.count());

    for (const auto& pid : pids)
    {
        result.append(getProcessByPid(pid, includeDeps));
    }

    std::sort(result.begin(), result.end(), OSWrapper::compareOSProcessInfo);

    return result;
}

OSProcessInfo MacosWrapper::processByPidImpl(int64_t pid, const bool includeDeps) const
{
    OSProcessInfo info;

    info.id = pid;
    info.name = getProcessPath(pid);
    info.fileName = info.name;
    if (includeDeps)
    {
        info.dependencies = getDeps(pid, info.canGetDependencies);
    }
    info.valid = true;

    qDebug() << info.id << info.name << ", found:" << info.dependencies.count();

    return info;
}

QList<int64_t> MacosWrapper::processesPids()
{
    QList<int64_t>pids;
    kinfo_proc* procList = nullptr;
    size_t procCount = 0;

    // Call sysctl once to get the size of the process list
    size_t size = 0;
    if (sysctl((int[]){ CTL_KERN, KERN_PROC, KERN_PROC_ALL }, 3, NULL, &size, NULL, 0) == -1)
    {
        qCritical() << "sysctl (size) failed: " << strerror(errno);
        return {};
    }

    do
    {
        // Allocate memory for the process list
        if (procList)
        {
            delete[] procList;
        }
        procList = new kinfo_proc[size / sizeof(kinfo_proc)];

        // Call sysctl again to get the actual process list
        if (sysctl((int[]){ CTL_KERN, KERN_PROC, KERN_PROC_ALL }, 3, procList, &size, NULL, 0) == -1)
        {
            if (errno == ENOMEM)
            {
                // Buffer was too small, loop again with the new size
                continue;
            }

            qCritical() << "sysctl (data) failed: " << strerror(errno);
            delete[] procList;
            return pids;
        }
        break; // Success
    } while (true);

    // Populate the vector of PIDs
    procCount = size / sizeof(kinfo_proc);
    for (size_t i = 0; i < procCount; ++i)
    {
        pids.push_back(procList[i].kp_proc.p_pid);
    }

    delete[] procList;
    return pids;
}

QString MacosWrapper::getProcessName(pid_t pid)
{
    char buf[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_name(pid, buf, sizeof(buf)) <= 0)
    {
        qCritical() << "failed proc_name " << pid << ": " << strerror(errno);
        return {};
    }

    return buf;
}

QString MacosWrapper::getProcessPath(const pid_t pid)
{
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, pathbuf, sizeof(pathbuf)) <= 0)
    {
        qCritical() << "failed proc_pidpath " << pid << ": " << strerror(errno);
        return {};
    }

    return pathbuf;
}

QList<OSProcessDependence> MacosWrapper::getDeps(const pid_t pid, bool& hasAccess)
{
    QList<OSProcessDependence> result;

    const auto data = VMMapParser::parseForPid(pid);
    hasAccess = data.hasAccess;

    const auto& regions = data.regions;
    for (const auto& region : regions)
    {
        if (region.startsWith('/'))
        {
            QFileInfo info(region);

            OSProcessDependence dep;
            dep.name = info.fileName();
            dep.fileName = region;
            dep.extention = info.suffix();
            dep.valid = true;

            result.append(dep);
        }
    }

    return result;
}

bool MacosWrapper::revealInFinder(const QString &rawPath)
{
    QFileInfo fileInfo(rawPath);

    auto path = fileInfo.canonicalFilePath();
    if (path.isEmpty())
    {
        qCritical() << "Failed to get canonical file path";
        path = rawPath;
    }

    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
                      .arg(path);

    if (QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs) != 0)
    {
        qCritical() << "Failed to execute script to reveal";
        return false;
    }


    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    if (QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs) != 0)
    {
        qCritical() << "Failed to execute script to activate";
        return false;
    }

    return true;
}
