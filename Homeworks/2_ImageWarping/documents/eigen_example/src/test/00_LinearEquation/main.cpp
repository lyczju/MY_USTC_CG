#include <iostream>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main() {
	MatrixXf A(4, 4);

	float data[4][4] = {
		{2, 0, 6, 1},
		{0, 1, 4, 0},
		{0, 5, 1, 10},
		{0, 7, 0, 8}
	};

	for (size_t row = 0; row < 4; row++) {
		for (size_t col = 0; col < 4; col++)
			A(row, col) = data[row][col];
	}

	MatrixXf b(4, 2);
	for (size_t row = 0; row < 4; row++) {
		b(row, 0) = 3;
		b(row, 1) = 5;
	}

	MatrixXf x = A.colPivHouseholderQr().solve(b);

	cout << x << endl;

	return 0;
}
