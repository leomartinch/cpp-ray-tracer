#include "mesh.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <filesystem>
using namespace std;

// OBJ MESH LOADER //
// Leo Martin (2025) //


Mesh::Mesh(const std::string& filename) {
    if (!load_obj(filename)) {
        cerr << "Failed to load mesh from: " << filename << "\n";
    }
    calculate_vertex_normals();
}

bool Mesh::load_obj(const string& filename) {
	std::filesystem::path obj_path(filename);
	std::filesystem::path directory = obj_path.parent_path();

    ifstream obj(filename);
    if (!obj.is_open()) {
        return false;
    }

    string material_name;
    string mtl_file;

    string line;
    while (getline(obj, line)) {
        if (line.empty() || line[0] == '#')
            continue;

        istringstream stream(line);
        string prefix;
        stream >> prefix;

        if (prefix == "o") { // object name
            stream >> object_name;
        }
        else if (prefix == "s") { // smooth shading flag
            stream >> smooth_shading;
        }
		else if (prefix == "mtllib") { // material file
            stream >> mtl_file;	
        }
        else if (prefix == "usemtl") { // material name

            stream >> material_name;
        }
		else if (prefix == "v") { // vertex
            float x, y, z;
            stream >> x >> y >> z;
            vertices.push_back(point3(x, y, z));
        }
        else if (prefix == "f") { // face
            Face current_face;
            string token;
            int i = 0;
            while (stream >> token && i < 3) { // have to make a system that works with quads and triangulates the face
                // break down face and only take the index (index/vertex_texture/vertex_normal)
                replace(token.begin(), token.end(), '/', ' ');
                istringstream point_stream(token);
                int vertex_index;
                point_stream >> vertex_index;
                current_face.face_vertices[i] = vertex_index - 1; // .obj indices start at 1
                i++;
            } 
            faces.push_back(current_face);
        }
    }
    obj.close();

    // create material object and make pointer that links to it	
    std::filesystem::path mtl_file_path = directory / mtl_file;
    material_pointer = make_shared<Material>(mtl_file_path.string(), material_name);
    return true;
}

void Mesh::calculate_vertex_normals() { // average from adjacent faces
    // initialize vertex normals to zero
    vertex_normals.assign(vertices.size(), vec3(0.0f, 0.0f, 0.0f));

    for (const Face &face : faces) {
        int index_0 = face.face_vertices[0];
        int index_1 = face.face_vertices[1];
        int index_2 = face.face_vertices[2];

        vec3 edge_1 = vertices[index_1] - vertices[index_0];
        vec3 edge_2 = vertices[index_2] - vertices[index_0];

        vec3 face_normal = normalize(cross(edge_1, edge_2));

        vertex_normals[index_0] += face_normal;
        vertex_normals[index_1] += face_normal;
        vertex_normals[index_2] += face_normal;
    }

    for (vec3 &normal : vertex_normals) { // normalize each vertex normal
        normal = normalize(normal);
    }
}

double Mesh::get_ray_mesh_intersection(const ray& render_ray, const point3 triangle[3]) const {
    vec3 edge_1 = triangle[1] - triangle[0];
    vec3 edge_2 = triangle[2] - triangle[0];

    vec3 p_vector = cross(render_ray.direction(), edge_2);
    double determinant = dot(edge_1, p_vector);

    const double kepsilon = 1e-6;
    if (fabs(determinant) < kepsilon) {
        return -1.0; // ray is parallel to triangle
    }

    double inverse_determinant = 1.0 / determinant;
    vec3 ray_to_vertex0 = render_ray.origin() - triangle[0];

    double u = dot(ray_to_vertex0, p_vector) * inverse_determinant;
    if (u < 0 || u > 1) {
        return -1.0;
    }

    vec3 q = cross(ray_to_vertex0, edge_1);
    double v = dot(render_ray.direction(), q) * inverse_determinant;
    if (v < 0 || (u + v) > 1) {
        return -1.0;
    }

    double t = dot(edge_2, q) * inverse_determinant;
    if (t > 0) {
        return t;
    }
    return -1.0;
}


vec3 Mesh::get_normal_vector(const int& face_index, const ray& render_ray) const {
    point3 triangle[3];
    triangle[0] = vertices[faces[face_index].face_vertices[0]];
    triangle[1] = vertices[faces[face_index].face_vertices[1]];
    triangle[2] = vertices[faces[face_index].face_vertices[2]];

    vec3 edge_1 = triangle[1] - triangle[0];
    vec3 edge_2 = triangle[2] - triangle[0];

    vec3 normal_0 = vertex_normals[faces[face_index].face_vertices[0]];
    vec3 normal_1 = vertex_normals[faces[face_index].face_vertices[1]];
    vec3 normal_2 = vertex_normals[faces[face_index].face_vertices[2]];

    vec3 p_vector = cross(render_ray.direction(), edge_2);
    double determinant = dot(edge_1, p_vector);

    double inverse_determinant = 1.0 / determinant;
    vec3 ray_to_vertex0 = render_ray.origin() - triangle[0];
    double u = dot(ray_to_vertex0, p_vector) * inverse_determinant;

    vec3 q = cross(ray_to_vertex0, edge_1);
    double v = dot(render_ray.direction(), q) * inverse_determinant;
    double w = 1.0 - u - v;
   
	vec3 normal_vector;
	if (smooth_shading) {
		normal_vector = (normal_0 * w) + (normal_1 * u) + (normal_2 * v);
	} else {
		normal_vector = cross(edge_1, edge_2); // non smooth shading	
	}
	return normalize(normal_vector);
}


RayHit Mesh::hit(const ray& render_ray) {
    RayHit local_ray_hit;
    local_ray_hit.hit_time = -1; // no hit
    local_ray_hit.face_id = -1;
    double best_time = std::numeric_limits<double>::max();
    int face_index = 0;

	// add bounding box hierarchy!!!
    for (const auto& current_face : faces) {
        point3 triangle[3];
        triangle[0] = vertices[current_face.face_vertices[0]];
        triangle[1] = vertices[current_face.face_vertices[1]];
        triangle[2] = vertices[current_face.face_vertices[2]];

        double hit_time = get_ray_mesh_intersection(render_ray, triangle);

        if (hit_time > 0.0 && hit_time < best_time) {
            best_time = hit_time;
            local_ray_hit.hit_time = hit_time;
            local_ray_hit.face_id = face_index;
			local_ray_hit.hit_object = this; // add pointer to object
        }
        face_index++;
    }
    return local_ray_hit;
}

color Mesh::ligma() {
	color obama = material_pointer->get_color();
	//color obama = color(1,0,0);
	return obama;
}


