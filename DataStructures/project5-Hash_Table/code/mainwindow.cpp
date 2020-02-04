#include "mainwindow.h"
#include "ui_mainwindow.h"

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

#include <iostream>
#include <fstream>
#include <QString>
#include <QFile>
#include <QChar>
#include <QDebug>

//存储内容结构
struct Student {
    int key;
    QString name;
};

struct Hash {
    int key;
    int hash; //初始位置
    int rehash; //再散列值
    QString name;
    int len; //查找长度
};

Student stu[30];
void Inital_stu() {
    int i = 0;
    QFile file("Stu_name.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    while(!file.atEnd()) {
        QString temp=file.readLine();
        stu[i].name = temp.mid(0,temp.length()-1);
        int sum = 0;
        for (int j = 0; j<temp.length()-1; j++) {
            char c= temp[j].toLatin1();
            sum+=toascii(c);
        }
        stu[i].key = sum-32;
        i++;
    }
    file.close();
}

struct Node{
    QString name;
    int key;
    int len;
    int hash;
    Node *next;
};

Node LinkHash[40];

Hash ha[40];
void Inital_hash(){
    for(int i=0;i<40;i++){
        ha[i].len=0;
        ha[i].key=0;
        ha[i].name="";
        ha[i].rehash=0;
        ha[i].hash=0;
        LinkHash[i].len=0;
        LinkHash[i].next=NULL;
    }
    return;
}

void Random_HashMap(){ //伪随机探测再散列法处理冲突
    for(int i=0;i<30;i++){
        int search=1;
        int hashi=(stu[i].key)%37;
        int d=(7+stu[i].key)%10+1; //伪随机数序列
        if(ha[hashi].len!=0){ //冲突
            int finh;// 最终地址
            do{
                finh=(hashi+d)%40; // 伪随机探测再散列法处理冲突
                search++; // 查找次数加
                hashi=finh;
            }while(ha[hashi].len !=0);
        }
        ha[hashi].key=stu[i].key;
        ha[hashi].hash=(stu[i].key)%37;
        ha[hashi].rehash=d;
        ha[hashi].name=stu[i].name;
        ha[hashi].len=search;
   }
}

void Linear_HashMap(){ //线性探测再散列法处理冲突
    for(int i=0;i<30;i++){
        int search=1;
        int hashi=((stu[i].key)%37);
        int d=0;
        if(ha[hashi].len!=0){ //冲突
            int finh;// 最终地址
            do{
                d++; //线性探测再散列
                finh=(hashi+d)%40; // 线性探测再散列法处理冲突
                search++; // 查找次数加
            }while(ha[finh].len !=0);
            hashi=finh;
        }
        ha[hashi].key=stu[i].key;
        ha[hashi].hash=(stu[i].key)%37;
        ha[hashi].rehash=d;
        ha[hashi].name=stu[i].name;
        ha[hashi].len=search;
    }
}

void Quadratic_HashMap(){ //二次探测再散列法处理冲突
    for(int i=0;i<30;i++){
        int search=1;
        int hashi=(stu[i].key)%37;
        int d=0;
        if(ha[hashi].len!=0){ //冲突
            int finh;// 最终地址
            do{
               d=pow(-1,search+1)*pow(search,2);
               finh=(hashi+40+d)%40; // 二次探测再散列法处理冲突
               search++; // 查找次数加
            }while(ha[finh].len !=0);
            hashi=finh;
        }
        ha[hashi].key=stu[i].key;
        ha[hashi].hash=(stu[i].key)%37;
        ha[hashi].rehash=d;
        ha[hashi].name=stu[i].name;
        ha[hashi].len=search;
    }
}

void Link_HashMap(){ //链地址法处理冲突
    for(int i=0;i<30;i++){
        int search=1;
        int hashi=((stu[i].key)%37);
        if(LinkHash[hashi].len!=0){ //冲突
            search++;
            Node *p=&LinkHash[hashi];
            Node *ptr=new Node;
            while(p->next!=NULL){
                p=p->next;
                search++;
            }
            ptr->next=p->next;
            p->next=ptr;
            ptr->key=stu[i].key;
            ptr->name=stu[i].name;
            ptr->len=search;
            ptr->hash=hashi;
        }
        else{ //不冲突
            LinkHash[hashi].key=stu[i].key;
            LinkHash[hashi].name=stu[i].name;
            LinkHash[hashi].len=1;
            LinkHash[hashi].hash=hashi;
        }
    qDebug()<<stu[i].name<<endl;
    }
}

void MainWindow:: Print_HashMap(){ //用于打印开放地址法处理冲突的哈希表
    //--------------↓初始化表格---------------------
    ui->tableWidget->clear();//清除所有可见数据
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(6);
    QStringList headerString;
    headerString<<"哈希地址"<<"key"<<"初次散列"<<"增量"<<"查找长度"<<"name";
    ui->tableWidget->setHorizontalHeaderLabels(headerString);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setVisible(false);
    //--------------↑初始化表格---------------------
    for(int i=0; i<40; i++){
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem((QString::number(i)+"  ")));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem((QString::number(ha[i].key))+"  "));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem((QString::number(ha[i].hash))+"  "));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem((QString::number(ha[i].rehash))+"  "));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem((QString::number(ha[i].len))+"  "));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(ha[i].name));
    }
    float average=0;
    for(int i=0;i<40;i++) average+=ha[i].len;
        average/=30;
    QString ASL = QString("%1").arg(average);
    ui->ASL_label->setText("平均查找长度ASL="+ASL);
}

