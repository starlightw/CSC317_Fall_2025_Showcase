#include "Triangle.h"
#include "Ray.h"
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <iostream>

bool Triangle::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  ////////////////////////////////////////////////////////////////////////////
  // Replace with your code here:
  ////////////////////////////////////////////////////////////////////////////
  // Replace with your code here:
	using Eigen::Vector3d;

	const Vector3d& A = std::get<0>(corners);
	const Vector3d& B = std::get<1>(corners);
	const Vector3d& C = std::get<2>(corners);

	// Edges
	const Vector3d e1 = B - A;
	const Vector3d e2 = C - A;

	const Vector3d pvec = ray.direction.cross(e2);
	const double det = e1.dot(pvec);

	// If det is near zero, ray is parallel to triangle (or lies in plane)
	const double eps = 1e-10;
	if (std::abs(det) < eps) return false;

	const double inv_det = 1.0 / det;
	const Vector3d tvec = ray.origin - A;
	const double u = tvec.dot(pvec) * inv_det;
	if (u < 0.0 || u > 1.0) return false;

	const Vector3d qvec = tvec.cross(e1);
	const double v = ray.direction.dot(qvec) * inv_det;
	if (v < 0.0 || u + v > 1.0) return false;

	const double t_hit = e2.dot(qvec) * inv_det;
	if (t_hit < min_t) return false;

	t = t_hit;

	// Geometric (unnormalized) normal; direction follows vertex winding (A,B,C)
	Vector3d gn = e1.cross(e2);

	n = gn.normalized();
	return true;
	////////////////////////////////////////////////////////////////////////////
}


