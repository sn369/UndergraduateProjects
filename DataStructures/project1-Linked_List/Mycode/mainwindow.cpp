#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "func.cpp"
#include <memory>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
    p = true;
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::addchar(QChar ch){
    if (p){
        QString str = ui->lineEdit->text();
        str = str + ch;
        ui->lineEdit->setText(str);
    }
    else{
        QString str = ui->lineEdit_2->text();
        str = str + ch;
        ui->lineEdit_2->setText(str);
    }
}

void MainWindow::on_pushButton_clicked(){
    p = p^true;
    if (p)
        ui->pushButton->setText("单击以输入B");
    else
        ui->pushButton->setText("单击以输入A");
}

void MainWindow::on_pushButton_0_clicked(){
    addchar('0');
}

void MainWindow::on_pushButton_1_clicked(){
    addchar('1');
}

void MainWindow::on_pushButton_2_clicked(){
    addchar('2');
}

void MainWindow::on_pushButton_3_clicked(){
    addchar('3');
}

void MainWindow::on_pushButton_4_clicked(){
    addchar('4');
}

void MainWindow::on_pushButton_5_clicked(){
    addchar('5');
}

void MainWindow::on_pushButton_6_clicked(){
    addchar('6');
}

void MainWindow::on_pushButton_7_clicked(){
    addchar('7');
}

void MainWindow::on_pushButton_8_clicked(){
    addchar('8');
}

void MainWindow::on_pushButton_9_clicked(){
    addchar('9');
}

void MainWindow::on_pushButton_pos_clicked(){
    addchar('+');
}

void MainWindow::on_pushButton_neg_clicked(){
    addchar('-');
}

void MainWindow::on_pushButton_point_clicked(){
    addchar('.');
}

void MainWindow::on_pushButton_sign_clicked(){
   addchar('x');
   addchar('^');
}

void MainWindow::on_pushButton_back_clicked(){
    if(p){
        QString str = ui->lineEdit->text();
        str = str.left(str.length() - 1);
        ui->lineEdit->setText(str);
    }
    else{
        QString str = ui->lineEdit_2->text();
        str = str.left(str.length() - 1);
        ui->lineEdit_2->setText(str);
    }
}

void MainWindow::on_pushButton_clear_clicked(){
    if(p){
        ui->lineEdit->setText("");
    }
    else{
        ui->lineEdit_2->setText("");
    }
    ui->lineEdit_4->setText("");
}

void MainWindow::on_pushButton_exit_clicked(){
    close();
}

QString Result(Poly P){
    Term * pa = (P.head)->next;
        QString ans;
        if(pa==NULL) ans=QChar('0');
        for (;pa!=NULL;pa=pa->next)
            if (pa->coef != 0){
                if(pa->coef > 0)
                    ans = ans + QChar('+');
                if(pa->coef!=1 && pa->coef!=-1)
                    ans = ans + QString::number(pa->coef);
                if(pa->coef == -1)
                    ans = ans + "-";
                if (pa->expn != 0 && pa->expn != 1)
                    ans = ans + QString("x^") + QString::number(pa->expn);
                else if (pa->expn == 1)
                    ans = ans + QString ("x");
                else if (pa->expn == 0 && (pa->coef==1 || pa->coef==-1))
                    ans = ans + '1';
            }
        if (ans[0] == '+')
            ans = ans.right(ans.length()-1);
    return ans;
}

//建立多项式
Poly CreatePoly(QString str){
    Term *p=new Term;
    p->next=NULL;
    int begin = 0, i = 0, expn = 0;
    double coef = 0;
    Poly P;

    while(i < str.length()){//大循环
        bool flag = true;

        //系数的循环
        while(str[i].isDigit()|| str[i] == '.' ||flag&&(str[i] == '+'||str[i] == '-')&&i<str.length()){
            flag = false;//正在读取中，flag设为false
            i++;
        }
        coef = str.mid(begin,i-begin).toDouble();//系数读取完毕
        if (str[i] == 'x'){//如果为x，则直接跳到指数部分
            i += 2;
            begin = i;
        }
            else{//系数后无x，意为该项为常数项，故指数为0
                begin = i;
                P.Insert(coef,0);
                flag = true;
                continue;  //本项插入完毕，跳出本次循环，再次开始大循环
            }

        flag = true; //开始读取非0指数

        //指数循环
        while(str[i].isDigit()||flag&&(str[i] == '+'||str[i] == '-')&&i<str.length()){
            if (!flag) //非首位
                if (str[i] == '+'||str[i] == '-')//为符号，说明已经进入下一项的系数部分
                    break;                         //意味指数读取完毕，直接跳出指数循环
            flag = false;//正在读取中，flag设为false
            i++;
        }

        expn = str.mid(begin,i-begin).toInt();
        P.Insert (coef,expn);
        begin = i;
    }//大循环结束
    return P;
}

void MainWindow::on_pushButton_value_clicked(){
    QString str1 = ui->lineEdit->text();
    Poly P=CreatePoly(str1);
    double x=ui->lineEdit_2->text().toDouble();
    ui->lineEdit_4->setText("A的值： "+QString::number(P.Value(x)));
    P.~Poly();
}

void MainWindow::on_pushButton_deriv_clicked(){
    QString str1 = ui->lineEdit->text();
    Poly P=CreatePoly(str1);
    Poly P1=P.Derivative();
    ui->lineEdit_4->setText("A的求导： "+Result(P1));
    P.~Poly();
    P1.~Poly();
}

void MainWindow::on_pushButton_add_clicked(){
    QString str1 = ui->lineEdit->text();
    QString str2 = ui->lineEdit_2->text();
    Poly Pa=CreatePoly(str1);
    Poly Pb=CreatePoly(str2);
    ui->lineEdit_4->setText("A+B： "+Result(Pa+Pb));
    Pa.~Poly();
    Pb.~Poly();
}

void MainWindow::on_pushButton_sub_clicked(){

    QString str1 = ui->lineEdit->text();
    QString str2 = ui->lineEdit_2->text();
    Poly Pa=CreatePoly(str1);
    Poly Pb=CreatePoly(str2);
    ui->lineEdit_4->setText("A-B： "+Result(Pa-Pb));
    Pa.~Poly();
    Pb.~Poly();
}

void MainWindow::on_pushButton_time_clicked(){

    QString str1 = ui->lineEdit->text();
    QString str2 = ui->lineEdit_2->text();
    Poly Pa=CreatePoly(str1);
    Poly Pb=CreatePoly(str2);
    ui->lineEdit_4->setText("A*B： "+Result(Pa*Pb));
    Pa.~Poly();
    Pb.~Poly();
}

void MainWindow::on_pushButton_print_clicked(){
    QString str1 = ui->lineEdit->text();
    Poly Pa=CreatePoly(str1);
    QString poly=QString::number(Pa.Getsize());
    Term*p = Pa.head->next;
    while (p) {
        poly= poly+", "+QString::number(int(p->coef))+ ", " +QString::number(p->expn);
        p = p->next;
    }
    ui->lineEdit_4->setText("A的标准输出： "+poly);
    Pa.~Poly();
}
