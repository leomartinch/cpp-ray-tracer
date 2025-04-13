#ifndef MESH_H
#define MESH_H

#include "ray.h"
#include "material.h"

#include <vector>
#include <string>
#include <limits>

struct Face {
    int face_vertices[3];
};

struct RayHit {
    double hit_time;
	int face_id;
	class Hittable* hit_object = nullptr; // pointer to the hit object
};

class Hittable {
public:
    virtual RayHit hit(const ray& render_ray) = 0;
	virtual ~Hittable() = default;
};


class Mesh : public Hittable { // Mesh is a subclass of Hittable
public:
    Mesh(const std::string& filename);
	
	bool smooth_shading;
	std::string material_name;
	std::string object_name;

    virtual ~Mesh() = default; // have to find out what virtual and the ~ mean
    virtual RayHit hit(const ray& render_ray) override; // have to find out what the override means
    vec3 get_normal_vector(const int& face_index, const ray& render_ray) const;
	color get_color() const; // returns diffuse color
    color get_emission() const;
	float get_roughness() const;
	vec3 get_specular_direction(const ray& render_ray_direction, const vec3& face_normal);
	vec3 get_diffuse_direction(const vec3& face_normal);


private:
    std::vector<point3> vertices;     // list for vertices
    std::vector<Face> faces;		  // list for faces
    std::vector<vec3> vertex_normals; // list for vertex normals
	std::shared_ptr<Material> material_pointer;
    
    bool load_obj(const std::string& filename);
    void calculate_vertex_normals();
    double get_ray_mesh_intersection(const ray& render_ray, const point3 triangle[3]) const; 
};

#endif

