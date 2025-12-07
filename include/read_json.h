#ifndef READ_JSON_H
#define READ_JSON_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

// Forward declarations
struct Object;
struct Light;
struct Camera;

inline bool read_json(
    const std::string& filename,
    Camera& camera,
    std::vector<std::shared_ptr<Object> >& objects,
    std::vector<std::shared_ptr<Light> >& lights);

// Implementation
#include <json.hpp>
#include "readSTL.h"
#include "dirname.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "TriangleSoup.h"
#include "Light.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Camera.h" 
#include <Eigen/Geometry>
#include <fstream>
#include <iostream>
#include <cassert>

inline bool read_json(
    const std::string& filename,
    Camera& camera,
    std::vector<std::shared_ptr<Object> >& objects,
    std::vector<std::shared_ptr<Light> >& lights)
{
    using json = nlohmann::json;

    std::ifstream infile(filename);
    if (!infile) return false;
    json j;
    infile >> j;

    // Helper to parse Vector3d safely
    auto parse_Vector3d = [](const json& j, const Eigen::Vector3d& def = Eigen::Vector3d(0, 0, 0)) -> Eigen::Vector3d
        {
            if (j.is_null()) return def; // Handle missing/null entries
            return Eigen::Vector3d(j[0], j[1], j[2]);
        };

    // parse camera
    auto parse_camera =
        [&parse_Vector3d](const json& j, Camera& camera)
        {
            assert(j["type"] == "perspective" && "Only handling perspective cameras");
            camera.d = j.value("focal_length", 1.0);
            camera.e = parse_Vector3d(j["eye"]);
            camera.v = parse_Vector3d(j["up"]).normalized();
            camera.w = -parse_Vector3d(j["look"]).normalized();
            camera.u = camera.v.cross(camera.w);
            camera.height = j.value("height", 1.0);
            camera.width = j.value("width", 1.0);
        };
    parse_camera(j["camera"], camera);

    // Parse materials
    std::unordered_map<std::string, std::shared_ptr<Material> > materials;
    auto parse_materials = [&parse_Vector3d](
        const json& j,
        std::unordered_map<std::string, std::shared_ptr<Material> >& materials)
        {
            materials.clear();
            for (const json& jmat : j)
            {
                std::string name = jmat["name"];
                std::shared_ptr<Material> material(new Material());

                // Use .value() or checks to prevent crashes on missing keys
                if (jmat.count("ka")) material->ka = parse_Vector3d(jmat["ka"]);
                if (jmat.count("kd")) material->kd = parse_Vector3d(jmat["kd"]);
                if (jmat.count("ks")) material->ks = parse_Vector3d(jmat["ks"]);
                if (jmat.count("km")) material->km = parse_Vector3d(jmat["km"]);

                material->phong_exponent = jmat.value("phong_exponent", 1.0);

                // Safe parsing for refraction
                material->is_refractive = jmat.value("is_refractive", false);
                material->ior = jmat.value("ior", 1.0);

                materials[name] = material;
            }
        };
    parse_materials(j["materials"], materials);

    auto parse_lights = [&parse_Vector3d](
        const json& j,
        std::vector<std::shared_ptr<Light> >& lights)
        {
            lights.clear();
            for (const json& jlight : j)
            {
                if (jlight["type"] == "directional")
                {
                    std::shared_ptr<DirectionalLight> light(new DirectionalLight());
                    light->d = parse_Vector3d(jlight["direction"]).normalized();
                    light->I = parse_Vector3d(jlight["color"]);
                    lights.push_back(light);
                }
                else if (jlight["type"] == "point")
                {
                    std::shared_ptr<PointLight> light(new PointLight());
                    light->p = parse_Vector3d(jlight["position"]);
                    light->I = parse_Vector3d(jlight["color"]);
                    lights.push_back(light);
                }
            }
        };
    parse_lights(j["lights"], lights);

    auto parse_objects = [&parse_Vector3d, &filename, &materials](
        const json& j,
        std::vector<std::shared_ptr<Object> >& objects)
        {
            objects.clear();
            for (const json& jobj : j)
            {
                if (jobj["type"] == "sphere")
                {
                    std::shared_ptr<Sphere> sphere(new Sphere());
                    sphere->center = parse_Vector3d(jobj["center"]);
                    sphere->radius = jobj.value("radius", 1.0);
                    objects.push_back(sphere);
                }
                else if (jobj["type"] == "plane")
                {
                    std::shared_ptr<Plane> plane(new Plane());
                    plane->point = parse_Vector3d(jobj["point"]);
                    plane->normal = parse_Vector3d(jobj["normal"]).normalized();
                    objects.push_back(plane);
                }
                else if (jobj["type"] == "triangle")
                {
                    std::shared_ptr<Triangle> tri(new Triangle());
                    tri->corners = std::make_tuple(
                        parse_Vector3d(jobj["corners"][0]),
                        parse_Vector3d(jobj["corners"][1]),
                        parse_Vector3d(jobj["corners"][2]));
                    objects.push_back(tri);
                }
                else if (jobj["type"] == "soup")
                {
                    std::vector<std::vector<double> > V;
                    std::vector<std::vector<double> > F;
                    std::vector<std::vector<int> > N;
                    {
#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR std::string("\\")
#else
#define PATH_SEPARATOR std::string("/")
#endif
                        const std::string stl_path = jobj["stl"];
                        igl::readSTL(
                            igl::dirname(filename) +
                            PATH_SEPARATOR +
                            stl_path,
                            V, F, N);
                    }
                    std::shared_ptr<TriangleSoup> soup(new TriangleSoup());
                    for (int f = 0; f < F.size(); f++)
                    {
                        std::shared_ptr<Triangle> tri(new Triangle());
                        tri->corners = std::make_tuple(
                            Eigen::Vector3d(V[F[f][0]][0], V[F[f][0]][1], V[F[f][0]][2]),
                            Eigen::Vector3d(V[F[f][1]][0], V[F[f][1]][1], V[F[f][1]][2]),
                            Eigen::Vector3d(V[F[f][2]][0], V[F[f][2]][1], V[F[f][2]][2])
                        );
                        soup->triangles.push_back(tri);
                    }
                    objects.push_back(soup);
                }
                if (jobj.count("material"))
                {
                    if (materials.count(jobj["material"]))
                    {
                        objects.back()->material = materials[jobj["material"]];
                    }
                }
            }
        };
    parse_objects(j["objects"], objects);

    return true;
}

#endif