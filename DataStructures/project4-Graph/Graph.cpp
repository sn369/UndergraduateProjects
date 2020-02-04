#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#define MAX_VERTEX_NUM 30
#define MAX_EDGE_NUM 435
using namespace std;

//---------------↓辅助函数-------------------

void drawPicture() { //绘图函数
	initgraph(640, 480);
	TCHAR s1[] = _T("乌鲁木齐");
	outtextxy(40, 25, s1);
	TCHAR s2[] = _T("呼和浩特");
	outtextxy(280, 100, s2);
	TCHAR s3[] = _T("北京");
	outtextxy(375, 125, s3);
	TCHAR s4[] = _T("天津");
	outtextxy(420, 168, s4);
	TCHAR s5[] = _T("沈阳");
	outtextxy(510, 125, s5);
	TCHAR s6[] = _T("长春");
	outtextxy(550, 90, s6);
	TCHAR s7[] = _T("哈尔滨");
	outtextxy(580, 40, s7);
	TCHAR s8[] = _T("大连");
	outtextxy(500, 180, s8);
	TCHAR s9[] = _T("西宁");
	outtextxy(150, 185, s9);
	TCHAR s10[] = _T("兰州");
	outtextxy(200, 200, s10);
	TCHAR s11[] = _T("西安");
	outtextxy(270, 210, s11);
	TCHAR s12[] = _T("郑州");
	outtextxy(345, 215, s12);
	TCHAR s13[] = _T("徐州");
	outtextxy(415, 220, s13);
	TCHAR s14[] = _T("成都");
	outtextxy(195, 275, s14);
	TCHAR s15[] = _T("武汉");
	outtextxy(342, 280, s15);
	TCHAR s16[] = _T("上海");
	outtextxy(450, 283, s16);
	TCHAR s17[] = _T("昆明");
	outtextxy(150, 360, s17);
	TCHAR s18[] = _T("贵阳");
	outtextxy(205, 338, s18);
	TCHAR s19[] = _T("株洲");
	outtextxy(320, 345, s19);
	TCHAR s20[] = _T("南昌");
	outtextxy(375, 343, s20);
	TCHAR s21[] = _T("福州");
	outtextxy(435, 360, s21);
	TCHAR s22[] = _T("南宁");
	outtextxy(200, 400, s22);
	TCHAR s23[] = _T("柳州");
	outtextxy(260, 380, s23);
	TCHAR s24[] = _T("广州");
	outtextxy(318, 400, s24);
	TCHAR s25[] = _T("深圳");
	outtextxy(350, 430, s25);
}
	
//顺序栈
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

