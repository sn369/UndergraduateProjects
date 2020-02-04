#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#define MAX_VERTEX_NUM 30
#define MAX_EDGE_NUM 435
using namespace std;

//---------------����������-------------------

void drawPicture() { //��ͼ����
	initgraph(640, 480);
	TCHAR s1[] = _T("��³ľ��");
	outtextxy(40, 25, s1);
	TCHAR s2[] = _T("���ͺ���");
	outtextxy(280, 100, s2);
	TCHAR s3[] = _T("����");
	outtextxy(375, 125, s3);
	TCHAR s4[] = _T("���");
	outtextxy(420, 168, s4);
	TCHAR s5[] = _T("����");
	outtextxy(510, 125, s5);
	TCHAR s6[] = _T("����");
	outtextxy(550, 90, s6);
	TCHAR s7[] = _T("������");
	outtextxy(580, 40, s7);
	TCHAR s8[] = _T("����");
	outtextxy(500, 180, s8);
	TCHAR s9[] = _T("����");
	outtextxy(150, 185, s9);
	TCHAR s10[] = _T("����");
	outtextxy(200, 200, s10);
	TCHAR s11[] = _T("����");
	outtextxy(270, 210, s11);
	TCHAR s12[] = _T("֣��");
	outtextxy(345, 215, s12);
	TCHAR s13[] = _T("����");
	outtextxy(415, 220, s13);
	TCHAR s14[] = _T("�ɶ�");
	outtextxy(195, 275, s14);
	TCHAR s15[] = _T("�人");
	outtextxy(342, 280, s15);
	TCHAR s16[] = _T("�Ϻ�");
	outtextxy(450, 283, s16);
	TCHAR s17[] = _T("����");
	outtextxy(150, 360, s17);
	TCHAR s18[] = _T("����");
	outtextxy(205, 338, s18);
	TCHAR s19[] = _T("����");
	outtextxy(320, 345, s19);
	TCHAR s20[] = _T("�ϲ�");
	outtextxy(375, 343, s20);
	TCHAR s21[] = _T("����");
	outtextxy(435, 360, s21);
	TCHAR s22[] = _T("����");
	outtextxy(200, 400, s22);
	TCHAR s23[] = _T("����");
	outtextxy(260, 380, s23);
	TCHAR s24[] = _T("����");
	outtextxy(318, 400, s24);
	TCHAR s25[] = _T("����");
	outtextxy(350, 430, s25);
}
	
//˳��ջ
template <class T> class Stack {
public:
	T stack[MAX_VERTEX_NUM];
	int top = -1;
	T gettop() {
		return stack[top];
	}
	void pop() {
		top--;
	}
	void push(T item) {
		stack[++top] = item;
	}
	bool isEmpty() {
		if (top == -1) return true;
		else return false;
	}
};

//����
template <class T> class Queue {
public:
	T queue[MAX_VERTEX_NUM]; //˳���ʵ��
	int front = 0, rear = 0;
	T getfront() {
		return queue[front];
	}
	void dequeue() {
		front = (front + 1) % MAX_VERTEX_NUM;
	}
	void enqueue(T item) {
		queue[rear] = item;
		rear = (rear + 1) % MAX_VERTEX_NUM;
	}
	bool isEmpty() {
		if (front == rear) return true;
		else return false;
	}
};

//-----------------����������-----------------------------


//-----------------�������ڽӱ�洢�ṹ-------------------

struct EBox { //�߽��
	int ivex, jvex; //�ñ�������������������
	EBox *ilink, *jlink; //�ֱ�ָ�������������������һ����
};

struct VexBox { //������
	string name; //�������
	int x, y; //�������
	EBox *firstedge; //ָ���һ�������˶���ı�
};

class MALGraph { //�����ڽӱ�洢����ͼ�� MAL:Multiple Adjacency List
public:
	MALGraph() {
		egdenum = 0;
		vexnum = 0;
		adjmulist = new VexBox[31]; //���㼯�ϣ���Ϊ�±��1��ʼ������ȡ����Ϊ31
	}
	~MALGraph() { //��������
		delete[]adjmulist;
	}
	void InitMALGraph();
	void BFS(int v); //�����������
	void DFS(int v); //�����������
	friend void drawPicture();

