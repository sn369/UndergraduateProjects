#include <QString>
#include <iostream>

//	��Ľṹ
struct Term {
    double coef;
    int expn;
    Term *next;
};

//����ʽ��
class Poly{
    public:
        Term *head;
        Poly();
        ~Poly();
        double Insert(double coef, int exp);
        int Getsize();
        double Value(double x);
        Poly Derivative();
    friend Poly operator +(Poly Pa, Poly Pb);//����ʽ�ӷ�����
    friend Poly operator -(Poly Pa, Poly Pb);//����ʽ��������
    friend Poly operator *(Poly Pa, Poly Pb);//����ʽ�˷�����
};

//���캯��
Poly::Poly(){
    head = new Term;
    head->next=NULL;//��ͷ�ڵ�
}

//���ٶ���ʽ
Poly::~Poly(){
    Term *q1, *q2;
    q1 = head;
    q2 = q1->next;
    while (q2) {
        delete(q1);
        q1 = q2;//ָ�����
        q2 = q2->next;
    }
}

//���뺯��
double Poly::Insert(double coef, int exp){//����ڵ㷽��
    Term* p = head;
    while(p -> next){//���p����һ���ڵ�
        if(p -> next -> expn == exp){//ָ����ͬ�ϲ���
            coef = coef + p -> next -> coef;
            if(coef == 0){//���ϵ��Ϊ0ɾ����
                p -> next = p -> next -> next;
                delete p -> next;
                return 0;
            }
            else{
                p -> next -> coef = coef;
                return coef;
            }
        }
        if(p -> next -> expn < exp){//����Ӧ����p��next֮��
            Term* q = new Term;
            q -> expn = exp;
            q -> coef = coef;
            q -> next = p -> next;
            p -> next = q;
            return coef;
        }
        p = p -> next;
    }
    //����½ڵ�ָ�������������нڵ㣬���뵽����ͷ
    Term* q = new Term;
    q -> expn = exp;
    q -> coef = coef;
    q -> next = p -> next;
    p -> next = q;
    return coef;
}

int compare(Term* a, Term* b) {
    if (a&&b) {
        if (!a || a->expn<b->expn) return -1;//a�ѿջ���ָ��С
        else if (!b || a->expn>b->expn) return 1;//b�ѿջ���ָ��С
        else return 0;
    }
    else if (!a&&b) return -1;//a����ʽ�ѿգ���b����ʽ�ǿ�
    else return 1;//b����ʽ�ѿգ���a����ʽ�ǿ�
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

//����
Poly operator-(Poly Pa,Poly Pb){//��Ⲣ��������ʽa-b
    Term *p=Pb.head->next;
    while(p){           //��pb��ϵ��ȡ��
     p->coef*=-1;
     p=p->next;
    }
 return(Pa+Pb);
}

//�˷�
Poly operator*(Poly Pa,Poly Pb){//��Ⲣ��������ʽa*b
        Term* qa=Pa.head->next;
        Term* qb=Pb.head->next;
        Poly P;
        for(;qa;qa=qa->next){
            qb=Pb.head->next;
            for(;qb;qb=qb->next){
                double coef=qa->coef*qb->coef;
                int expn=qa->expn+qb->expn;
                P.Insert(coef,expn);//����Insert�����Ժϲ�ָ����ͬ����
            }
        }
        return P;
}

//������
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

//��
Poly Poly::Derivative() {//��Ⲣ����a�ĵ���������ʽ
    Poly Pnew;
    Term* q = this->head->next;
    Term *p1 =Pnew.head;//����ͷ���
    Term *p2;
    while (q) {
        if (q->expn != 0) {               //����ǳ�����ʱ
            p2 = new Term;
            p2->coef = q->coef*q->expn;
            p2->expn = q->expn - 1;
            p2->next = p1->next;//���ӽ��
            p1->next = p2;
            p1 = p2;
        }
        q = q->next;
    }
    return Pnew;
}

//��ֵ
double Poly::Value(double x) {
    Term* p;
    int i;
    double sum = 0,t = 1;
    for (p = head->next; p; p = p->next) {
        for (i = p->expn; i != 0;) {
            if (i<0) { t /= x; i++; }//ָ��С��0�����г���
            else { t *= x; i--; }//ָ������0�����г˷�
        }
        sum += p->coef*t;
    }
    return sum;
}
