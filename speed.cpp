#include <iostream>
#include "stdio.h"
#include "string.h"
#include "vector"
#include "algorithm"
#include <map>
using namespace std;
#define pii pair<int, int>
#define pip pair<int, pii>
#define F first
#define S second

inline void inp(int *n)	//fast input function
{
    *n = 0;
    int ch = getchar_unlocked();
    int sign = 1;
    while(ch < '0' || ch > '9') 
    {
        if (ch == '-') 
            sign = -1;
        ch = getchar_unlocked();
    }
    while(ch >= '0' && ch <= '9')
        (*n) = ((*n)<<3) + ((*n)<<1) + ch - '0', ch = getchar_unlocked();
    *n = (*n)*sign;
}

const int MAX = 10010;
int maxW = 0;
int minW = 0;
//class implementing Union Find Data Structure with Path Compression
class Union_Find
{
  public:

	int id[MAX], sz[MAX];
	Union_Find(int n)	//class constructor
	{
		for (int i = 0; i < n; ++i)
		{
			id[i] = i;
			sz[i] = 1;
		}
	}
	
	int root(int i)
	{
		while(i != id[i])
		{
			id[i] = id[id[i]];	//path Compression
			i = id[i];
		}
		return i;
	}
	int find(int p, int q)
	{
		return root(p)==root(q);
	}
	void unite(int p, int q)
	{
		int i = root(p);
		int j = root(q);

		if(sz[i] < sz[j])
		{
			id[i] = j;
			sz[j] += sz[i];
		}
		else
		{
			id[j] = i;
			sz[i] += sz[j];
		}
	}
};


vector< pip > graph;	
int n, e;
long long int T;
int numberEdges = 0;
int diferences[100];
int start = 0;
int end = 0;
bool check = false;
int indexW = 0;
int numE = 0;
bool checkOther = false;
std::map<int,bool> visited;

int numPairs = 0;
std::map<int,int> temp;
int arr[100];
void Kruskal_MST()
{
	for (int i = 0; i < e; i++)
	{
		for (int j = 0; j < e; j++){	
			for(int m = 1;m<=n;m++){
				visited[m] = false;
			}
			if(graph[i].F < graph[j].F){
				minW = graph[i].F;
				maxW = graph[j].F;
			}
			else{
				minW = graph[j].F;
				maxW = graph[i].F;
			}
			Union_Find UF(n);
			int u, v;
			start = 0;
			end = 0;
			checkOther = false;
			for (int k = 0; k < e; k++)
			{	
					if(graph[k].F>=minW && graph[k].F<=maxW && i != j){
						if(visited[graph[k].S.S+1]==false || visited[graph[k].S.F+1]==false ){
						{	
							if(!checkOther){start=graph[k].F; checkOther= true;}
							end = graph[k].F;
							visited[graph[k].S.F+1] = true;
							visited[graph[k].S.S+1] = true;
							std::cout<<graph[k].S.F+1<<"-"<<graph[k].S.S+1<<": "<<graph[k].F<<" "<<minW<<"-"<<maxW<<std::endl;
							temp[graph[k].S.F+1] = graph[k].S.S+1;
							arr[numPairs] = graph[k].S.F+1;
							numPairs++;
							arr[numPairs] = graph[k].S.S+1;
							numPairs++;
						}
					}	
					
				}
			}
			for(int d = 0;d<numPairs;d++){
				std::cout<<arr[d]<<" ";
			}
			memset(arr,0,100);
			numPairs = 0;
			for (int p = 1; p <= n; p++)
					{
						if(visited[p]==true){
							check = true;
						}
						else{
							check = false;
							std::cout<<std::endl;
							std::cout<<p <<" unvisited"<<std::endl;
							break;
						}
					}
				std::cout<<"YES"<<std::endl;
				check = false;
				std::cout<<end - start<<std::endl;
				diferences[indexW] = end-start;
				indexW++;
			}
				numE = 0 ;
			std::cout<<"------"<<std::endl;

			
		}	
			
			
	}
}

int main()
{
	int u, v, c;
	inp(&n);//enter the no of nodes
	inp(&e);//enter the no of edges
	
	graph.resize(e);
	for (int i = 0; i < e; ++i)
	{
		inp(&u);	//enter vertex u
		inp(&v);	//enter vertex v
		inp(&c);	//enter cost of edge (u,v)
		u--;
		v--;
		graph[i] = pip( c, pii(u,v));
	}
	sort(graph.begin(), graph.end());	//sort the edges in increasing order of cost
	
	T = 0;
	Kruskal_MST();
	for(int i = 0;i<indexW;i++){
		std::cout<<diferences[i]<<std::endl;
	}

	return 0;
}