	//private:
	VexBox *adjmulist; //���㼯��
	int vexnum, egdenum; //����ͼ�ĵ�ǰ�������ͱ���
};

void MALGraph::InitMALGraph() { //��ʼ��
	ifstream fcin;
	int i = 1;
	fcin.open("C:/Users/x/Desktop/name.txt"); //��ȡ�������ݼ�����
	while (!fcin.eof()) {
		fcin >> i;
		fcin >> adjmulist[i].name >> adjmulist[i].x >> adjmulist[i].y;
		adjmulist[i].firstedge = NULL; //��ʼ��ʼ��
		vexnum++; //�������
	}
	fcin.close();

	fcin.open("C:/Users/x/Desktop/edge.txt"); //��ȡ�ߣ������ԣ��������˵����ű�ʾ
	while (!fcin.eof()) {
		EBox *e = new EBox;
		fcin >> (e->ivex) >> (e->jvex);
		e->ilink = adjmulist[e->ivex].firstedge; //ͷ�巨���������i�˵���صı�����
		adjmulist[e->ivex].firstedge = e;
		e->jlink = adjmulist[e->jvex].firstedge; //ͷ�巨���������j�˵���صı�����
		adjmulist[e->jvex].firstedge = e;
		egdenum++; //����
	}
	fcin.close();
}

void MALGraph::BFS(int v) { //����ͼ��ָ���������Ĺ����������
	int i, vi, j = 0;
	EBox es[MAX_VERTEX_NUM]; //�洢�߼�
	bool visited[MAX_VERTEX_NUM]; //�������飬��¼�������
	Queue<int> Q; //��������
	EBox *p;
	for (int i = 1; i<MAX_VERTEX_NUM; i++) //ȫ�������Ϊδ����
		visited[i] = 0;
	//drawPicture();
	visited[v] = 1; //ʼ�����Ϊ�ѷ���
	cout << "---------�� [" << adjmulist[v].name << "] ����������ȱ���---------\n";
	cout << "-----���˳��Ϊ��\n";
	cout << adjmulist[v].name << " "; //�����ǰ���ʽ��λ��
	Q.enqueue(v);
	while (!Q.isEmpty()) { //���в�Ϊ��
		vi = Q.getfront(); //��ȡ��ͷ�����Ϣ
		Q.dequeue(); //����
		p = adjmulist[vi].firstedge; //pָ��vi��������
		while (p) { //�ߴ���
			if (p->ivex == vi) { //p��ivex�˵���vi
				if (!visited[p->jvex]) { //p��jvex�˵�û���ʹ�
					cout << adjmulist[p->jvex].name << " "; //����֮
					visited[p->jvex] = 1; //���Ϊ���ʹ�
					Q.enqueue(p->jvex); //���
					es[j] = *p; j++; //�ñ߼���߼�
									 //line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
									 //����
				}
				p = p->ilink; //ָ��vi����һ�ڱ߽��
			}
			else { //p��jvex�˵���vi
				if (!visited[p->ivex]) { //p��ivex�˵�û���ʹ�
					cout << adjmulist[p->ivex].name << " "; //����֮
					visited[p->ivex] = 1; //���Ϊ���ʹ�
					Q.enqueue(p->ivex); //���
					es[j] = *p; j++;
					//line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
				}
				p = p->jlink; //ָ��vi����һ�ڱ߽��
			}
		}
	}
	// ��������˳�ͼ��
	//_getch();
	//closegraph();
	cout << endl << endl << "-----�߼�Ϊ��";
	for (i = 0; i < j; i++) {
		if (i % 5 == 0) cout << endl;
		cout << "(" << adjmulist[es[i].ivex].name << ", " << adjmulist[es[i].jvex].name << ")" << " ";
	}
	cout << endl;
}

