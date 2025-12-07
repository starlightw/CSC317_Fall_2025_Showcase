#include "first_hit.h"

bool first_hit(
  const Ray & ray, 
  const double min_t,
  const std::vector< std::shared_ptr<Object> > & objects,
  int & hit_id, 
  double & t,
  Eigen::Vector3d & n)
{
  ////////////////////////////////////////////////////////////////////////////
  // Replace with your code here:
    bool found = false;
    double best_t = std::numeric_limits<double>::infinity();
    Eigen::Vector3d best_n = Eigen::Vector3d::Zero();
    int best_id = -1;

    for (int i = 0; i < (int)objects.size(); ++i)
    {
        double ti;
        Eigen::Vector3d ni;
        if (objects[i]->intersect(ray, min_t, ti, ni))        // ti >= min_t inside
        {
            if (ti < best_t)
            {
                best_t = ti;
                best_n = ni;                                      // assume ni already normalized
                best_id = i;
                found = true;
            }
        }
    }

    if (found)
    {
        t = best_t;
        n = best_n;
        hit_id = best_id;
    }
    return found;
  ////////////////////////////////////////////////////////////////////////////
}
