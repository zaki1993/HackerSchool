#include<iostream>
#include<cmath>
using namespace std;
class Round {
private:
	double x, y, r;
public:
	Round(int _x = 1, int _y = 1, int _r = 2) {
		x = _x;
		y = _y;
		r = _r;
	}
	double getX() const {
		return x;
	}
	double getY() const {
		return y;
	}
	double getR() const {
		return r;
	}
	void setX(double _x) {
		x = _x;
	}
	void setY(double _y) {
		y = _y;
	}
	void setR(double _r) {
		if (_r >= 0) {
			r = _r;
		}
		else {
			cout << "Enter non negative value..!" << endl;
		}
	}
};
void EnterCircle(Round& p) {
	double temp;
	cout << "X: ";
	cin >> temp;
	
	p.setX(temp);
	cout << "Y: ";
	cin >> temp;

	p.setY(temp);
	cout << "R: ";
	cin >> temp;

	p.setR(temp);
}
bool crossed(Round& p, Round& q) {
	double result = sqrt((p.getX() - q.getX())*(p.getX() - q.getX())) + sqrt((p.getY() - q.getY())*(p.getY() - q.getY()));
	return (abs(p.getR() - q.getR()) <= result && result <= (p.getR() + q.getR()));
}

bool isTherePath(Round& p, Round& q) {
	return(crossed(p, q));
}
void markVisited(bool matrix[10][10], int row, int col,int &count) {
	if (matrix[row][col]) {
		matrix[row][col] = false;
		count++;
	}
}
bool canYouStepOn(bool matrix[10][10],int row,int col,int n) {
	return matrix[row][col] == true && row < n && col < n;
}
void isTherePathA1An(bool arr[10][10], int n, int startX, int startY, int &path) {
	

			if (canYouStepOn(arr, startX, startY,n)) {
				markVisited(arr, startX, startY,path);
			}
			if (canYouStepOn(arr,startX-1,startY,n)) {
				return isTherePathA1An(arr, n, startX - 1, startY, path);
			}
			if (canYouStepOn(arr,startX + 1,startY,n)) {
				return isTherePathA1An(arr, n, startX + 1, startY, path);
			}
			if (canYouStepOn(arr,startX,startY - 1,n)) {
				return isTherePathA1An(arr, n, startX, startY - 1, path);
			}
			if (canYouStepOn(arr,startX,startY + 1,n)) {
				return isTherePathA1An(arr, n, startX, startY + 1, path);
			}
			return;
}
void pushMatrixPath(bool arr[10][10], int n, Round *group) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (isTherePath(group[i], group[j])) {
				arr[i][j] = true;
			}
			else {
				arr[i][j] = false;
			}
		}
	}
}
int main() {
	int n;
	int path = -1;
	Round group[100];
	cout << "Enter number of the circles: ";
	cin >> n;
	for (int i = 0; i < n; i++) {
		cout <<"Circle "<< i+1 << ": " << endl;
		EnterCircle(group[i]);
	}
	bool matrix[10][10];

	pushMatrixPath(matrix, n, group);
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << matrix[i][j];
		}
		cout << endl;
	}
	isTherePathA1An(matrix, n, 0, 0, path);
	cout << path;
	system("pause");
	return 0;
}
