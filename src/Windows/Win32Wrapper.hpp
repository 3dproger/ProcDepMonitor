#ifndef WIN32WRAPPER_HPP
#define WIN32WRAPPER_HPP

#include "OSWrapper.hpp"
#include <windows.h>

class Win32Wrapper : public OSWrapper
{
public:
    QList<OSProcessInfo> getProcesses() override;
    OSProcessInfo processByPID(int64_t pid) override;
    Win32Wrapper();
    bool getIsAdministrator() const;

private:
    OSProcessInfo getProcessInfo(DWORD processID);

    SpecialDirs getSpecialDir(const QString& path);

    QRegExp _rxInSystem32;
    QRegExp _rxInSysWOW64;
    QRegExp _rxInWindows;

    QList<OSProcessInfo> _processes;

    bool _isAdministrator = false;

    void addDep(QList<OSProcessDependence>& list, const OSProcessDependence& dep);
};

#endif // WIN32WRAPPER_HPP