void MALGraph::DFS(int v) { //�ǵݹ������������
	bool visited[MAX_VERTEX_NUM]; //���ʱ��
	Stack<int> S; //����ջ
	int i, j = 0, vi;
	EBox *p;
	EBox es[MAX_VERTEX_NUM];
	for (i = 0; i<MAX_VERTEX_NUM; i++) //��ʼ������δ����
		visited[i] = 0;
	//drawPicture();
	cout << "---------�� [" << adjmulist[v].name << "] ����������ȱ���---------\n";
	cout << "-----���˳��Ϊ��\n";
	cout << adjmulist[v].name << " "; //������㲢��ջ
	visited[v] = 1;
	S.push(v);
	while (!S.isEmpty()) { //��ջ��Ϊ��
		vi = S.gettop(); //��ȡջ��Ԫ��
		p = adjmulist[vi].firstedge; //ָ��ջ��Ԫ�ص�ʼ��
		while (p) { //���ߴ���
			if (p->ivex == vi) { //���ߵ�ivex��vi
				if (!visited[p->jvex]) { //�ߵ���һ�˵�δ���ʹ�
					cout << adjmulist[p->jvex].name << " "; //����֮������ǣ�����ջ
					visited[p->jvex] = 1;
					S.push(p->jvex);
					//line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
					es[j] = *p; j++; //�洢��
					vi = p->jvex; //���ոշ��ʹ��ĵ���Ϊ��һĿ�궥��
					p = adjmulist[p->jvex].firstedge; //��������ʼ��
				}
				else p = p->ilink; //����һ�˵���ʹ��ˣ�ָ����һ����
			}
			else { //���ߵ�jvex��vi
				if (!visited[p->ivex]) { //�ߵ���һ�˵�δ���ʹ�
					cout << adjmulist[p->ivex].name << " "; //����֮������ǣ�����ջ
					visited[p->ivex] = 1;
					S.push(p->ivex);
					//line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
					es[j] = *p; j++; //�洢��
					vi = p->ivex; //���ոշ��ʹ��ĵ���Ϊ��һĿ�궥��
					p = adjmulist[p->ivex].firstedge; //��������ʼ��
				}
				else p = p->jlink; //����һ�˵���ʹ��ˣ�ָ����һ����
			}
		}
		S.pop(); //��pΪ�գ���ʾû��δ���ʹ����ڵ㣬�����
	}
	// ��������˳�
	//_getch();
	//closegraph();
	cout << endl << endl;
	cout << "-----�߼�Ϊ��";
	for (i = 0; i < j; i++) {
		if (i % 5 == 0) cout << endl;
		cout << "(" << adjmulist[es[i].ivex].name << ", " << adjmulist[es[i].jvex].name << ")" << " ";
	}
	cout << endl;
}

//------------------------�������ڽӱ�洢�ṹ------------------------

//------------------------���ڽӱ�洢�ṹ----------------------------

struct EdgeNode { //�߽��
	int index; //�ñ���ָ�Ķ�������
	EdgeNode *next; //��һ����ö������ڵı�
	EdgeNode(int n, EdgeNode*p) {
		index = n;
		next = p;
	}
};

struct VertexNode { //������
	string name; //����
	int x, y; //����
	EdgeNode *first; //��һ�����ڵı�
};

class ADLGraph { //�ڽӱ�洢����ͼ
public:
	ADLGraph() {
		adjList = new VertexNode[31];
		vNum = 0; eNum = 0;
	}
	~ADLGraph() {
		delete[] adjList;
	}
	void InitADLGraph();
	void DFStree(int v); //�����������������
	void BFStree(int v); //�����������������
	friend void drawPicture();
//private:
	VertexNode *adjList;
	int vNum, eNum;
	int lastIndex; //���һ������ĵ���±�
};

void ADLGraph::InitADLGraph() { //��ʼ��
	ifstream fcin;
	int i = 1, j = 1;
	EdgeNode *p1, *p2;
	fcin.open("C:/Users/x/Desktop/name.txt");
	while (!fcin.eof()) {
		fcin >> i;
		fcin >> adjList[i].name >> adjList[i].x >> adjList[i].y;
		adjList[i].first = NULL;
		vNum++; //�������
	}
	fcin.close();
	lastIndex = i;
	fcin.open("C:/Users/x/Desktop/edge.txt");
	while (!fcin.eof()) { //��ȡ��
		fcin >> i >> j;
		p1 = new EdgeNode(j, adjList[i].first);
		p2 = new EdgeNode(i, adjList[j].first);
		adjList[i].first = p1;
		adjList[j].first = p2;
		eNum++;
	}
	fcin.close();
}

