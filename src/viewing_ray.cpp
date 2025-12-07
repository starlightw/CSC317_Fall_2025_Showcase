#include "viewing_ray.h"

void viewing_ray(
  const Camera & camera,
  const int i,
  const int j,
  const int width,
  const int height,
  Ray & ray)
{
  ////////////////////////////////////////////////////////////////////////////
  // Add your code here:
    using Eigen::Vector3d;

    // Image plane bounds
    const double l = -0.5 * camera.width;
    const double r = 0.5 * camera.width;
    const double b = -0.5 * camera.height;
    const double t = 0.5 * camera.height;

    // Pixel center mapped to (u,v) coordinates on the image plane
    const double su = (r - l) / static_cast<double>(width);
    const double sv = (t - b) / static_cast<double>(height);
    const double u_coord = l + (j + 0.5) * su;
    const double v_coord = t - (i + 0.5) * sv;   // rows increase downward

    const Vector3d P = camera.e - camera.d * camera.w
        + u_coord * camera.u + v_coord * camera.v;

    // Ray so that origin + 1*direction = P
    ray.origin = camera.e;
    ray.direction = P - camera.e; // = -d*w + u_coord*u + v_coord*v


  ////////////////////////////////////////////////////////////////////////////
}
