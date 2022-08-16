#include "RBF.h"

RBFWarping::RBFWarping()
{
	this->image_pos_x = 0;
	this->image_pos_y = 0;
	mu = 0.5;
	weight_x.resize(0);
	weight_x.setZero();
	weight_y.resize(0);
	weight_y.setZero();
}

RBFWarping::RBFWarping(int image_pos_x, int image_pos_y)
{
	this->image_pos_x = image_pos_x;
	this->image_pos_y = image_pos_y;
	mu = 0.5;
	weight_x.resize(0);
	weight_x.setZero();
	weight_y.resize(0);
	weight_y.setZero();
}

RBFWarping::~RBFWarping()
{

}

void RBFWarping::initAnchor(QVector<QPoint> start_list, QVector<QPoint> end_list)
{
	for (int i = 0; i < start_list.size(); i++) {
		p_points.push_back(QPoint(start_list[i].x() - image_pos_x, start_list[i].y() - image_pos_y));
		q_points.push_back(QPoint(end_list[i].x() - image_pos_x, end_list[i].y() - image_pos_y));
	}
	if (p_points.size() > 1) {
		for (int i = 0; i < p_points.size(); i++) {
			int min_dist = 10000000;
			for (int j = 0; j < p_points.size(); j++) {
				if (i != j) {
					int dist = getDistance(p_points[i], p_points[j]);
					if (dist < min_dist) {
						min_dist = dist;
					}
				}
			}
			radius.push_back(min_dist);
		}

		Eigen::MatrixXd A_x(p_points.size(), p_points.size()), A_y(p_points.size(), p_points.size());
		Eigen::VectorXd b_x(p_points.size()), b_y(p_points.size());
		weight_x.resize(p_points.size());
		weight_y.resize(p_points.size());
		A_x.setZero();
		A_y.setZero();
		b_x.setZero();
		b_y.setZero();
		for (int i = 0; i < p_points.size(); i++) {
			b_x(i) = (double)q_points[i].x() - p_points[i].x();
			b_y(i) = (double)q_points[i].y() - p_points[i].y();
			for (int j = 0; j < p_points.size(); j++) {
				A_x(i, j) = pow(pow(getDistance(p_points[i], p_points[j]), 2) + pow(radius[i], 2), mu);
				A_y(i, j) = pow(pow(getDistance(p_points[i], p_points[j]), 2) + pow(radius[i], 2), mu);
			}
		}
		weight_x = A_x.colPivHouseholderQr().solve(b_x);
		weight_y = A_y.colPivHouseholderQr().solve(b_y);

	}
}

QPoint RBFWarping::pointConvert(QPoint p)
{
	double converted_x = 0;
	double converted_y = 0;
	if (p_points.size() > 1) {
		for (int i = 0; i < p_points.size(); i++) {
			converted_x += weight_x(i) * pow(pow(getDistance(p_points[i], p), 2) + pow(radius[i], 2), mu);
			converted_y += weight_y(i) * pow(pow(getDistance(p_points[i], p), 2) + pow(radius[i], 2), mu);
		}
	}
	else {
		converted_x = (double)q_points[0].x() - (double)p_points[0].x();
		converted_y = (double)q_points[0].y() - (double)p_points[0].y();
	}
	return QPoint(converted_x + p.x(), converted_y + p.y());
}