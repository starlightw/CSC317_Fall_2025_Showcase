#include "PointLight.h"

void PointLight::direction(
  const Eigen::Vector3d & q, Eigen::Vector3d & d, double & max_t) const
{
  ////////////////////////////////////////////////////////////////////////////
  // Add your code here:
  // Vector from query point q to the light position
	d = p - q;

	// Distance along this vector
	max_t = d.norm();

  ////////////////////////////////////////////////////////////////////////////
}
