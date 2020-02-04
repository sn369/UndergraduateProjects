#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QCharRef>
#include "func.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::addchar(QChar ch){
    if (ui->radioButton->isChecked()){ //若选择第一个文本框，则输入字符至第一个文本框
        QString str = ui->lineEdit_2->text();
        str = str + ch;
        ui->lineEdit_2->setText(str);
    }
    else{QString str = ui->lineEdit->text(); //否则输入字符至第二个文本框
        str = str + ch;
        ui->lineEdit->setText(str);
    }
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

void MainWindow::on_pushButton_add_clicked(){
    addchar('+');
}

void MainWindow::on_pushButton_sub_clicked(){
    addchar('-');
}

void MainWindow::on_pushButton_time_clicked(){
   addchar('*');
}

void MainWindow::on_pushButton_div_clicked(){
   addchar('/');
}

void MainWindow::on_pushButton_pt_clicked(){
    addchar('.');
}

void MainWindow::on_pushButton_lb_clicked(){
   addchar('(');
}


void MainWindow::on_pushButton_rb_clicked(){
   addchar(')');
}

void MainWindow::on_pushButton_end_clicked(){
   addchar('#');
}

void MainWindow::on_pushButton_double_clicked(){
   addchar('^');
   addchar('2');
}

void MainWindow::on_pushButton_a_clicked(){
   addchar('a');
}


void MainWindow::on_pushButton_back_clicked(){
    QString str = ui->lineEdit->text();
    str = str.left(str.length() - 1);
    ui->lineEdit->setText(str);
    ui->lcdNumber->display(0);
}

void MainWindow::on_pushButton_clear_clicked(){
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->lcdNumber->display(0);
    ui->tableWidget->clearContents();//只清除表中数据，不清除表头内容
    ui->tableWidget->setRowCount(0);//连行也清除掉
}

double value(QChar oper,double n1,double n2){//计算结果
    if(oper=='+') return n2+n1;
    if(oper=='-') return n2-n1;
    if(oper=='*') return n2*n1;
	if(oper=='/') return n2/n1;
	if(oper=='^') return n2*n2;
}


double evalue(QString str){ //若赋值为表达式，计算赋值结果
    Operator_Stack opStk;
    Number_Stack numStk;
    int begin = 0, i = 0;
    double n; //存储数字
    QChar op; //存储符号
    while(i<str.length()){
		while(str[i].isDigit()|| str[i] == '.'){//若字符为数字或点
			i++;
		}
		n = str.mid(begin,i-begin).toDouble(); //读出数字
		if(i!=begin){ //若读到了数字
			numNode* p=new numNode;
			p->num=n;
			p->next=NULL;
			numStk.push(p);
		} //压入数字栈

		bool r=false; //当前读取的操作符是否被处理过
			while(!r){
				op=str[i]; //读出操作符
				int flag=opStk.cmp(op,opStk.top->op);//与栈顶操作符比较
				if(flag==1){ //若新操作符比栈顶操作符优先级高
					opNode* p=new opNode;
					p->op=op;
					p->next=NULL;
					opStk.push(p); //压入操作符栈
					r=true;
				}

				if(flag==0){//若优先级相同
					QChar op=opStk.pop(); //栈顶操作符出栈
					r=true;
				}

				if(flag==-1){//若新操作符比栈顶操作符优先级低
					double num1=numStk.pop(); //弹出两个操作数
					double num2=numStk.pop();
					QChar oper=opStk.pop(); //弹出一个运算符
					double result=value(oper,num1,num2); //计算结果
					numNode* p=new numNode;
					p->num=result;
					p->next=NULL;
					numStk.push(p);//将结果压入数字栈
				}
			}
        i++;
        begin=i;
    }
    opStk.~Operator_Stack();
    numStk.~Number_Stack();
    return numStk.top->num;  //最后返回数字栈中的结果
}

