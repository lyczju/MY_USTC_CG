#pragma once
#include <Eigen\Sparse>
#include <Eigen\Dense>
#include <iostream>
#include <cmath>
using namespace Eigen;
using namespace std;

typedef Eigen::SparseMatrix<double> SparseMatrixType;
typedef Eigen::Triplet<double> T;
typedef Eigen::SimplicialCholesky<SparseMatrixType> Solver;

class PoissonEditing
{
public:
	PoissonEditing();
	~PoissonEditing();
	Solver* Get_Poisson_solver(MatrixXd selected_region);
	VectorXd Use_Poisson_solver(MatrixXd front, MatrixXd background, MatrixXd selected_region, Solver* p_A);
};
