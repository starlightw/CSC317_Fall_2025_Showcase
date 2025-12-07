#include "Plane.h"
#include "Ray.h"
#include <iostream>
bool Plane::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  ////////////////////////////////////////////////////////////////////////////
  // Replace with your code here:
	const double EPS = 1e-9;

	// Denominator of intersection formula
	double denom = normal.dot(ray.direction);

	// If denom is ~0, the ray is parallel to the plane ¡ú no hit
	if (std::abs(denom) < EPS) return false;

	// Solve for t: (point - origin) ¡¤ normal / (dir ¡¤ normal)
	t = (point - ray.origin).dot(normal) / denom;

	// Only count forward intersections beyond min_t
	if (t < min_t) return false;

	// Return the plane¡¯s normal (make sure it¡¯s unit length)
	n = normal.normalized();

	return true;
  ////////////////////////////////////////////////////////////////////////////
}

