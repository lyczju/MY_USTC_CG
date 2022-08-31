#pragma once

#include <QtGui>
#include <vector>

class Shape
{
public:
	Shape();
	virtual ~Shape();
	virtual void Draw(QPainter& paint) = 0;
	void set_start(QPoint s);
	void set_end(QPoint e);

public:
	enum Type
	{
		kRect = 1,
		kFreehand = 2,
	};

protected:
	QPoint start;
	QPoint end;

public:
	std::vector<QPoint> points;
};