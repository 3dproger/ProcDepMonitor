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

#ifndef DIALOGSELECTPROCESS_H
#define DIALOGSELECTPROCESS_H

#include <QDialog>
#include <QTableWidgetItem>

//ToDo: после нажатия сортировки по алфавиту обновление списка происходит с ошибкой

namespace Ui {
class DialogSelectProcess;
}

class DialogSelectProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectProcess(QWidget *parent = nullptr);
    ~DialogSelectProcess();

    QString processNameSelected() const;

    int64_t pid() const;

signals:
    void pidSelected(int64_t pid);

public slots:
    void updateList();

protected:
    void changeEvent(QEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void on_btnCancel_clicked();
    void on_btnSelect_clicked();

    void on_tableWidgetProcesses_itemDoubleClicked(QTableWidgetItem *item);

    void on_btnUpdateList_clicked();

private:
    Ui::DialogSelectProcess *ui;

    int64_t _pid = -1;
    QString _processNameSelected;
};

#endif // DIALOGSELECTPROCESS_H
