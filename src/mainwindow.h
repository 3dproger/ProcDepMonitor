/*
 * MIT License

 * Copyright (c) 2020 Alexander Kirsanov

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * */

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

    static bool showInGraphicalShell(const QString &path);

private:
    void updateDepsList();

    Ui::MainWindow *ui;
    DialogSelectProcess *_dialogSelectProcess = nullptr;

    QLabel* _labelStatus = nullptr;

    QMenu* _itemContextMenu = nullptr;

    int64_t _pid = -1;
};

#endif // MAINWINDOW_H