void ADLGraph::DFStree(int v) { //�ǵݹ��������������
	bool visited[MAX_VERTEX_NUM];
	int i;
	EdgeNode *e;
	Stack <int >S; //����ջ
	int xx, yy;
	for (i = 1; i<vNum + 1; i++) //��ʼ����δ���ʹ� 
		visited[i] = 0;
	drawPicture();
	//initgraph(640, 480);
	circle(adjList[v].x, adjList[v].y, 10);
	visited[v] = 1;
	S.push(v);
	while (!S.isEmpty()) { //ջ��Ϊ��
		e = adjList[S.gettop()].first; //ѡ��ջ���˵�
		xx = adjList[S.gettop()].x;
		yy = adjList[S.gettop()].y;
		while (e) { //���ߴ���
			if (!visited[e->index]) { //�ڵ�δ���ʹ�
				circle(adjList[e->index].x, adjList[e->index].y, 10);
				visited[e->index] = 1; //��������ջ
				S.push(e->index);
				line(xx, yy, adjList[e->index].x, adjList[e->index].y); //����
				xx = adjList[e->index].x; //������ʼ�˵�
				yy = adjList[e->index].y;
				e = adjList[e->index].first; //��������һ�˵����ڵĵ�һ����
			}
			else { //�����ʹ��ˣ�ָ����һ���ڱ�
				e = e->next;
			}
		}
		S.pop(); //��ö������ڵĵ�����ʹ��ˣ���ջ
	}
	// ��������˳�ͼ��
	_getch();
	closegraph();
	return;
}

void ADLGraph::BFStree(int v) { //�������������
	bool visited[MAX_VERTEX_NUM];
	int i;
	EdgeNode *e;
	Queue<int> Q; //��������
	for (i = 1; i<vNum + 1; i++) //��ʼ����δ���ʹ� 
		visited[i] = 0;
	drawPicture();
	circle(adjList[v].x, adjList[v].y, 10);
	visited[v] = 1;
	Q.enqueue(v);
	int xx, yy;
	while (!Q.isEmpty()) { //���Ӳ�Ϊ��
		e = adjList[Q.getfront()].first; //ѡ�ж�ͷ����Ϊ������
		xx = adjList[Q.getfront()].x;
		yy = adjList[Q.getfront()].y;
		Q.dequeue(); //����
		while (e) { //�ߴ���
			if (!visited[e->index]) { //�ڵ�δ���ʹ�
				circle(adjList[e->index].x, adjList[e->index].y, 10);
				visited[e->index] = 1; //���������
				Q.enqueue(e->index);
				line(xx, yy, adjList[e->index].x, adjList[e->index].y); //��������������һ�˵�
			}
			e = e->next; //ָ����һ�ڵ�
		}
	}
	// ��������˳�ͼ��
	_getch();
	closegraph();
	return;
}

int main() {
	int j, k;
		cout << "------------------------�����빦�����------------------------\n";
		MALGraph malG;
		malG.InitMALGraph();
		ADLGraph adlG;
		adlG.InitADLGraph();
		cout << "                       1.������ȱ���\n" << "                       2.������ȱ���\n";
		cout << "                       3.�������������\n" << "                       4.�������������\n";
		cin >> j;
		cout << "------------------------������������------------------------\n";
		for (int i = 1; i <= adlG.lastIndex; i++) {
			cout << i << "-" << malG.adjmulist[i].name << " ";
			if (i % 5 == 0) cout << endl;
		}
		cout << "--------------------------------------------------------------\n";
		cin >> k;
		switch (j) {
		case 1: malG.DFS(k); break;
		case 2: malG.BFS(k); break;
		case 3: adlG.DFStree(k); break;
		case 4: adlG.BFStree(k); break;
		}
		malG.~MALGraph();
		adlG.~ADLGraph();
	return 0;
}