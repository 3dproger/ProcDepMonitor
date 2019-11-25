#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QJsonArray>
#include <QByteArrayList>
#include <QColor>

#ifdef QT_WIDGETS_LIB
#include <QPushButton>
#include <QToolButton>
#endif

class Utils
{
public:
    Utils();

    enum EncodeAlgorithm {EncoderV1};

    static QString mergePaths(QString path1, QString path2);

    static QString getFileNameFromPath(QString filePath);
    static QString getFileNameFromPathWitoutExtention(QString filePath);
    static QString getFileExtension(QString path);
    static QString fromUChar(const unsigned char *str);  

    static QString removeFirstSpaces(QString s);
    static QString removeLastSpaces(QString s);
    static QByteArray removeFirstSpaces(QByteArray ba);
    static QByteArray removeLastSpaces(QByteArray ba);


    static float angle360(float x1, float y1, float x2, float y2);
    static float angle180(float x1, float y1, float x2, float y2);

    #ifdef QT_WIDGETS_LIB
    static void setButtonColor(QPushButton* button, const QColor& color);
    static void setButtonColor(QToolButton* button, const QColor& color);
    static void moveButtonCenter(QToolButton* button, int x, int y);
    #endif

    /*static void   saveColor(const QString& name, const QColor& color);
    static QColor loadColor(const QString& name, const QColor& defaultColor);*/

    static QString fromJSONArray(const QJsonArray& array);
    static QString prepareFloat(float value);
    static double getNumber(QByteArray s, int pos, bool *ok = nullptr);
    static float  getNumberf(QByteArray s, int pos, bool *ok = nullptr);
    static QByteArrayList* prepareBAList(const QString& gcode);

    static QByteArray encode(const QByteArray& ba, int seed, EncodeAlgorithm algorithm);
    static QByteArray decode(const QByteArray& ba, int seed, EncodeAlgorithm algorithm);

    static bool containsLeft (QString string, QString substring, bool registerSensetive = true);
    static bool containsLeft (QByteArray ba, QByteArray subba, bool registerSensetive = true);

    static QColor colorMix(const QList<QColor> &colors);

    static int getQuadrantDeg(float angle);
    static int getQuadrantRad(float angle);

    static QString minimizeStringFloatNumber(QString text);

    static QString findFile(const QStringList& paths, const QString& fileName);

    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    static QString removeLeft(const QString& string, const QString& subString);

    static bool showInGraphicalShell(const QString &path);
};

#endif // UTILS_H
