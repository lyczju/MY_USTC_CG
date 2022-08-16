#ifndef RBF_H
#define RBF_H
#include "Warping.h"

class RBFWarping : public Warping
{
public:
	RBFWarping();
	RBFWarping(int image_pos_x, int image_pos_y);
	~RBFWarping();
	void initAnchor(QVector<QPoint> start_list, QVector<QPoint> end_list);
	QPoint pointConvert(QPoint p);

private:
	Eigen::VectorXd weight_x, weight_y;
	QVector <double> radius;
	double mu;
};

#endif