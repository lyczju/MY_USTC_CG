#include "Freehand.h"

Freehand::Freehand()
{
}

Freehand::~Freehand()
{
}

void Freehand::Draw(QPainter& painter)
{
	for (size_t i = 0; i < points.size() - 1; i++) {
		painter.drawLine(points[i], points[i + 1]);
	}
}

