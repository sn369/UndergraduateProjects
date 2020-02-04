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
    bool p;
    void addchar(QChar ch);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_0_clicked();

    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_pos_clicked();

    void on_pushButton_neg_clicked();

    void on_pushButton_sign_clicked();

    void on_pushButton_back_clicked();

    void on_pushButton_point_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_exit_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_sub_clicked();

    void on_pushButton_time_clicked();

    void on_pushButton_print_clicked();

    void on_pushButton_deriv_clicked();

    void on_pushButton_value_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
