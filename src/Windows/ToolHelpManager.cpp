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

#include "ToolHelpManager.hpp"
#include "Windows/TaskListManager.hpp"
#include <QDebug>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <QSettings>

//From https://docs.microsoft.com/en-us/windows/win32/toolhelp/traversing-the-module-list

ToolHelpManager::ToolHelpManager()
{
    //Check if As Administrator
    // get ready to attempt to edit the registry
    QSettings adminTest("HKEY_LOCAL_MACHINE", QSettings::NativeFormat);
    // get the current value and put it back
    QVariant currentValue = adminTest.value("(Default)");
    adminTest.setValue("(Default)", currentValue);
    adminTest.sync();
    // see if there was an error
    _isAdministrator = adminTest.status() != QSettings::AccessError;

    if (_isAdministrator)
    {
        qDebug("Started as Administrator");
    }
    else
    {
        qDebug("Started without administrator privileges");
    }
}

QList<OSProcessInfo> ToolHelpManager::getProcesses(const bool includeDeps) const
{
    QList<OSProcessInfo> result;
    const auto tasks = TaskListManager::getList();

    for (const auto& task : tasks)
    {
        auto info = _processByPID(uint32_t(task.processId), includeDeps);

        if (info.name.isEmpty())
        {
            info.name = task.processName;
        }

        info.fileName = getProcessPathByPid(task.processId);

        result.append(info);
    }

    std::sort(result.begin(), result.end(), OSWrapper::compareOSProcessInfo);

    return result;
}

OSProcessInfo ToolHelpManager::processByPidImpl(const int64_t pid, const bool includeDeps) const
{
    auto info = _processByPID(pid, includeDeps);

    if (info.name.isEmpty())
    {
        const auto& tasks = TaskListManager::getList();

        for (const auto& task : tasks)
        {
            if (task.processId == info.id)
            {
                info.name = task.processName;
                break;
            }
        }
    }

    return info;
}

OSProcessInfo ToolHelpManager::_processByPID(const int64_t pid, const bool includeDeps)
{
    OSProcessInfo info;

    info.id = pid;

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    //Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, uint32_t(pid));
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        return info;
    }

    //  Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    //  Retrieve information about the first module,
    //  and exit if unsuccessful
    if (!Module32First(hModuleSnap, &me32))
    {
        CloseHandle(hModuleSnap);     // Must clean up the snapshot object!
        return info;
    }

    info.valid = true;
    info.canGetDeps = true;

    if (includeDeps)
    {
        info.loadedDeps = true;
        //  Now walk the module list of the process,
        //  and display information about each module
        do
        {
            OSProcessDependence dep;

            dep.valid = true;
            dep.name = QString::fromWCharArray(me32.szModule);
            dep.fileName = QString::fromWCharArray(me32.szExePath);
            dep.info = QFileInfo(dep.fileName);

            if (dep.fileName.startsWith("\\\\?\\"))
            {
                dep.fileName = dep.fileName.mid(4);
            }

            dep.specialDir = getSpecialDir(dep.fileName);

            info.deps.append(dep);
        }
        while(Module32Next(hModuleSnap, &me32));
    }
    else
    {
        info.loadedDeps = false;
    }

    //  Do not forget to clean up the snapshot object.
    CloseHandle(hModuleSnap);

    return info;
}

void ToolHelpManager::addDep(QList<OSProcessDependence> &list, const OSProcessDependence &dep)
{
    bool added = false;

    for (const OSProcessDependence& old : list)
    {
        if (!old.fileName.isEmpty() && !dep.fileName.isEmpty())
        {
            if (old.fileName.toLower() == dep.fileName.toLower())
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

QString ToolHelpManager::getProcessPathByPid(const int64_t pid)
{
    auto hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess == NULL)
    {
        qCritical() << "OpenProcess failed. Error code:" << GetLastError();
        return {};
    }

    TCHAR path[MAX_PATH];
    DWORD pathSize = MAX_PATH;

    QString result;

    if (QueryFullProcessImageName(hProcess, 0, path, &pathSize))
    {
        result = QString::fromWCharArray(path, pathSize);
    }
    else
    {
        qCritical() << "QueryFullProcessImageName failed. Error code:" << GetLastError();
    }

    CloseHandle(hProcess);

    return result;
}

SpecialDirs ToolHelpManager::getSpecialDir(const QString &path)
{
    static QRegExp rxInSystem32("[cC]:[\\\\/]windows[\\\\/]system32[\\\\/].+");
    rxInSystem32.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    static QRegExp rxInSysWOW64("[cC]:[\\\\/]windows[\\\\/]syswow64[\\\\/].+");
    rxInSysWOW64.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    static QRegExp rxInWindows("[cC]:[\\\\/]windows[\\\\/].+");
    rxInWindows.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    if (rxInSystem32.indexIn(path) != -1)
    {
        return SpecialDirs::WindowsSystem32;
    }
    else if (rxInSysWOW64.indexIn(path) != -1)
    {
        return SpecialDirs::WindowsSysWOW64;
    }
    else if (rxInWindows.indexIn(path) != -1)
    {
        return SpecialDirs::Windows;
    }
    else
    {
        return SpecialDirs::None;
    }
}

bool ToolHelpManager::getIsAdministrator() const
{
    return _isAdministrator;
}
