#include "Win32Wrapper.hpp"
#include <psapi.h>
#include <QDebug>
#include "Windows/tasklistmanager.h"
#include <QSettings>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

Win32Wrapper::Win32Wrapper()
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

QList<OSProcessInfo> Win32Wrapper::getProcesses()
{
    _processes.clear();

    QList<TaskListManager::Task> tasks = TaskListManager::getList();

    for (const TaskListManager::Task& task : tasks)
    {
        OSProcessInfo info = getProcessInfo(uint32_t(task.processId));

        if (info.name.isEmpty())
        {
            info.name = task.processName;
        }

        _processes.append(info);
    }

    std::sort(_processes.begin(), _processes.end(), OSWrapper::sortOSProcessInfo);

    return _processes;
}

OSProcessInfo Win32Wrapper::processByPID(int64_t pid)
{
    OSProcessInfo info;

    if (_processes.isEmpty())
    {
        _processes = getProcesses();
    }

    for (const OSProcessInfo& p : _processes)
    {
        if (p.id == pid)
        {
            return p;
        }
    }

    return info;
}

OSProcessInfo Win32Wrapper::getProcessInfo(DWORD processID)
{
    OSProcessInfo info;

    //Process ID
    info.id = processID;
    info.valid = true;

    HANDLE hProcess = OpenProcess(/*PROCESS_QUERY_LIMITED_INFORMATION*/ PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                  FALSE,
                                  processID);

    if (hProcess)
    {
        //Name and fileName

        //Plan A: Proccess name from GetProcessImageFileName
        if (info.name.isEmpty())
        {
            WCHAR lpstrName[MAX_PATH];

            GetProcessImageFileName(hProcess, lpstrName, sizeof(lpstrName)/sizeof(*lpstrName));

            info.fileName = QString::fromWCharArray(lpstrName);
            info.name = info.fileName;

            if (info.name.contains('\\'))
            {
                info.name = info.name.mid(info.name.lastIndexOf('\\') + 1);
            }
        }

        //Plan B for getting name from GetModuleBaseName
        if (info.name.isEmpty())
        {
            HMODULE hMod;
            DWORD cbNeeded;
            TCHAR szProcessName[MAX_PATH] = TEXT("");
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded) )
            {
                GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR));
            }

            info.name = QString::fromWCharArray(szProcessName);
        }

        //Dependencies

        //Plan A: Dependence from EnumProcessModulesEx LIST_MODULES_32BIT and LIST_MODULES_64BIT
        if (!info.canGetDependencies)
        {
            HMODULE hMods[8192];
            DWORD cbNeeded;

            bool found32, found64;
            found32 = EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_32BIT);

            if (found32)
            {
                for (int64_t i  = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
                {
                    TCHAR szModName[MAX_PATH];

                    OSProcessDependence dep;

                    dep.valid = GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR));

                    // Get the full path to the module's file.

                    if (dep.valid)
                    {
                        dep.fileName = QString::fromWCharArray(szModName);
                        dep.name = dep.fileName;

                        if (dep.name.contains('\\'))
                        {
                            dep.name = dep.name.mid(dep.name.lastIndexOf('\\') + 1);
                        }
                    }

                    addDep(info.dependencies, dep);
                }
            }

            found64 = EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_64BIT);

            if (found64)
            {
                for (int64_t i  = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
                {
                    TCHAR szModName[MAX_PATH];

                    OSProcessDependence dep;

                    dep.valid = GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR));

                    // Get the full path to the module's file.

                    if (dep.valid)
                    {
                        dep.fileName = QString::fromWCharArray(szModName);
                        dep.name = dep.fileName;

                        if (dep.name.contains('\\'))
                        {
                            dep.name = dep.name.mid(dep.name.lastIndexOf('\\') + 1);
                        }
                    }

                    addDep(info.dependencies, dep);
                }
            }

            info.canGetDependencies = found32 | found64;
        }

        //Plan A: Dependence from EnumProcessModulesEx LIST_MODULES_ALL
        if (!info.canGetDependencies)
        {
            HMODULE hMods[8192];
            DWORD cbNeeded;

            info.canGetDependencies = EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL);

            if (info.canGetDependencies)
            {
                for (int64_t i  = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
                {
                    TCHAR szModName[MAX_PATH];

                    OSProcessDependence dep;

                    dep.valid = GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR));

                    // Get the full path to the module's file.

                    if (dep.valid)
                    {
                        dep.fileName = QString::fromWCharArray(szModName);
                        dep.name = dep.fileName;

                        if (dep.name.contains('\\'))
                        {
                            dep.name = dep.name.mid(dep.name.lastIndexOf('\\') + 1);
                        }
                    }

                    addDep(info.dependencies, dep);
                }
            }
        }

        //Plan B: Dependence from EnumProcessModules
        if (!info.canGetDependencies)
        {
            HMODULE hMods[8192];
            DWORD cbNeeded;

            info.canGetDependencies = EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded);

            if (info.canGetDependencies)
            {
                for (int64_t i  = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
                {
                    TCHAR szModName[MAX_PATH];

                    OSProcessDependence dep;

                    dep.valid = GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR));

                    // Get the full path to the module's file.

                    if (dep.valid)
                    {
                        dep.fileName = QString::fromWCharArray(szModName);
                        dep.name = dep.fileName;

                        if (dep.name.contains('\\'))
                        {
                            dep.name = dep.name.mid(dep.name.lastIndexOf('\\') + 1);
                        }
                    }

                    addDep(info.dependencies, dep);
                }
            }
        }

        //Close process
        CloseHandle(hProcess);
    }

    for (OSProcessDependence& dep : info.dependencies)
    {
        dep.specialDir = getSpecialDir(dep.fileName);
    }

    std::sort(info.dependencies.begin(), info.dependencies.end(), sortOSProcessDependence);

    return info;
}

SpecialDirs Win32Wrapper::getSpecialDir(const QString &path)
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

void Win32Wrapper::addDep(QList<OSProcessDependence> &list, const OSProcessDependence &dep)
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

bool Win32Wrapper::getIsAdministrator() const
{
    return _isAdministrator;
}

