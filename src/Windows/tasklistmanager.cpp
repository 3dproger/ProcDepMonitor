#include "tasklistmanager.h"
#include "qprocessreturner.h"
#include <QDebug>

QList<TaskListManager::Task> TaskListManager::getList()
{
    #ifndef Q_OS_WINDOWS
        qDebug() << "This class does not work on Non-Windows systems";
    #endif

    QList<Task> list;

    int exitCode = -1;

    QByteArray output = QProcessReturner::start("cmd", {"/C", "tasklist /NH /FO CSV"}, &exitCode);
    //qDebug() << output;

    if (exitCode != 0)
    {
        qWarning() << "Exit code:" << exitCode;
    }

    output.replace("\r", "");//Remove all '\r' symbols

    QByteArrayList processOutputList = output.split('\n');

    QRegExp rx("\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\"");

    for (const QByteArray& ba : processOutputList)
    {
        if (rx.indexIn(ba) != -1)
        {
            Task task;

            //Process Name
            task.processName = rx.cap(1);

            //Process ID
            QString str = rx.cap(2);

            bool ok;
            int64_t pid = str.toLongLong(&ok);
            if (ok) { task.processId = pid; }

            //Session Name
            task.sessionName = rx.cap(3);

            //Session ID
            str = rx.cap(2);

            int64_t sid = str.toLongLong(&ok);
            if (ok) { task.sessionID = sid; }

            list.append(task);
        }
    }

    return list;
}

TaskListManager::TaskListManager()
{

}
