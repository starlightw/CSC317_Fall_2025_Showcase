#include "blinn_phong_shading.h"
#include "first_hit.h"
#include <iostream>
#include <algorithm>
#include <cmath>

Eigen::Vector3d blinn_phong_shading(
    const Ray& ray,
    const int& hit_id,
    const double& t,
    const Eigen::Vector3d& n,
    const std::vector< std::shared_ptr<Object> >& objects,
    const std::vector<std::shared_ptr<Light> >& lights)
{
    
    const double EPS = 1e-4;
    const double ia = 0.1;

    const Eigen::Vector3d p = ray.origin + t * ray.direction;
    const Eigen::Vector3d v = (-ray.direction).normalized();
    const auto& mat = objects[hit_id]->material;

    Eigen::Vector3d L = ia * mat->ka;

    for (const auto& light : lights)
    {
        Eigen::Vector3d d_to_light;
        double max_t_to_light;
        light->direction(p, d_to_light, max_t_to_light);

        const Eigen::Vector3d l = d_to_light.normalized();

        Ray shadow_ray;
        shadow_ray.origin = p + EPS * l;
        shadow_ray.direction = l;

        int s_hit_id;
        double s_t;
        Eigen::Vector3d s_n;

        bool in_shadow = false;
        if (first_hit(shadow_ray, EPS, objects, s_hit_id, s_t, s_n))
        {
            if (s_t < max_t_to_light)
            {
                // Glass objects (refractive) do not cast shadows
                if (!objects[s_hit_id]->material->is_refractive) {
                    in_shadow = true;
                }
            }
        }

        if (in_shadow) continue;

        const Eigen::Vector3d h = (v + l).normalized();
        const double nh = std::max(0.0, n.dot(h));
        const Eigen::Vector3d specular =
            mat->ks.cwiseProduct(light->I) * std::pow(nh, mat->phong_exponent);

        // Glass usually has 0 diffuse, keep this logic just in case
        Eigen::Vector3d diffuse(0, 0, 0);
        if (!mat->is_refractive) {
            const double nl = std::max(0.0, n.dot(l));
            diffuse = mat->kd.cwiseProduct(light->I) * nl;
        }

        L += diffuse + specular;
    }

    return L.cwiseMax(0.0);
}