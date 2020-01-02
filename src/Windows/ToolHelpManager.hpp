#ifndef TOOLHELPMANAGER_HPP
#define TOOLHELPMANAGER_HPP

#include "OSWrapper.hpp"

class ToolHelpManager : public OSWrapper
{
public:
    ToolHelpManager();
    QList<OSProcessInfo> getProcesses() override;
    OSProcessInfo processByPID(int64_t pid) override;

    bool getIsAdministrator() const;

private:
    QList<OSProcessInfo> _processes;

    void addDep(QList<OSProcessDependence>& list, const OSProcessDependence& dep);

    SpecialDirs getSpecialDir(const QString& path);

    QRegExp _rxInSystem32;
    QRegExp _rxInSysWOW64;
    QRegExp _rxInWindows;

    bool _isAdministrator = false;
};

#endif // TOOLHELPMANAGER_HPP
