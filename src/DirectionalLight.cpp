#include "DirectionalLight.h"
#include <limits>

void DirectionalLight::direction(
  const Eigen::Vector3d & q, Eigen::Vector3d & d, double & max_t) const
{
  ////////////////////////////////////////////////////////////////////////////
  // Add your code here

  // the light from q to the light is just -d.
	d = -this->d;

	// Light is at infinity
	max_t = std::numeric_limits<double>::infinity();
  ////////////////////////////////////////////////////////////////////////////
}

