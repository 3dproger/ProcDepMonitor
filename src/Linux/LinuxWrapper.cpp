#include "LinuxWrapper.hpp"
#include <QDebug>
#include <QDir>
#include "Utils/utils.h"

LinuxWrapper::LinuxWrapper()
{

}

QList<OSProcessInfo> LinuxWrapper::getProcesses()
{
    _processes.clear();

    QDir dirProc("/proc");
    if (!dirProc.exists())
    {
        qWarning() << "Palundra!" << dirProc.path() << "not found!";
    }

    const QFileInfoList& files = dirProc.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden);

    bool ok;
    int64_t pid;
    for (const QFileInfo& info : files)
    {
        const QString& fileNamePid = info.fileName();
        pid = fileNamePid.toLongLong(&ok);
        if (ok)
        {
            _processes.append(processByPID(pid));
        }
    }


    std::sort(_processes.begin(), _processes.end(), OSWrapper::sortOSProcessInfo);

    return _processes;
}

OSProcessInfo LinuxWrapper::processByPID(int64_t pid)
{
    OSProcessInfo info;

    info.id = pid;

    //Name from cmdline
    QFile fileCmdLine(QString("/proc/%1/cmdline").arg(pid));
    if (fileCmdLine.open(QFile::Text | QFile::ReadOnly))
    {
        info.name = fileCmdLine.readAll();
        info.valid = true;
        fileCmdLine.close();
    }

    //fileName from exe
    QFileInfo fileExe(QString("/proc/%1/exe").arg(pid));
    if (fileExe.exists())
    {
        info.fileName = fileExe.canonicalFilePath();
        info.valid = true;
    }

    //Name from status
    if (info.name.isEmpty())
    {
        QFile fileStatus(QString("/proc/%1/status").arg(pid));
        if (fileStatus.open(QFile::Text | QFile::ReadOnly))
        {

            // Usually the name is in the first line
            // But just in case, check another 1024 lines
            // So that there is no endless loop for any reason, we limit ourselves to this number

            for (int i = 0; i < 1024; ++i)
            {
                const QByteArray& line = fileStatus.readLine();

                if (Utils::containsLeft(line, "Name:", false))
                {
                    info.name = "[" + line.mid(5).trimmed() + "]";
                    info.valid = true;
                    break;
                }

                if (fileStatus.atEnd())
                {
                    break;
                }
            }

            fileStatus.close();
        }
    }


    //Dependenced from map_files
    QDir mapFiles(QString("/proc/%1/map_files").arg(pid));

    if (mapFiles.isReadable())
    {
        info.canGetDependencies = true;

        const QFileInfoList& symlinks = mapFiles.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden);

        for (const QFileInfo& fileInfo : symlinks)
        {
            OSProcessDependence dep;

            dep.fileName = fileInfo.canonicalFilePath();

            dep.name = Utils::getFileNameFromPath(dep.fileName);

            if (!dep.fileName.isEmpty())
            {
                dep.valid = true;
            }

            addDep(info.dependencies, dep);
        }
    }

    std::sort(info.dependencies.begin(), info.dependencies.end(), sortOSProcessDependence);

    return info;
}

void LinuxWrapper::addDep(QList<OSProcessDependence> &list, const OSProcessDependence &dep)
{
    bool added = false;

    for (const OSProcessDependence& old : list)
    {
        if (!old.fileName.isEmpty() && !dep.fileName.isEmpty())
        {
            if (old.fileName == dep.fileName)
            {
                added = true;
                break;
            }
        }
    }

    if (!added)
    {
        list.append(dep);
    }
}
