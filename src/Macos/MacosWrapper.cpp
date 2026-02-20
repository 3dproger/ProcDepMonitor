#include "MacosWrapper.h"
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <sys/proc_info.h>
#include <libproc.h>
#include <mach-o/dyld.h>
#include <QDebug>

MacosWrapper::MacosWrapper() {}

QList<OSProcessInfo> MacosWrapper::processes()
{
    const auto pids = processesPids();

    _processes.clear();
    _processes.reserve(pids.size());

    for (const auto& pid : pids)
    {
        _processes.append(processByPID(pid));
    }

    std::sort(_processes.begin(), _processes.end(), OSWrapper::compareOSProcessInfo);

    return _processes;
}

OSProcessInfo MacosWrapper::processByPIDImpl(int64_t pid)
{
    OSProcessInfo info;

    info.id = pid;
    info.name = getProcessPath(pid);

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
