#include "raycolor.h"
#include "first_hit.h"
#include "blinn_phong_shading.h"
#include "reflect.h"
#include <cmath>

bool refract(const Eigen::Vector3d& I, const Eigen::Vector3d& N, double ior_ratio, Eigen::Vector3d& refracted)
{
    double dt = I.dot(N);
    double discriminant = 1.0 - ior_ratio * ior_ratio * (1.0 - dt * dt);
    if (discriminant > 0)
    {
        refracted = ior_ratio * (I - N * dt) - N * std::sqrt(discriminant);
        return true;
    }
    return false;
}

double fresnel_schlick(double cosine, double ior)
{
    double r0 = (1.0 - ior) / (1.0 + ior);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
}

bool raycolor(
    const Ray& ray,
    const double min_t,
    const std::vector< std::shared_ptr<Object> >& objects,
    const std::vector< std::shared_ptr<Light> >& lights,
    const int num_recursive_calls,
    Eigen::Vector3d& rgb)
{
    const double EPS = 1e-4;
    const int MAX_DEPTH = 10;

    int hit_id;
    double t;
    Eigen::Vector3d n;

	// If the ray misses everything, try to render sky
    if (!first_hit(ray, min_t, objects, hit_id, t, n))
    {
        // Calculate a gradient based on the ray's Y direction
        Eigen::Vector3d unit_direction = ray.direction.normalized();
        double p = 0.5 * (unit_direction.y() + 1.0);

        // Mix White (1,1,1) and Sky Blue (0.5, 0.7, 1.0)
        rgb = (1.0 - p) * Eigen::Vector3d(1.0, 1.0, 1.0) + p * Eigen::Vector3d(0.5, 0.7, 1.0);
        return false; 

        // Note: It ends up didn't render the sky correctly, but doing something weired in the octahedrons
    }

    // Handle entering vs exiting
    bool inside = false;
    Eigen::Vector3d normal = n;
    if (n.dot(ray.direction) > 0)
    {
        normal = -n;
        inside = true;
    }

    const auto& mat = objects[hit_id]->material;

    Eigen::Vector3d Lo = blinn_phong_shading(ray, hit_id, t, normal, objects, lights);
    Eigen::Vector3d Lr(0.0, 0.0, 0.0);

    if (num_recursive_calls < MAX_DEPTH)
    {
        if (mat->is_refractive)
        {
            double ior = mat->ior;
            double eta = inside ? ior : (1.0 / ior);
            double cosine = -ray.direction.normalized().dot(normal);

            double kr = fresnel_schlick(cosine, ior);

            Eigen::Vector3d refract_color(0.0, 0.0, 0.0);
            Eigen::Vector3d refract_dir;
            bool tir = !refract(ray.direction.normalized(), normal, eta, refract_dir);

            if (tir) {
                kr = 1.0;
            }
            else {
                Ray refract_ray;
                refract_ray.origin = (ray.origin + t * ray.direction) + EPS * refract_dir;
                refract_ray.direction = refract_dir;
                raycolor(refract_ray, EPS, objects, lights, num_recursive_calls + 1, refract_color);
            }

            Eigen::Vector3d reflect_color(0.0, 0.0, 0.0);
            Eigen::Vector3d reflect_dir = reflect(ray.direction.normalized(), normal).normalized();
            Ray reflect_ray;
            reflect_ray.origin = (ray.origin + t * ray.direction) + EPS * reflect_dir;
            reflect_ray.direction = reflect_dir;
            raycolor(reflect_ray, EPS, objects, lights, num_recursive_calls + 1, reflect_color);

            // Mix reflection and refraction
            Lr = (kr * reflect_color) + ((1.0 - kr) * refract_color.cwiseProduct(mat->kd));

			// Note: Same for this one, didn't render correctly, but doing something weired in the octahedrons
        }
        else if (mat->km.maxCoeff() > 0.0)
        {
            Eigen::Vector3d d_refl = reflect(ray.direction.normalized(), normal).normalized();
            Ray refl_ray;
            refl_ray.origin = (ray.origin + t * ray.direction) + EPS * d_refl;
            refl_ray.direction = d_refl;

            Eigen::Vector3d child_rgb;
            raycolor(refl_ray, EPS, objects, lights, num_recursive_calls + 1, child_rgb);
            Lr = mat->km.cwiseProduct(child_rgb);
        }
    }

    rgb = (Lo + Lr).cwiseMax(0.0);
    return true;
}