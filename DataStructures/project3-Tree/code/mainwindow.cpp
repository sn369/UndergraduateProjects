#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "func.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

Expression E1,E2,E;
Expression ReadExpr(QString str){ //读取并存储表达式
    Expression E;
    Node *root=new Node;
    Node *p=root;
    SqStack<Node*> stk;
    int i=0;
    while(i<str.length()){
        if(str[i]=='+'||str[i]=='-'||str[i]=='*'||str[i]=='/'||str[i]=='^'){//若为运算符
            p->data=str[i];
            p->flag=1;
            stk.push(p);
            p->lchild=new Node;
            p=p->lchild;
            i++;
        }

        else if(str[i]=='S'||str[i]=='C'||str[i]=='T'){//若读取三角函数
                p->data=str.mid(i,2);
                p->flag=2;
                if(str[i+1].isDigit()){
                    p->flag=0;
                    int n=str.mid(i+1,1).toInt();
                    if(p->data[0]=='S'){
                        p->value=sin(n);
                    }
                    if(p->data[0]=='C'){
                        p->value=cos(n);
                    }
                    if(p->data[0]=='T'){
                        p->value=tan(n);
                    }
                }
                if(stk.isEmpty()) break;
                p=stk.gettop(); //返回父亲结点
                while(p->rchild){ //若父亲结点的右孩子不为空
                    stk.pop(); //弹出父亲结点
                    if(stk.isEmpty()) break; //若栈空，构造完成
                    p=stk.gettop(); //继续返回上一父亲结点
                }
                if(stk.isEmpty()) break; //若栈空，构造完成
                p->rchild=new Node;
                p=p->rchild;
                i+=2;
        }

            else{//若为数字或变量
            p->data=str[i];
            p->flag=2;
            if(str[i].isDigit()){
                int number=str.mid(i,1).toInt();
                p->value=number;
                p->flag=0;
            }
            if(stk.isEmpty()) break;
            p=stk.gettop(); //返回父亲结点
            while(p->rchild){ //若父亲结点的右孩子不为空
                stk.pop(); //弹出父亲结点
                if(stk.isEmpty()) break; //若栈空，构造完成
                p=stk.gettop(); //继续返回上一父亲结点
            }
            if(stk.isEmpty()) break; //若栈空，构造完成
            p->rchild=new Node;
            p=p->rchild;
            i++;
            }
    }//while
    if(stk.isEmpty()){
        E.root=root;
        return E;
    }
    else{
        E.flag=0;
        return E;
    }
}

void MainWindow::on_pre_clicked(){
    QString str=ui->lineEdit->text();
    E1=ReadExpr(str);
    if(E1.flag){
        ui->lineEdit_4->setText("Valid E1!");
        E=E1;
    }
    else
        ui->lineEdit_4->setText("Invalid!");
}

void MainWindow::on_pre_2_clicked(){
    QString str=ui->lineEdit_2->text();
    E2=ReadExpr(str);
    if(E2.flag){
        ui->lineEdit_4->setText("Valid E2!");
        E=E2;
    }
    else
       ui->lineEdit_4->setText("Invalid!");
}

void MainWindow::on_clear_clicked(){
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");
}

void MainWindow::on_mid_clicked(){
    ui->lineEdit_4->setText(WritrExpr(E.root));
}

void MainWindow::on_value_clicked(){
    float ans=E.value();
    ui->lineEdit_4->setText(WritrExpr(E.root)+"="+QString::number(ans));
}

void MainWindow::on_assign_clicked(){
    QString val=ui->lineEdit_3->text();
    int i=0;
    while(i<val.length()-2){
        E.Assign(val[i],val.mid(i+2,1).toInt());
        i+=4;
    }
    float ans=E.value();
    ui->lineEdit_4->setText(WritrExpr(E.root)+"="+QString::number(ans));
}

void MainWindow::on_add_clicked(){
    E=CompoundExpr("+",E1.root,E2.root);
    ui->lineEdit_4->setText(WritrExpr(E.root)+"="+QString::number(E1.value()+E2.value()));
}

void MainWindow::on_sub_clicked(){
    E=CompoundExpr("-",E1.root,E2.root);
    ui->lineEdit_4->setText(WritrExpr(E.root)+"="+QString::number(E1.value()-E2.value()));
}

void MainWindow::on_time_clicked(){
    E=CompoundExpr("*",E1.root,E2.root);
    ui->lineEdit_4->setText(WritrExpr(E.root)+"="+QString::number(E1.value()*E2.value()));
}

void MainWindow::on_div_clicked(){
    E=CompoundExpr("/",E1.root,E2.root);
    ui->lineEdit_4->setText(WritrExpr(E.root)+"="+QString::number(E1.value()/E2.value()));
}

void MainWindow::on_merge_clicked(){
    MergeConst(E.root);
    QString exp=WritrExpr(E.root);
    ui->lineEdit_4->setText(exp);
}

void MainWindow::on_diff_clicked(){
    QString val=ui->lineEdit_3->text();
    QString str=ui->lineEdit_4->text();
    QString exp=WritrExpr(E.root);
    exp+=")'="+Diff(str,val);
    ui->lineEdit_4->setText("("+exp);
}
