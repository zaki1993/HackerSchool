#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct Round
{ 
  int x,y;
  int r;
};

bool crossing(Round &A, Round &B)
{ 
  return (A.r - B.r)*(A.r - B.r) < (B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y) && (B.x - A.x)*(B.x - A.x) + (B.y - A.y)*(B.y - A.y)  < (A.r + B.r)*(A.r + B.r);
}  
  

Round group[100];
int vertex[100];

vector<int> arr[100]; 
  


//bfs algorithm
void BredthFirstSearch(int x,int n) 
{ 
  for(int i=1; i<=n; i++)
    vertex[i] = -1;
  
  vertex[x] = 0;
  queue<int> q;
  q.push(x);
  
  while(!q.empty())
  { 
    x = q.front(); 
    q.pop();
    
    for(int i=0; i <arr[x].size(); i++)
    { 
      int y = arr[x][i];
  
      if(vertex[y]==-1)
      { 
        vertex[y] = vertex[x] + 1;
        q.push(y);
      }
    }
  } 
}       
  
int main()
{ 

  int n;  
  cin >> n;
  for(int i=1; i<=n; i++)
  {
        cin >> group[i].x >> group[i].y >> group[i].r;
  }
  for(int i=1; i<n; i++)
    for(int j=i+1; j<=n; j++)
      if(crossing(group[i], group[j])) 
      { 
      	// check if rounds are crossing and pushing them
        arr[i].push_back(j);
        arr[j].push_back(i);
      }
  
  BredthFirstSearch(1,n); 
  cout << vertex[n] << endl;
       
  return 0;
}    
