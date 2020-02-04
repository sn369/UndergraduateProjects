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
    Ui::MainWindow *ui;
    double read_Expression(QString str);

private:
    void addchar(QChar ch);
	
private slots:

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

    void on_pushButton_add_clicked();

    void on_pushButton_sub_clicked();

    void on_pushButton_time_clicked();

    void on_pushButton_div_clicked();

    void on_pushButton_double_clicked();

    void on_pushButton_back_clicked();

    void on_pushButton_pt_clicked();

    void on_pushButton_lb_clicked();

    void on_pushButton_rb_clicked();

    void on_pushButton_end_clicked();

    void on_pushButton_clear_clicked();
	
	void on_pushButton_eq_clicked();

    void on_pushButton_op_clicked();

    void on_pushButton_a_clicked();
};

#endif // MAINWINDOW_H
