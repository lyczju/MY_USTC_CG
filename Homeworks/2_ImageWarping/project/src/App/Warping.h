#ifndef WARPING_H
#define WARPING_H
#include <qvector.h>
#include <qimage.h>
#include <Eigen/Dense>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

class Warping
{
public:
	int image_pos_x;
	int image_pos_y;

	Warping();
	Warping(int image_pos_x, int image_pos_y);
	virtual ~Warping();
	double getDistance(QPoint p, QPoint q);
	void imageWarping(QImage& image);
	virtual void initAnchor(QVector<QPoint> start_list, QVector<QPoint> end_list) = 0;
	virtual QPoint pointConvert(QPoint p) = 0;

protected:
	QVector<QPoint> p_points;
	QVector<QPoint> q_points;
};

#endif