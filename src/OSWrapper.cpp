/*
 * MIT License

 * Copyright (c) 2020 Alexander Kirsanov

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * */

#include "OSWrapper.hpp"
#if defined (Q_OS_WIN32)
#include "Windows/ToolHelpManager.hpp"
#elif defined(Q_OS_LINUX)
#include "Linux/LinuxWrapper.hpp"
#elif defined(Q_OS_MAC)
#include "Macos/MacosWrapper.h"
#endif
#include <QDebug>

namespace
{

static const QSet<QString> Win32ExecutableExtentions = {
    "exe",
    "msi",
    "bat",
};

void removeDuplicates(OSProcessInfo& info)
{
    for (int i = 0; i < info.deps.count();)
    {
        const OSProcessDependence dep = info.deps[i];

        bool removed = false;
        for (int j = i + 1; j < info.deps.count(); j++)
        {
            const OSProcessDependence dep2 = info.deps[j];
            if (dep == dep2)
            {
                info.deps.removeAt(j);
                removed = true;
                //qDebug() << "Removed duplicate" << dep2.fileName;
                break;
            }
        }

        if (!removed)
        {
            i++;
        }
    }
}

QString extractExtension(const QString& path_)
{
    QString path = path_;
    path = path.mid(path.lastIndexOf('/') + 1);
    path = path.mid(path.lastIndexOf('\\') + 1);
    if (path.contains('.'))
    {
        return path.mid(path.lastIndexOf('.') + 1).toLower();
    }

    return QString();
}

}

OSWrapper &OSWrapper::instance()
{
    //It is signleton
    #if defined (Q_OS_WIN32)
    static ToolHelpManager instance;
    #elif defined(Q_OS_LINUX)
    static LinuxWrapper instance;
    #elif defined(Q_OS_MAC)
    static MacosWrapper instance;
    #else
    qFatal("This OS is not yet supported");
    static OSWrapper instance;
    #endif

    return instance;
}

OSProcessInfo OSWrapper::getProcessByPid(const int64_t pid, const bool includeDeps) const
{
    OSProcessInfo info = processByPidImpl(pid, includeDeps);
    removeDuplicates(info);

    for (OSProcessDependence& dep : info.deps)
    {
        dep.extention = extractExtension(dep.fileName);
#if defined (Q_OS_WIN32)
        dep.executable = Win32ExecutableExtentions.contains(dep.extention);
#endif
    }

    std::sort(info.deps.begin(), info.deps.end(), compareOSProcessDependence);

    return info;
}

bool OSWrapper::compareOSProcessInfo(const OSProcessInfo &info1, const OSProcessInfo &info2)
{
    //By canGetDependencies
    if (info1.canGetDeps && !info2.canGetDeps)
    {
        return true;
    }
    else if (!info1.canGetDeps && info2.canGetDeps)
    {
        return false;
    }
    else if (!info1.canGetDeps && !info2.canGetDeps)
    {
        #ifdef Q_OS_UNIX
        //By first chat '[' for UNIX only
        if (!info1.name.isEmpty() && !info2.name.isEmpty())
        {
            if (info1.name.at(0) == '[' && info2.name.at(0) != '[')
            {
                return false;
            }
            else if (info1.name.at(0) != '[' && info2.name.at(0) == '[')
            {
                return true;
            }
        }
        #endif

        return info1.id < info2.id;
    }

    //By name
    #ifdef Q_OS_WIN32
        return info1.name.toLower() < info2.name.toLower();
    #else
        return info1.name < info2.name;
    #endif
}

bool OSWrapper::compareOSProcessDependence(const OSProcessDependence &dep1, const OSProcessDependence &dep2)
{
    if (dep1.executable != dep2.executable)
    {
        return dep1.executable;
    }

    if (dep1.specialDir != dep2.specialDir)
    {
        return dep1.specialDir < dep2.specialDir;
    }

    #ifdef Q_OS_WIN32
    if (dep1.extention != dep2.extention)
    {
        return dep1.extention > dep2.extention;
    }

    if (dep1.fileName.toLower() != dep2.fileName.toLower())
    {
        return dep1.fileName.toLower() < dep2.fileName.toLower();
    }

    return dep1.name.toLower() < dep2.name.toLower();
    #else
    if (dep1.fileName != dep2.fileName)
    {
        return dep1.fileName < dep2.fileName;
    }

    return dep1.name < dep2.name;
    #endif
}
