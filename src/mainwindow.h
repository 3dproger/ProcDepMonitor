#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "I18nManager.h"
#include "dialogselectprocess.h"
#include <QLabel>
#include "OSWrapper.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAboutQt_triggered();
    void on_actionAboutSoftware_triggered();
    void on_actionExit_triggered();
    void on_btnSelectProcess_clicked();

    void onPidSelected(int64_t pid);

    void on_btnShowInExplorer_clicked();

    void on_tableWidgetResult_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_tableWidgetResult_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidgetResult_customContextMenuRequested(const QPoint &pos);

    void copyPath();

    void copyName();

    void on_btnUpdateList_clicked();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::MainWindow *ui;
    I18nManager _i18n;
    DialogSelectProcess *_dialogSelectProcess = nullptr;

    QLabel* _labelStatus = nullptr;

    QMenu* _itemContextMenu = nullptr;

    int64_t _pid = -1;
};

#endif // MAINWINDOW_H
