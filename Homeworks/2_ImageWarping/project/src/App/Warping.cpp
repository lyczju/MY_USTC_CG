#include "Warping.h"

double Warping::getDistance(QPoint p, QPoint q)
{
	return sqrt(pow((double)p.x() - (double)q.x(), 2) + pow((double)p.y() - (double)q.y(), 2));
}