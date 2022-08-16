#include "Warping.h"

Warping::Warping()
{
	this->image_pos_x = 0;
	this->image_pos_y = 0;
}

Warping::Warping(int image_pos_x, int image_pos_y)
{
	this->image_pos_x = image_pos_x;
	this->image_pos_y = image_pos_y;
}

Warping::~Warping()
{
}

double Warping::getDistance(QPoint p, QPoint q)
{
	return sqrt(pow((double)p.x() - (double)q.x(), 2) + pow((double)p.y() - (double)q.y(), 2));
}

void Warping::imageWarping(QImage& image)
{
	QPoint converted_point;
	QImage tmp(image);
	for (int i = 0; i < image.width(); i++) {
		for (int j = 0; j < image.height(); j++) {
			image.setPixel(i, j, qRgb(255, 255, 255));
		}
	}
	for (int i = 0; i < image.width(); i++) {
		for (int j = 0; j < image.height(); j++) {
			converted_point = pointConvert(QPoint(i, j));
			if (converted_point.x() >= 0 && converted_point.x() < image.width() && converted_point.y() >= 0 && converted_point.y() < image.height()) {
				image.setPixel(converted_point, tmp.pixel(i, j));
			}
		}
	}
}