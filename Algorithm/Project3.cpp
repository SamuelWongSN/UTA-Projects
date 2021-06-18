#include <iostream>
#include <vector>
#include <stack>

using namespace std;

int timestamp = 0;

typedef struct Node {
	int color;
	int pai;
	int starttime;
	int finishtime;
};
//存储每个节点所需信息
struct Node list[12];

vector<int> Topo_list;


void DFS_Visit(int u, vector<vector<int>> array) {

	cout << u << " ";

	list[u].color = 1;
	timestamp++;
	list[u].starttime = timestamp;

	for (int v = 0; v < array[u].size(); v++) {
		int tempv = array[u][v];
		if (list[tempv].color == 0) {
			list[tempv].pai = u;
			DFS_Visit(tempv, array);
		}
	}
	list[u].color = 2;
	Topo_list.push_back(u);

	timestamp++;
	list[u].finishtime = timestamp;
}

void DFS(vector<vector<int>> array) {
	
	for (int i = 1; i < 12; i++) {
		list[i].color = 0;
		list[i].pai = -1;
	}

	//从点7开始
	/*
	cout << "Root of tree : " << 7 << endl;
	cout << "Tree :";
	DFS_Visit(7, array);
	*/
	//从点11开始
	/*
	cout << "Root of tree : " << 11 << endl;
	cout << "Tree :";
	DFS_Visit(11, array);
	*/

	for (int i = 1; i < 12; i++) {
		if (list[i].color == 0) {
			cout << "Root of tree : " << i << endl;
			cout << "Tree :";
			DFS_Visit(i, array);
			cout << endl;
		}
	}
}

int main() {

	//初始化为12 * 12大小的数组用于存储
	//array[m][n] = 1,意味着有从m指向n的线
	vector<vector<int>> array;
	for (int i = 0; i < 12; i++) {
		vector<int> path;
		array.push_back(path);
	}

	//初始化二维数组
	array[1].push_back(2);
	array[2].push_back(3);
	array[3].push_back(6);
	array[4].push_back(1), array[4].push_back(5);
	array[5].push_back(1);
	array[6].push_back(8), array[6].push_back(9);
	array[7].push_back(4), array[7].push_back(10);
	array[8].push_back(5), array[8].push_back(9);
	array[10].push_back(8), array[10].push_back(11);
	array[11].push_back(9);

	//邻接矩阵
	cout << "邻接链表：" << endl;
	for (int i = 1; i < 12; i++) {
		cout << i;
		for (int j = 0; j < array[i].size(); j++) {
			cout << "->" << array[i][j];
		}
		cout << endl;
	}
	cout << endl;

	cout << "start from Node 1："<<endl;
	DFS(array);

	cout << endl << "Topo_list：" << endl;
	for (int i = Topo_list.size()-1; i >= 0; i--) {
		cout << Topo_list[i] << " ";
		Topo_list.pop_back();
	}
	cout << endl << endl;

	for (int i = 1; i < 12; i++) {
		cout << "Node " << i << " starttime = " << list[i].starttime << ", finishtime = " << list[i].finishtime << endl;
	}
	cout << endl;
	

	return 0;
}


