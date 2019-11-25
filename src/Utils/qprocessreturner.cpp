#include "qprocessreturner.h"
#include <QProcess>

QByteArray QProcessReturner::start(const QString &program, const QStringList &arguments, int *exitCode)
{
    QProcess process;

    process.setReadChannelMode(QProcess::MergedChannels);
    process.start(program, arguments, QProcess::ReadOnly);
    process.waitForFinished(-1);

    if (exitCode)
    {
        *exitCode = process.exitCode();
    }

    return process.readAllStandardOutput();
}

QProcessReturner::QProcessReturner()
{

}
