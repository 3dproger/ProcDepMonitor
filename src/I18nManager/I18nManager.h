#ifndef I18NMANAGER_H
#define I18NMANAGER_H

#include <QTranslator>
#include <QIcon>
#include <QLocale>
#include <QAction>

class I18nManager : public QObject
{
    Q_OBJECT

    struct LocaleInfo{
        QIcon icon;
        QString name;
        QLocale locale;
    };

    enum IconSize {_16x16};

public:
    I18nManager();
    ~I18nManager();
    void changeLocale(const QLocale& locale);
    QIcon findFlag(const QLocale& locale, const IconSize& size, bool* found = nullptr);
    QIcon currentIcon();
    QList<LocaleInfo> availableLocales();
    bool isAvailableLocale(const QLocale& locale);
    QList<QAction*> buildActions(QObject* parent = nullptr);

private slots:
    void changeLocalBySenderData();

private:
    void deleteTranslators();
    QString firstUpperCase(QString line);

    QTranslator* _translatorQt   = nullptr;//Translates standard Qt strings
    QTranslator* _translatorApp = nullptr;//Translates user application strings

    QIcon _currentIcon;

    QString _settingNameLocale = QString(typeid (*this).name()) + "/last";//Where to save last selected locale
    QString _userQmFilesDir = ":";
    QString _userQmFilePrefix = "app_";

    QList<LocaleInfo> _availableLocales;
};

#endif // I18NMANAGER_H
