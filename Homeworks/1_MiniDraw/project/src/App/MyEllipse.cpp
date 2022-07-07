#include "MyEllipse.h"

MyEllipse::MyEllipse()
{
}

MyEllipse::~MyEllipse()
{
}

void MyEllipse::Draw(QPainter& painter)
{
	painter.drawEllipse(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
}