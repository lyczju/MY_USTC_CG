#ifndef IDW_H
#define IDW_H
#include "Warping.h"

class IDWWarping : public Warping
{
public:
	IDWWarping();
	IDWWarping(int image_pos_x, int image_pos_y);
	~IDWWarping();
	void initAnchor(QVector<QPoint> start_list, QVector<QPoint> end_list);
	QPoint pointConvert(QPoint p);

private:
	Eigen::MatrixXd T;
	QVector <double> weight;
	double total_weight;
	int mu;
};

#endif