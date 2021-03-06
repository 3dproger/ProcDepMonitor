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


    //RX
    _rxInSystem32 = QRegExp("[cC]:[\\\\/]windows[\\\\/]system32[\\\\/].+");
    _rxInSystem32.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    _rxInSysWOW64 = QRegExp("[cC]:[\\\\/]windows[\\\\/]syswow64[\\\\/].+");
    _rxInSysWOW64.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);

    _rxInWindows = QRegExp("[cC]:[\\\\/]windows[\\\\/].+");
    _rxInWindows.setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
}

QList<OSProcessInfo> ToolHelpManager::processes()
{
    _processes.clear();

    QList<TaskListManager::Task> tasks = TaskListManager::getList();

    for (const TaskListManager::Task& task : tasks)
    {
        OSProcessInfo info = _processByPID(uint32_t(task.processId));

        if (info.name.isEmpty())
        {
            info.name = task.processName;
        }

        _processes.append(info);
    }

    std::sort(_processes.begin(), _processes.end(), OSWrapper::compareOSProcessInfo);

    return _processes;
}

OSProcessInfo ToolHelpManager::processByPIDImpl(int64_t pid)
{
    OSProcessInfo info = _processByPID(pid);

    if (info.name.isEmpty())
    {
        const QList<TaskListManager::Task>& tasks = TaskListManager::getList();

        for (const TaskListManager::Task& task : tasks)
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

OSProcessInfo ToolHelpManager::_processByPID(int64_t pid)
{
    OSProcessInfo info;

    info.id = pid;

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    //Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot (TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, uint32_t(pid));
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
    info.canGetDependencies = true;
    //  Now walk the module list of the process,
    //  and display information about each module
    do
    {
        OSProcessDependence dep;

        dep.valid = true;
        dep.name = QString::fromWCharArray(me32.szModule);
        dep.fileName = QString::fromWCharArray(me32.szExePath);

        if (dep.fileName.startsWith("\\\\?\\"))
        {
            dep.fileName = dep.fileName.mid(4);
        }

        dep.specialDir = getSpecialDir(dep.fileName);

        info.dependencies.append(dep);
    }
    while(Module32Next(hModuleSnap, &me32));

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

SpecialDirs ToolHelpManager::getSpecialDir(const QString &path)
{
    if (_rxInSystem32.indexIn(path) != -1)
    {
        return SpecialDirs::WindowsSystem32;
    }
    else if (_rxInSysWOW64.indexIn(path) != -1)
    {
        return SpecialDirs::WindowsSysWOW64;
    }
    else if (_rxInWindows.indexIn(path) != -1)
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
