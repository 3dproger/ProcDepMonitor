#include "OSWrapper.hpp"
#if defined (Q_OS_WIN32)
//#include "Windows/Win32Wrapper.hpp"
#include "Windows/ToolHelpManager.hpp"
#elif defined(Q_OS_LINUX)
#include "Linux/LinuxWrapper.hpp"
#endif

OSWrapper &OSWrapper::instance()
{
    //It is signleton
    #if defined (Q_OS_WIN32)
    static ToolHelpManager instance;
    //static Win32Wrapper instance;
    #elif defined(Q_OS_LINUX)
    static LinuxWrapper instance;
    #else
    qFatal("This OS is not yet supported");
    static OSWrapper instance;
    #endif

    return instance;
}

QList<OSProcessInfo> OSWrapper::getProcesses()
{
    return QList<OSProcessInfo>();
}

OSProcessInfo OSWrapper::processByPID(int64_t)
{
    qDebug("the function processByPID(int64_t pid) must be overridden in the heir classes!");
    return OSProcessInfo();
}

bool OSWrapper::sortOSProcessInfo(const OSProcessInfo &info1, const OSProcessInfo &info2)
{
    //By canGetDependencies
    if (info1.canGetDependencies && !info2.canGetDependencies)
    {
        return true;
    }
    else if (!info1.canGetDependencies && info2.canGetDependencies)
    {
        return false;
    }
    else if (!info1.canGetDependencies && !info2.canGetDependencies)
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

bool OSWrapper::sortOSProcessDependence(const OSProcessDependence &dep1, const OSProcessDependence &dep2)
{
    if (dep1.specialDir != dep2.specialDir)
    {
        return dep1.specialDir < dep2.specialDir;
    }

    #ifdef Q_OS_WIN32
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
