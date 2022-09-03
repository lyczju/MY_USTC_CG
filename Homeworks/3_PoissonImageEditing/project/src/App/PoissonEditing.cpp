#include "PoissonEditing.h"

PoissonEditing::PoissonEditing()
{
}

PoissonEditing::~PoissonEditing()
{
}

Solver* PoissonEditing::Get_Poisson_solver(MatrixXd selected_region)
{
	int height = (int)selected_region.rows();
	int width = (int)selected_region.cols();
	int N = height * width;
	SparseMatrix<double> A(N, N);
	vector<T> tripletlist;

	// compute the coefficient matrix
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int center = i * width + j;
			if (selected_region(i, j) == 0)
				tripletlist.push_back(T(center, center, 1));
			else {
				tripletlist.push_back(T(center, center, 4));
				if (selected_region(i - 1, j) == 1)
					tripletlist.push_back(T(center, center - width, -1));
				if (selected_region(i + 1, j) == 1)
					tripletlist.push_back(T(center, center + width, -1));
				if (selected_region(i, j - 1) == 1)
					tripletlist.push_back(T(center, center - 1, -1));
				if (selected_region(i, j + 1) == 1)
					tripletlist.push_back(T(center, center + 1, -1));
			}
		}
	}
	A.setFromTriplets(tripletlist.begin(), tripletlist.end());
	A.makeCompressed();
	Solver* p_A = new Solver(A);
	return p_A;
}

VectorXd PoissonEditing::Use_Poisson_solver(MatrixXd front, MatrixXd background, MatrixXd selected_region, Solver* p_A)
{
	int height = (int)front.rows();
	int width = (int)background.cols();
	int N = height * width;
	VectorXd b = VectorXd::Zero(N);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int center = i * width + j;
			if (selected_region(i, j) == 0)
				b(center) = front(i, j);
			else {
				if (selected_region(i - 1, j) != 1)
					b(center) += front(i - 1, j);
				if (selected_region(i + 1, j) != 1)
					b(center) += front(i + 1, j);
				if (selected_region(i, j - 1) != 1)
					b(center) += front(i, j - 1);
				if (selected_region(i, j + 1) != 1)
					b(center) += front(i, j + 1);
				b(center) += 4 * background(i, j) - background(i + 1, j) - background(i - 1, j) - background(i, j + 1) - background(i, j - 1);
			}
		}
	}
	VectorXd x;
	x = p_A->solve(b);
	return x;
}