//读取表达式
double MainWindow::read_Expression(QString str){
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
	Operator_Stack opStk;
	Number_Stack numStk;
	int begin = 0, i = 0,row=0;
	double n;
	QChar op;
	ui->tableWidget->insertRow(row);
	ui->tableWidget->setItem(row,0,new QTableWidgetItem(opStk.s));
	ui->tableWidget->setItem(row,1,new QTableWidgetItem(numStk.s));   //输出栈中的内容
	while(i<str.length()){
    while(str[i].isDigit()|| str[i] == '.'){//若字符为数字或点
        i++;
    }

    n = str.mid(begin,i-begin).toDouble(); //读出数字

    if(i!=begin){ //若读到了数字
		numNode* p=new numNode;
		p->num=n;
		p->next=NULL;
		numStk.push(p);
		ui->tableWidget->setItem(row,2,new QTableWidgetItem(QString::number(n)));
		ui->tableWidget->setItem(row,3,new QTableWidgetItem("压入"+QString::number(n))); //输出操作内容
		row++;
		ui->tableWidget->insertRow(row);
		ui->tableWidget->setItem(row,0,new QTableWidgetItem(opStk.s));
		ui->tableWidget->setItem(row,1,new QTableWidgetItem(numStk.s)); //输出栈中的内容
    } //压入数字栈

    if(str[i]=='a'){ //若读到了变量名
        numNode* p=new numNode;
        QString number=ui->lineEdit_2->text();
        double n=evalue(number);
        p->num=n;
        p->next=NULL;
        numStk.push(p);
        ui->tableWidget->setItem(row,2,new QTableWidgetItem(QString::number(n)));
        ui->tableWidget->setItem(row,3,new QTableWidgetItem("压入"+QString::number(n)));
        row++;
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0,new QTableWidgetItem(opStk.s));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(numStk.s));
        i++;
    }

    bool r=false; //当前读取的操作符是否被处理过
		while(!r){       
			op=str[i]; //读出操作符
			ui->tableWidget->setItem(row,2,new QTableWidgetItem(op));
			int flag=opStk.cmp(op,opStk.top->op);//与栈顶操作符比较
			if(flag==1){ //若新操作符比栈顶操作符优先级高
				opNode* p=new opNode;
				p->op=op;
				p->next=NULL;
				opStk.push(p); //压入操作符栈
				r=true;
				QString s="压入";
				s+=op;
				ui->tableWidget->setItem(row,3,new QTableWidgetItem(s));
				row++;
				ui->tableWidget->insertRow(row);
				ui->tableWidget->setItem(row,0,new QTableWidgetItem(opStk.s));
				ui->tableWidget->setItem(row,1,new QTableWidgetItem(numStk.s));
			}

			if(flag==0){//若优先级相同
				QChar op=opStk.pop(); //栈顶操作符出栈

				ui->tableWidget->setItem(row,2,new QTableWidgetItem(op));
				QString s="弹出";
				s+=op;
				ui->tableWidget->setItem(row,3,new QTableWidgetItem(s));
				row++;
				ui->tableWidget->insertRow(row);
				ui->tableWidget->setItem(row,0,new QTableWidgetItem(opStk.s));
				ui->tableWidget->setItem(row,1,new QTableWidgetItem(numStk.s));
				r=true;
			}

			if(flag==-1){//若新操作符比栈顶操作符优先级低
				double num1=numStk.pop(); //弹出两个操作数
				double num2=numStk.pop();

				QChar oper=opStk.pop(); //弹出一个运算符
				ui->tableWidget->setItem(row,2,new QTableWidgetItem(op));
				QString s="弹出";
				s+=oper;
				double result=value(oper,num1,num2); //计算结果
				ui->tableWidget->setItem(row,3,new QTableWidgetItem(s+", 压入"+QString::number(num2)+oper+QString::number(num1)));
				numNode* p=new numNode;
				p->num=result;
				p->next=NULL;
				numStk.push(p);//将结果压入数字栈
				row++;
				ui->tableWidget->insertRow(row);
				ui->tableWidget->setItem(row,0,new QTableWidgetItem(opStk.s));
				ui->tableWidget->setItem(row,1,new QTableWidgetItem(numStk.s));
			}
		}
		i++;
		begin=i;
    }
    opStk.~Operator_Stack();
    numStk.~Number_Stack();
    return numStk.top->num;
}

void MainWindow::on_pushButton_eq_clicked(){
    QString str=ui->lineEdit->text();
    double ans=read_Expression(str);
    ui->lcdNumber->display(ans);
}

void MainWindow::on_pushButton_op_clicked(){
    QString str=ui->lineEdit->text();
    double ans=read_Expression(str)*(-1);
    ui->lcdNumber->display(ans);
    ui->tableWidget->clearContents();//只清除表中数据，不清除表头内容
    ui->tableWidget->setRowCount(0);//连行也清除掉
}

