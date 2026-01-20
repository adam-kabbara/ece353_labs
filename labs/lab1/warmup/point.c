#include <assert.h>
#include "common.h"
#include "point.h"
#include <math.h>

void point_translate(struct point *p, double x, double y)
{
	int new_x = x + point_X(p);
	int new_y = y + point_Y(p); // reminder, eqivanlent to p->y += y 
	point_set(p, new_x, new_y);
}

double point_distance(const struct point *p1, const struct point *p2)
{
	return sqrt(pow((point_X(p1) - point_X(p2)),2) + pow((point_Y(p1) - point_Y(p2)),2));
}

int point_compare(const struct point *p1, const struct point *p2)
{
	double dist1 = sqrt(pow(point_X(p1), 2) + pow(point_Y(p1), 2));
	double dist2 = sqrt(pow(point_X(p2), 2) + pow(point_Y(p2), 2));

	if (dist1 == dist2){return 0;}
	else if (dist1 > dist2){return 1;}
	else {return -1;}
}
