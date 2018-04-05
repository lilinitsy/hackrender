#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "PointMass.h"

struct Triangle
{
	PointMass *p1;
	PointMass *p2;
	PointMass *p3;
	float area;

	Triangle(PointMass *pee1, PointMass *pee2, PointMass *pee3)
	{
		p1 = pee1;
		p2 = pee2;
		p3 = pee3;

		calculate_area();
	}

	void calculate_area()
	{
		printf("p1 position: %f %f %f\n", p1->position.x, p1->position.y, p1->position.z);
		area = (glm::distance(p1->position, p2->position) * glm::distance(p1->position, p3->position)) / 2.0f;
	}
};

#endif