//队列
template <class T> class Queue {
public:
	T queue[MAX_VERTEX_NUM]; //顺序表实现
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

//-----------------↑辅助函数-----------------------------


//-----------------↓多重邻接表存储结构-------------------

struct EBox { //边结点
	int ivex, jvex; //该边依附的两个顶点的序号
	EBox *ilink, *jlink; //分别指向依附这两个顶点的下一条边
};

struct VexBox { //顶点结点
	string name; //结点内容
	int x, y; //结点坐标
	EBox *firstedge; //指向第一条依附此顶点的边
};

class MALGraph { //多重邻接表存储无向图类 MAL:Multiple Adjacency List
public:
	MALGraph() {
		egdenum = 0;
		vexnum = 0;
		adjmulist = new VexBox[31]; //顶点集合，因为下标从1开始，所以取容量为31
	}
	~MALGraph() { //析构函数
		delete[]adjmulist;
	}
	void InitMALGraph();
	void BFS(int v); //广度优先搜索
	void DFS(int v); //深度优先搜索
	friend void drawPicture();

	//private:
	VexBox *adjmulist; //顶点集合
	int vexnum, egdenum; //无向图的当前顶点数和边数
};

void MALGraph::InitMALGraph() { //初始化
	ifstream fcin;
	int i = 1;
	fcin.open("C:/Users/x/Desktop/name.txt"); //读取顶点内容及坐标
	while (!fcin.eof()) {
		fcin >> i;
		fcin >> adjmulist[i].name >> adjmulist[i].x >> adjmulist[i].y;
		adjmulist[i].firstedge = NULL; //初始化始边
		vexnum++; //顶点个数
	}
	fcin.close();

	fcin.open("C:/Users/x/Desktop/edge.txt"); //读取边，以数对，即两个端点的序号表示
	while (!fcin.eof()) {
		EBox *e = new EBox;
		fcin >> (e->ivex) >> (e->jvex);
		e->ilink = adjmulist[e->ivex].firstedge; //头插法建立与序号i端点相关的边链表
		adjmulist[e->ivex].firstedge = e;
		e->jlink = adjmulist[e->jvex].firstedge; //头插法建立与序号j端点相关的边链表
		adjmulist[e->jvex].firstedge = e;
		egdenum++; //边数
	}
	fcin.close();
}

void MALGraph::BFS(int v) { //无向图从指定结点出发的广度优先搜索
	int i, vi, j = 0;
	EBox es[MAX_VERTEX_NUM]; //存储边集
	bool visited[MAX_VERTEX_NUM]; //辅助数组，记录访问情况
	Queue<int> Q; //辅助队列
	EBox *p;
	for (int i = 1; i<MAX_VERTEX_NUM; i++) //全部结点标记为未访问
		visited[i] = 0;
	//drawPicture();
	visited[v] = 1; //始结点标记为已访问
	cout << "---------从 [" << adjmulist[v].name << "] 出发广度优先遍历---------\n";
	cout << "-----结点顺序为：\n";
	cout << adjmulist[v].name << " "; //输出当前访问结点位置
	Q.enqueue(v);
	while (!Q.isEmpty()) { //队列不为空
		vi = Q.getfront(); //获取队头结点信息
		Q.dequeue(); //出队
		p = adjmulist[vi].firstedge; //p指向vi的首条边
		while (p) { //边存在
			if (p->ivex == vi) { //p的ivex端点是vi
				if (!visited[p->jvex]) { //p的jvex端点没访问过
					cout << adjmulist[p->jvex].name << " "; //访问之
					visited[p->jvex] = 1; //标记为访问过
					Q.enqueue(p->jvex); //入队
					es[j] = *p; j++; //该边加入边集
									 //line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
									 //连线
				}
				p = p->ilink; //指向vi的下一邻边结点
			}
			else { //p的jvex端点是vi
				if (!visited[p->ivex]) { //p的ivex端点没访问过
					cout << adjmulist[p->ivex].name << " "; //访问之
					visited[p->ivex] = 1; //标记为访问过
					Q.enqueue(p->ivex); //入队
					es[j] = *p; j++;
					//line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
				}
				p = p->jlink; //指向vi的下一邻边结点
			}
		}
	}
	// 按任意键退出图像
	//_getch();
	//closegraph();
	cout << endl << endl << "-----边集为：";
	for (i = 0; i < j; i++) {
		if (i % 5 == 0) cout << endl;
		cout << "(" << adjmulist[es[i].ivex].name << ", " << adjmulist[es[i].jvex].name << ")" << " ";
	}
	cout << endl;
}

void MALGraph::DFS(int v) { //非递归深度优先搜索
	bool visited[MAX_VERTEX_NUM]; //访问标记
	Stack<int> S; //辅助栈
	int i, j = 0, vi;
	EBox *p;
	EBox es[MAX_VERTEX_NUM];
	for (i = 0; i<MAX_VERTEX_NUM; i++) //初始化，都未访问
		visited[i] = 0;
	//drawPicture();
	cout << "---------从 [" << adjmulist[v].name << "] 出发深度优先遍历---------\n";
	cout << "-----结点顺序为：\n";
	cout << adjmulist[v].name << " "; //访问起点并入栈
	visited[v] = 1;
	S.push(v);
	while (!S.isEmpty()) { //当栈不为空
		vi = S.gettop(); //获取栈顶元素
		p = adjmulist[vi].firstedge; //指向栈顶元素的始边
		while (p) { //当边存在
			if (p->ivex == vi) { //若边的ivex是vi
				if (!visited[p->jvex]) { //边的另一端点未访问过
					cout << adjmulist[p->jvex].name << " "; //访问之，并标记，且入栈
					visited[p->jvex] = 1;
					S.push(p->jvex);
					//line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
					es[j] = *p; j++; //存储边
					vi = p->jvex; //将刚刚访问过的点设为下一目标顶点
					p = adjmulist[p->jvex].firstedge; //访问它的始边
				}
				else p = p->ilink; //若另一端点访问过了，指向下一条边
			}
			else { //若边的jvex是vi
				if (!visited[p->ivex]) { //边的另一端点未访问过
					cout << adjmulist[p->ivex].name << " "; //访问之，并标记，且入栈
					visited[p->ivex] = 1;
					S.push(p->ivex);
					//line(adjmulist[p->ivex].x, adjmulist[p->ivex].y, adjmulist[p->jvex].x, adjmulist[p->jvex].y);
					es[j] = *p; j++; //存储边
					vi = p->ivex; //将刚刚访问过的点设为下一目标顶点
					p = adjmulist[p->ivex].firstedge; //访问它的始边
				}
				else p = p->jlink; //若另一端点访问过了，指向下一条边
			}
		}
		S.pop(); //若p为空，表示没有未访问过的邻点，须回退
	}
	// 按任意键退出
	//_getch();
	//closegraph();
	cout << endl << endl;
	cout << "-----边集为：";
	for (i = 0; i < j; i++) {
		if (i % 5 == 0) cout << endl;
		cout << "(" << adjmulist[es[i].ivex].name << ", " << adjmulist[es[i].jvex].name << ")" << " ";
	}
	cout << endl;
}

//------------------------↑多重邻接表存储结构------------------------

//------------------------↓邻接表存储结构----------------------------

struct EdgeNode { //边结点
	int index; //该边所指的顶点的序号
	EdgeNode *next; //下一条与该顶点相邻的边
	EdgeNode(int n, EdgeNode*p) {
		index = n;
		next = p;
	}
};

struct VertexNode { //顶点结点
	string name; //内容
	int x, y; //坐标
	EdgeNode *first; //第一条相邻的边
};

class ADLGraph { //邻接表存储无向图
public:
	ADLGraph() {
		adjList = new VertexNode[31];
		vNum = 0; eNum = 0;
	}
	~ADLGraph() {
		delete[] adjList;
	}
	void InitADLGraph();
	void DFStree(int v); //画出深度优先生成树
	void BFStree(int v); //画出广度优先生成树
	friend void drawPicture();
//private:
	VertexNode *adjList;
	int vNum, eNum;
	int lastIndex; //最后一个读入的点的下标
};

void ADLGraph::InitADLGraph() { //初始化
	ifstream fcin;
	int i = 1, j = 1;
	EdgeNode *p1, *p2;
	fcin.open("C:/Users/x/Desktop/name.txt");
	while (!fcin.eof()) {
		fcin >> i;
		fcin >> adjList[i].name >> adjList[i].x >> adjList[i].y;
		adjList[i].first = NULL;
		vNum++; //顶点个数
	}
	fcin.close();
	lastIndex = i;
	fcin.open("C:/Users/x/Desktop/edge.txt");
	while (!fcin.eof()) { //读取边
		fcin >> i >> j;
		p1 = new EdgeNode(j, adjList[i].first);
		p2 = new EdgeNode(i, adjList[j].first);
		adjList[i].first = p1;
		adjList[j].first = p2;
		eNum++;
	}
	fcin.close();
}

void ADLGraph::DFStree(int v) { //非递归深度优先生成树
	bool visited[MAX_VERTEX_NUM];
	int i;
	EdgeNode *e;
	Stack <int >S; //辅助栈
	int xx, yy;
	for (i = 1; i<vNum + 1; i++) //初始化均未访问过 
		visited[i] = 0;
	drawPicture();
	//initgraph(640, 480);
	circle(adjList[v].x, adjList[v].y, 10);
	visited[v] = 1;
	S.push(v);
	while (!S.isEmpty()) { //栈不为空
		e = adjList[S.gettop()].first; //选中栈顶端点
		xx = adjList[S.gettop()].x;
		yy = adjList[S.gettop()].y;
		while (e) { //当边存在
			if (!visited[e->index]) { //邻点未访问过
				circle(adjList[e->index].x, adjList[e->index].y, 10);
				visited[e->index] = 1; //访问且入栈
				S.push(e->index);
				line(xx, yy, adjList[e->index].x, adjList[e->index].y); //连线
				xx = adjList[e->index].x; //更新起始端点
				yy = adjList[e->index].y;
				e = adjList[e->index].first; //访问与另一端点相邻的第一条边
			}
			else { //若访问过了，指向下一条邻边
				e = e->next;
			}
		}
		S.pop(); //与该顶点相邻的点均访问过了，出栈
	}
	// 按任意键退出图像
	_getch();
	closegraph();
	return;
}

void ADLGraph::BFStree(int v) { //广度优先生成树
	bool visited[MAX_VERTEX_NUM];
	int i;
	EdgeNode *e;
	Queue<int> Q; //辅助队列
	for (i = 1; i<vNum + 1; i++) //初始化均未访问过 
		visited[i] = 0;
	drawPicture();
	circle(adjList[v].x, adjList[v].y, 10);
	visited[v] = 1;
	Q.enqueue(v);
	int xx, yy;
	while (!Q.isEmpty()) { //当队不为空
		e = adjList[Q.getfront()].first; //选中队头顶点为主顶点
		xx = adjList[Q.getfront()].x;
		yy = adjList[Q.getfront()].y;
		Q.dequeue(); //出队
		while (e) { //边存在
			if (!visited[e->index]) { //邻点未访问过
				circle(adjList[e->index].x, adjList[e->index].y, 10);
				visited[e->index] = 1; //访问且入队
				Q.enqueue(e->index);
				line(xx, yy, adjList[e->index].x, adjList[e->index].y); //连接主顶点与另一端点
			}
			e = e->next; //指向下一邻点
		}
	}
	// 按任意键退出图像
	_getch();
	closegraph();
	return;
}

int main() {
	int j, k;
		cout << "------------------------请输入功能序号------------------------\n";
		MALGraph malG;
		malG.InitMALGraph();
		ADLGraph adlG;
		adlG.InitADLGraph();
		cout << "                       1.深度优先遍历\n" << "                       2.广度优先遍历\n";
		cout << "                       3.深度优先生成树\n" << "                       4.广度优先生成树\n";
		cin >> j;
		cout << "------------------------请输入起点序号------------------------\n";
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