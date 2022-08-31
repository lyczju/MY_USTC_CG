#pragma once

#include <vector>
#include "shape.h"

using namespace std;

class Freehand :public Shape
{
public:
	Freehand();
	~Freehand();

	void Draw(QPainter& painter);


};
