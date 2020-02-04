#include <QString>

//用单链表实现栈
//运算数结点
struct numNode{
    double num;
    numNode* next;
};

//运算数栈
class Number_Stack{
	public:
        numNode* base;  //栈底
        numNode* top;   //栈顶
		QString s;		//用于记录栈的元素内容
        Number_Stack(){ //初始化
            base=new numNode;
            base->num=0;
			base->next=NULL;
			top=base;
            s="";
        }
		
		~Number_Stack(){ //析构函数
			numNode* p;
			for(p=top;p!=NULL;p=p->next)
				delete(p);
		}

        void push(numNode* p){//入栈
            p->next=top;
            top=p;
            s=s+"  "+QString::number(p->num);
        }

        double pop(){//出栈并返回栈顶元素
            double temp=top->num;
            s=s.remove("  "+QString::number(temp));
            top=top->next;
            return temp;
        }
};


//运算符结点
struct opNode{
    QChar op;
    opNode* next;
};

//运算符栈
class Operator_Stack{
    public:
         opNode* base;  //栈底
         opNode* top;  //栈顶         
		 QString s;
         Operator_Stack(){//初始化，压入#符号
            base=new opNode;
			base->op='#';
            base->next=NULL;
            top=base;
            s="#";
        }

        ~Operator_Stack(){ //析构函数
            opNode* p;
			for(p=top;p!=NULL;p=p->next)
				delete(p);
		}

        void push(opNode* p){//入栈
            p->next=top;
            top=p;
            s=s+p->op;
        }

        QChar pop(){//出栈并返回栈顶元素
            QChar temp=top->op;
            s=s.left(s.length()-1);
            top=top->next;
            return temp;
        }

        int cmp(QChar op1,QChar op2);  //符号优先级比较
										//op1为当前读入元素，op2为栈顶元素
};

int Operator_Stack::cmp(QChar op1,QChar op2){//当前符号优先级高，返回1；低，返回-1；相等，返回0
    if(op2=='+'||op2=='-'){
        if(op1=='+'||op1=='-'||op1==')'||op1=='#') return -1;
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

    if(op2=='#'){
		if(op1=='#') return 0;
        else return 1;
	}
}





