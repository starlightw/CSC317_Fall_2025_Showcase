#include "Sphere.h"
#include "Ray.h"


bool Sphere::intersect(
  const Ray & ray, const double min_t, double & t, Eigen::Vector3d & n) const
{
  ////////////////////////////////////////////////////////////////////////////
  // Replace with your code here:
    Eigen::Vector3d oc = ray.origin - center;
    double a = ray.direction.dot(ray.direction);
    double b = 2.0 * ray.direction.dot(oc);
    double c = oc.dot(oc) - radius * radius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return false; // no real roots, no hit

    double sqrt_disc = std::sqrt(discriminant);
    double t1 = (-b - sqrt_disc) / (2.0 * a);
    double t2 = (-b + sqrt_disc) / (2.0 * a);

    // pick the smallest valid t ¡Ý min_t
    double chosen_t = std::numeric_limits<double>::infinity();
    if (t1 >= min_t && t1 < chosen_t) chosen_t = t1;
    if (t2 >= min_t && t2 < chosen_t) chosen_t = t2;

    if (chosen_t == std::numeric_limits<double>::infinity())
        return false;

    t = chosen_t;
    Eigen::Vector3d p = ray.origin + t * ray.direction;  // hit point
    n = (p - center).normalized();                       // outward normal
    return true;
  ////////////////////////////////////////////////////////////////////////////
}

