#include <QString>
#include <iostream>

//	项的结构
struct Term {
    double coef;
    int expn;
    Term *next;
};

//多项式类
class Poly{
    public:
        Term *head;
        Poly();
        ~Poly();
        double Insert(double coef, int exp);
        int Getsize();
        double Value(double x);
        Poly Derivative();
    friend Poly operator +(Poly Pa, Poly Pb);//多项式加法运算
    friend Poly operator -(Poly Pa, Poly Pb);//多项式减法运算
    friend Poly operator *(Poly Pa, Poly Pb);//多项式乘法运算
};

//构造函数
Poly::Poly(){
    head = new Term;
    head->next=NULL;//表头节点
}

//销毁多项式
Poly::~Poly(){
    Term *q1, *q2;
    q1 = head;
    q2 = q1->next;
    while (q2) {
        delete(q1);
        q1 = q2;//指针后移
        q2 = q2->next;
    }
}

//插入函数
double Poly::Insert(double coef, int exp){//插入节点方法
    Term* p = head;
    while(p -> next){//如果p后还有一个节点
        if(p -> next -> expn == exp){//指数相同合并项
            coef = coef + p -> next -> coef;
            if(coef == 0){//如果系数为0删除项
                p -> next = p -> next -> next;
                delete p -> next;
                return 0;
            }
            else{
                p -> next -> coef = coef;
                return coef;
            }
        }
        if(p -> next -> expn < exp){//新项应插在p与next之间
            Term* q = new Term;
            q -> expn = exp;
            q -> coef = coef;
            q -> next = p -> next;
            p -> next = q;
            return coef;
        }
        p = p -> next;
    }
    //如果新节点指数大于所有已有节点，插入到链表头
    Term* q = new Term;
    q -> expn = exp;
    q -> coef = coef;
    q -> next = p -> next;
    p -> next = q;
    return coef;
}

int compare(Term* a, Term* b) {
    if (a&&b) {
        if (!a || a->expn<b->expn) return -1;//a已空或者指数小
        else if (!b || a->expn>b->expn) return 1;//b已空或者指数小
        else return 0;
    }
    else if (!a&&b) return -1;//a多项式已空，但b多项式非空
    else return 1;//b多项式已空，但a多项式非空
}//compare

//add
Poly operator +(Poly Pa, Poly Pb){
    Term* qa = (Pa.head)->next;
    Term* qb = (Pb.head)->next;
    Poly P;
    Term *hc = P.head;
    while (qa || qb) {
        Term* qc = new Term;
        switch (compare(qa, qb)) {
        case 1: {
            qc->coef = qa->coef;
            qc->expn = qa->expn;
            qa = qa->next;
            break;
        }

        case 0: {
            qc->coef = qa->coef + qb->coef;
            qc->expn = qa->expn;
            qa = qa->next;
            qb = qb->next;
            break;
        }

        case -1: {
            qc->coef = qb->coef;
            qc->expn = qb->expn;
            qb = qb->next;
            break;
        }
        }
        if (qc->coef != 0) {
            qc->next = hc->next;
            hc->next = qc;
            hc = qc;
        }
        else free(qc);
    }
    return P;
}//Add

//减法
Poly operator-(Poly Pa,Poly Pb){//求解并建立多项式a-b
    Term *p=Pb.head->next;
    while(p){           //将pb的系数取反
     p->coef*=-1;
     p=p->next;
    }
 return(Pa+Pb);
}

//乘法
Poly operator*(Poly Pa,Poly Pb){//求解并建立多项式a*b
        Term* qa=Pa.head->next;
        Term* qb=Pb.head->next;
        Poly P;
        for(;qa;qa=qa->next){
            qb=Pb.head->next;
            for(;qb;qb=qb->next){
                double coef=qa->coef*qb->coef;
                int expn=qa->expn+qb->expn;
                P.Insert(coef,expn);//调用Insert函数以合并指数相同的项
            }
        }
        return P;
}

//求项数
int Poly::Getsize() {
    Term*p1, *p2;
    p1 = this->head->next;
    p2 = p1->next;
    int i = 1;
    while (p2) {
        i++;
        p1 = p2;
        p2 = p2->next;
    }
    return i;
}

//求导
Poly Poly::Derivative() {//求解并建立a的导函数多项式
    Poly Pnew;
    Term* q = this->head->next;
    Term *p1 =Pnew.head;//建立头结点
    Term *p2;
    while (q) {
        if (q->expn != 0) {               //该项不是常数项时
            p2 = new Term;
            p2->coef = q->coef*q->expn;
            p2->expn = q->expn - 1;
            p2->next = p1->next;//连接结点
            p1->next = p2;
            p1 = p2;
        }
        q = q->next;
    }
    return Pnew;
}

//求值
double Poly::Value(double x) {
    Term* p;
    int i;
    double sum = 0,t = 1;
    for (p = head->next; p; p = p->next) {
        for (i = p->expn; i != 0;) {
            if (i<0) { t /= x; i++; }//指数小于0，进行除法
            else { t *= x; i--; }//指数大于0，进行乘法
        }
        sum += p->coef*t;
    }
    return sum;
}
