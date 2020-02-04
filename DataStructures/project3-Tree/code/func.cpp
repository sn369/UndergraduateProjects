#include <QString>
#include"math.h"

//二叉树的链式存储结构
struct Node{
    int flag;  //标记数据类型，变量为2，运算符为1，常量为0
    QString data;  //以字符串形式存储元素，方便输出
    float value=0;    //表示变量或常量的数值，用于赋值、计算
    Node *lchild, *rchild;  //左右孩子结点
    Node(QString s=0, Node *llink=NULL, Node *rlink=NULL){ //结点的构造函数
        data=s;
        lchild=llink;
        rchild=rlink;
    }
};

//顺序栈用来保存二叉树结点、运算数
template <class T> class SqStack {
public:
    T stack[30];
    int top=-1;
    T gettop(){
        return stack[top];
    }
    void pop(){
        top--;
    }
    void push(T item){
        stack[++top]=item;
    }
    bool isEmpty(){
        if(top==-1) return true;
        else return false;
    }
};

int cmp(QChar op1,QChar op2){//当前符号优先级高，返回1；低，返回-1；相等，返回0
    if(op2=='+'||op2=='-'){
        if(op1=='+'||op1=='-'||op1==')') return -1;
        if(op1=='*'||op1=='/'||op1=='('||op1=='^') return 1;
    }
    if(op2=='*'||op2=='/'){
        if(op1=='('||op1=='^') return 1;
        else return -1;
    }

    if(op2=='^'){
        if(op1=='(') return 1;
        else return -1;
    }

    if(op2=='('){
        if(op1==')') return 0;
        else return 1;
    }

    if(op2==')'){
        return -1;
    }
}

float val(QChar oper,float n1,float n2){//计算结果
    if(oper=='+') return n2+n1;
    if(oper=='-') return n2-n1;
    if(oper=='*') return n2*n1;
    if(oper=='/') return n2/n1;
    if(oper=='^') return pow(n2,n1);
}

class Expression{ //表达式类
    public:
    Expression(Node *p=NULL,int f=1){ //构造函数
        root=p;
        flag=f;
    }
    Node *root; //根结点
    int flag;   //是否有效

    void Assign(QChar V,int c);
    float value();
    friend void MergeConst(Node *p);
    friend float val(QChar oper,int n1,int n2);
    friend int cmp(QChar op1,QChar op2);
};

float Expression::value(){
    Node *p=root;
    SqStack<Node *> NoStack;  //二叉树结点栈
    SqStack<float> NumStack;   //运算数栈
    float n1,n2;
    QChar oper;
    int sign;
    if(p){
       do{
          while(p){
             NoStack.push(p);
             p=p->lchild;
          }
          Node *temp=NULL;
          sign=1;
          while(!NoStack.isEmpty()&&sign){
               p=NoStack.gettop();
               if(p->rchild==temp){
                  if(p->flag==1){
                     oper=p->data[0];
                     n1=NumStack.gettop();
                     NumStack.pop();
                     n2=NumStack.gettop();
                     NumStack.pop();
                     NumStack.push(val(oper,n1,n2));
                   }
                   else NumStack.push(p->value);
                   NoStack.pop();
                   temp=p;
                }
                else{
                     p=p->rchild;
                     sign=0;
                }
          }
        }while(!NoStack.isEmpty());
    }
    return NumStack.gettop();
}

void Expression::Assign(QChar V, int c){ //赋值
    Node *p=root;
    SqStack<Node *> s;
    while(!s.isEmpty()||p){
        while(p){
            s.push(p);
            p=p->lchild;
        }
        p=s.gettop();
        if(!s.isEmpty()){
            if(p->data==V){  //单个变量
                p->value=c;
            }
            if(p->data[1]==V){ //三角函数
                if(p->data[0]=='S'){
                    p->value=sin(c);
                }
                if(p->data[0]=='C'){
                    p->value=cos(c);
                }
                if(p->data[0]=='T'){
                    p->value=tan(c);
                }
            }//if
        }//if
        s.pop();
        p=p->rchild;
    }//while
    return;
}//Assign

QString WritrExpr(Node* root){ //中序遍历输出表达式
    QString exp;
    if(root){
        if(root->lchild&&root->lchild->flag==1){
            if(cmp(root->data[0],root->lchild->data[0])==1)
                exp+="("+WritrExpr(root->lchild)+")";
            else exp+=WritrExpr(root->lchild);
        }
        else exp+=WritrExpr(root->lchild); //遍历左子树
        exp+=root->data;
        if(root->rchild&&root->rchild->flag==1){
            if(cmp(root->data[0],root->rchild->data[0])==1)
                exp+="("+WritrExpr(root->rchild)+")";
            else exp+=WritrExpr(root->rchild);
        }
        else exp+=WritrExpr(root->rchild); //遍历右子树
    }
    return exp;
}

Expression CompoundExpr(QString P,Node *p1, Node *p2){  //合成新的表达式
    Node *root=new Node(P,p1,p2);
    Expression E(root);
    return E;
}

void MergeConst(Node *p){  //合并常数项
    float result;
    if(p->lchild&&p->rchild){
        if(p->lchild->flag==0&&p->rchild->flag==0){
            result=val(p->data[0],p->rchild->value,p->lchild->value);
            p->flag=0;
            p->value=result;
            p->data=QString::number(result);
            free(p->lchild);
            free(p->rchild);
            p->lchild=NULL;
            p->rchild=NULL;
        }
        else{
            MergeConst(p->lchild);
            MergeConst(p->rchild);
        }
    }
}

QString Diff(QString str,QString c){ //求偏导
    int i=0;
    QString exp;
    while(i<str.length()){
        if(str[i]==c){
            if(i=0&&str[i+1]!='^') exp+=1;
            if(i=0&&str[i+1]=='^'){
            int ep=str.mid(i+2,1).toInt()-1;
            exp+=str[i+2]+c+"^"+QString::number(ep);
            }
            if(i>=1){
        if((str[i-1]=='+'||str[i-1]=='-')&&str[i+1]!='^'){
            exp+=str[i-1]+"1";
        }
        else if((str[i-1]=='+'||str[i-1]=='-')&&str[i+1]=='^'){
            int ep=str.mid(i+2,1).toInt()-1;
            exp+=str[i-1];
            exp+=str[i+2]+c+"^"+QString::number(ep);
        }}
            if(i>=3){
        if(str[i-1]=='*'&&str[i+1]!='^'){
            exp+=str[i-3];
            exp+=str[i-2];
        }
            }
        if(i>=3){if(str[i-1]=='*'&&str[i+1]=='^'){
            int inf=str.mid(i-2,1).toInt()*str.mid(i+2).toInt();
            int ep=str.mid(i+2,1).toInt()-1;
            exp+=str[i-3]+QString::number(inf)+"*"+c+"^"+QString::number(ep);
        }}
        }
        i++;
    }
    return exp;
}
