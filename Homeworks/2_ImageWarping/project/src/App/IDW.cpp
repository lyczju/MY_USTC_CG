#include "IDW.h"

IDWWarping::IDWWarping()
{
	this->image_pos_x = 0;
	this->image_pos_y = 0;
	mu = -2;
	total_weight = 0;
}

IDWWarping::IDWWarping(int image_pos_x, int image_pos_y)
{
	this->image_pos_x = image_pos_x;
	this->image_pos_y = image_pos_y;
	mu = -2;
	total_weight = 0;
}

IDWWarping::~IDWWarping()
{

}

void IDWWarping::initAnchor(QVector<QPoint> start_list, QVector<QPoint> end_list)
{
	for (int i = 0; i < start_list.size(); i++) {
		p_points.push_back(QPoint(start_list[i].x() - image_pos_x, start_list[i].y() - image_pos_y));
		q_points.push_back(QPoint(end_list[i].x() - image_pos_x, end_list[i].y() - image_pos_y));
	}
	T.resize(p_points.size(), 4);
	if (p_points.size() == 1) {
		T << 1, 0, 0, 1;
	}
	else if (p_points.size() == 2) {
		for (int i = 0; i < 2; i++) {
			T(i, 0) = (q_points[1].x() - q_points[0].x()) / (p_points[1].x() - p_points[0].x());
			T(i, 1) = 0;
			T(i, 2) = 0;
			T(i, 3) = (q_points[1].y() - q_points[0].y()) / (p_points[1].y() - p_points[0].y());
		}
	}
	else {
		for (int i = 0; i < p_points.size(); i++) {
			Eigen::MatrixXd A(2, 2), B(2, 2), T_(2, 2);
			A.setZero();
			B.setZero();
			for (int j = 0; j < p_points.size(); j++) {
				if (i != j) {
					double sigma = pow(getDistance(p_points[i], p_points[j]), mu);
					Eigen::Matrix2d diff_p;
					Eigen::Matrix2d diff_q;
					diff_p << (double)p_points[j].x() - (double)p_points[i].x(), 0, (double)p_points[j].y() - (double)p_points[i].y(), 0;
					diff_q << (double)q_points[j].x() - (double)q_points[i].x(), (double)q_points[j].y() - (double)q_points[i].y(), 0, 0;
					A += sigma * diff_p * diff_p.transpose();
					B += sigma * diff_p * diff_q;
				}
			}
			T_ = (A.inverse()) * B;
			/*T(i, 0) = T_(0, 0);
			T(i, 1) = T_(0, 1);
			T(i, 2) = T_(1, 0);
			T(i, 3) = T_(1, 1);*/
			T(i, 0) = 1;
			T(i, 1) = 0;
			T(i, 2) = 0;
			T(i, 3) = 1;
		}
	}
}

QPoint IDWWarping::pointConvert(QPoint p)
{
	Eigen::Vector2d converted_vector, p_vector;
	converted_vector.setZero();
	p_vector << p.x(), p.y();
	total_weight = 0;
	weight.clear();
	for (int i = 0; i < p_points.size(); i++) {
		weight.push_back(pow(getDistance(p_points[i], p), mu));
		total_weight += weight[i];
	}
	if (weight.empty()) {
		return p;
	}
	else {
		for (int i = 0; i < weight.size(); i++)
		{
			Eigen::MatrixXd T_i = T.row(i);
			T_i.resize(2, 2);
			Eigen::Vector2d p_vector_i, q_vector_i;
			q_vector_i << q_points[i].x(), q_points[i].y();
			p_vector_i << p_points[i].x(), p_points[i].y();
			converted_vector += (weight[i] / total_weight) * (q_vector_i + T_i * (p_vector - p_vector_i));
		}
	}
	return QPoint(converted_vector.x(), converted_vector.y());
}