void MainWindow:: Print_Link_HashMap(){ //用于打印链地址法处理冲突的哈希表
    //--------------↓初始化表格---------------------
    ui->tableWidget->clear();//清除所有可见数据
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(7);
    QStringList headerString;
    headerString<<"哈希地址"<<"key1"<<"name1"<<"key2"<<"name2"<<"key3"<<"name3";
    ui->tableWidget->setHorizontalHeaderLabels(headerString);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget->verticalHeader()->setVisible(false);
    //--------------↑初始化表格---------------------
    float average=0;
    for(int i=0; i<40; i++){
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(i)));
        int j=1;
        for(Node *p=&LinkHash[i];p;p=p->next){
           ui->tableWidget->setItem(i,j,new QTableWidgetItem((QString::number(p->key))+"  "));
           ui->tableWidget->setItem(i,j+1,new QTableWidgetItem(p->name));
           j+=2;
           average+=p->len;
        }
    }
    average/=30;
    QString ASL = QString("%1").arg(average);
    ui->ASL_label->setText("平均查找长度ASL="+ASL);
}

int choose=0;
void MainWindow::on_LINK_clicked(){
    Inital_stu();
    Inital_hash();
    Link_HashMap();
    Print_Link_HashMap();
    choose=1;
    ui->label->setText("哈希表-链地址法");
}

void MainWindow::on_LINEAR_clicked(){
    Inital_stu();
    Inital_hash();
    Linear_HashMap();
    Print_HashMap();
    choose=2;
    ui->label->setText("哈希表-线性探测");
}

void MainWindow::on_DOUBLE_clicked(){
    Inital_stu();
    Inital_hash();
    Quadratic_HashMap();
    Print_HashMap();
    choose=3;
    ui->label->setText("哈希表-二次探测");
}

void MainWindow::on_RANDOM_clicked(){
    Inital_stu();
    Inital_hash();
    Random_HashMap();
    Print_HashMap();
    choose=4;
    ui->label->setText("哈希表-伪随机探测");
}

void MainWindow::on_INPUT_clicked(){
    ui->lineEdit->setReadOnly(false);
}

void MainWindow::on_SEARCH_clicked(){
    QString str=ui->lineEdit->text();
    int hash_addr;
    Node *find;
    int key = 0;
    for (int j = 0; j<str.length(); j++) {
        char c= str[j].toLatin1();
        key+=toascii(c);
    }
    key-=32;//获取到key
    int hash=key%37;
    bool success=0;
    ui->textBrowser->clear();
    switch(choose){
        case 1:{
            Node *p=&LinkHash[hash];
            while(p){
                if(p->name==str){
                    find=p;
                    success=1;
                    hash_addr=p->hash;
                    break;
                }
                else{
                    p=p->next;
                }
            }
            break;
        }

        case 2:{
            int d=0;
            int addr=hash+d;
            while(ha[addr].len!=0 && ha[addr].name!=str){
                ui->textBrowser->append("Hash:"+QString::number(addr));
                d++;
                addr=(hash+d)%40;
            }
            if(ha[addr].name==str){
                success=1;
            }
            hash_addr=addr;
            break;
        }

        case 3:{
            int cnt=1,d=0;
            int addr=hash+d;
            while(ha[addr].len!=0 && ha[addr].name!=str){
                ui->textBrowser->append("Hash:"+QString::number(addr));
                d=pow(-1,cnt+1)*pow(cnt,2);
                addr=(hash+d+40)% 40;
                cnt++;
            }
            if(ha[addr].name==str){
                success=1;
            }
            hash_addr=addr;
            break;
        }

        case 4:{
            int d=(7+key)%10+1;;
            int addr=hash;
            while(ha[addr].len!=0 && ha[addr].name!=str){
                ui->textBrowser->append("Hash:"+QString::number(addr));
                addr=(addr+d)%40;
            }
            if(ha[addr].name==str){
                success=1;
            }
            hash_addr=addr;
            break;
        }
    }

    if(success){
        ui->textBrowser->append("Hash:"+QString::number(hash_addr));
        ui->textBrowser->append("查找成功！（≧∇≦）");
        ui->textBrowser->append("哈希地址:"+QString::number(hash_addr));
        if(choose==1){
            ui->textBrowser->append("查找次数:"+QString::number(find->len));
            QTableWidgetItem *item = ui->tableWidget->item(hash_addr,find->len*2);
            item->setTextColor(QColor(250,128,10));
        }
        else{
            ui->textBrowser->append("查找次数:"+QString::number(ha[hash_addr].len));
            QTableWidgetItem *item = ui->tableWidget->item(hash_addr,5);
            item->setTextColor(QColor(250,128,10));
        }
    }
    else{
        ui->textBrowser->append("Hash:"+QString::number(hash_addr));
        ui->textBrowser->append("未找到结果~(Q A Q)~");
        ui->textBrowser->append("可插入的哈希地址:"+QString::number(hash_addr));
    }
}
