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

private:
    Ui::MainWindow *ui;

private slots:
    void on_mid_clicked();
    void on_clear_clicked();
    void on_pre_clicked();
    void on_pre_2_clicked();
    void on_value_clicked();
    void on_assign_clicked();
    void on_add_clicked();
    void on_sub_clicked();
    void on_time_clicked();
    void on_div_clicked();
    void on_merge_clicked();
    void on_diff_clicked();
};

#endif // MAINWINDOW_H
