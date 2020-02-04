#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Print_Link_HashMap();
    void Print_HashMap();

private:
    Ui::MainWindow *ui;
private slots:

    void on_LINEAR_clicked();
    void on_DOUBLE_clicked();
    void on_RANDOM_clicked();
    void on_LINK_clicked();
    void on_SEARCH_clicked();
    void on_INPUT_clicked();
};

#endif // MAINWINDOW_H
