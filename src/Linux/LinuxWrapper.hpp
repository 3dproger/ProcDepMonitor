#ifndef LINUXWRAPPER_HPP
#define LINUXWRAPPER_HPP

#include "OSWrapper.hpp"

class LinuxWrapper : public OSWrapper
{
public:
    LinuxWrapper();
    QList<OSProcessInfo> getProcesses() override;
    OSProcessInfo processByPID(int64_t pid) override;

private:
    QList<OSProcessInfo> _processes;
    void addDep(QList<OSProcessDependence>& list, const OSProcessDependence& dep);
};

#endif // LINUXWRAPPER_HPP
