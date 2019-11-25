#ifndef TASKLISTMANAGER_H
#define TASKLISTMANAGER_H

#include <QList>

class TaskListManager
{
public:
    struct Task
    {
        QString processName;

        int64_t processId = -1;

        QString sessionName;
        int64_t sessionID = -1;
    };

    static QList<Task> getList();

protected:
    TaskListManager();
};

#endif // TASKLISTMANAGER_H
