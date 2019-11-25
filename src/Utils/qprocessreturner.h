#ifndef QPROCESSRETURNER_H
#define QPROCESSRETURNER_H

#include <QStringList>

class QProcessReturner
{

public:
    static QByteArray start(const QString &program,
                         const QStringList &arguments = QStringList(),
                         int* exitCode = nullptr);

protected:
    QProcessReturner();
};

#endif // QPROCESSRETURNER_H
