#pragma once
#include <Eigen/dense>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace Eigen;
using namespace std;

typedef struct EDGE
{
	double x;
	double dx;
	int ymax;
} edge;

class ScanLine
{
public:
	ScanLine();
	~ScanLine();
	MatrixXd Scan(int width, int height, vector<int> x_points, vector<int> y_points);
private:
	void swap(int& a, int& b);
};