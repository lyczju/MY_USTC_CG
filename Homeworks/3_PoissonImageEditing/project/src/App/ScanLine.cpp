#include "ScanLine.h"

ScanLine::ScanLine()
{
}

ScanLine::~ScanLine()
{
}

void ScanLine::swap(int& a, int& b)
{
	int tmp = a;
	a = b;
	b = tmp;
}

MatrixXd ScanLine::Scan(int width, int height, vector<int> x_points, vector<int> y_points)
{
	MatrixXd selected_region = MatrixXd::Zero(width, height);
	vector<vector<edge>> NET(height, vector<edge>(0));

	// generate edge table
	for (size_t i = 0; i < y_points.size(); i++) {
		int prev = (int)i - 1;
		if (i == 0)
			prev = y_points.size() - 1;
		edge new_edge;
		if (y_points[prev] > y_points[i]) {
			new_edge.x = x_points[i];
			new_edge.dx = (x_points[i] - x_points[prev]) / (y_points[i] - y_points[prev]);
			new_edge.ymax = y_points[prev];
			NET[y_points[i]].push_back(new_edge);
		}
		else if (y_points[prev] < y_points[i]) {
			new_edge.x = x_points[prev];
			new_edge.dx = (x_points[i] - x_points[prev]) / (y_points[i] - y_points[prev]);
			new_edge.ymax = y_points[i];
			NET[y_points[prev]].push_back(new_edge);
		}
		else {
			int min_x = min(x_points[i], x_points[prev]);
			int max_x = max(x_points[i], x_points[prev]);
			for (int j = min_x; j < max_x; j++) {
				selected_region(j, y_points[i]) = 1;
			}
		}
	}

	vector<vector<int>> ET(height, vector<int>(0));
	for (int i = 0; i < height; i++) {
		if (!NET[i].empty()) {
			for (int j = 0; j < NET[i].size(); j++) {
				double rx = NET[i][j].x;
				for (int k = i; k < NET[i][j].ymax; k++) {
					ET[k].push_back((int)round(rx));
					rx += NET[i][j].dx;
				}
			}
		}
	}

	// sort by edge.x
	for (int i = 0; i < ET.size(); i++)
	{
		if (!ET[i].empty()) {
			for (int j = 1; j < ET[i].size(); j++) {
				int tmp = ET[i][j];
				for (int k = j - 1; j >= 0; j--) {
					if (ET[i][k] > tmp)
						ET[i][k + 1] = ET[i][k];
					else
					{
						ET[i][k + 1] = tmp;
						break;
					}
					if (k == 0)
						ET[i][0] = tmp;
				}
			}
		}
	}

	// fill selected region
	for (int i = 0; i < height; i++) {
		if (!ET[i].empty()) {
			for (int k = 0; k < ET[i].size(); k += 2) {
				for (int j = ET[i][k] + 1; j < ET[i][k + 1]; j++)
					selected_region(i, j) = 1;
			}
		}
	}
}