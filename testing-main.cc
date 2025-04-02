#include "leoraytracer.h"
#include "hittable.h"
#include "hittable_list.h"

#include <fstream>
#include <sstream> // stringstream
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

// RAY-TRACER //
// Leo Martin (2025) //


// import mesh //
// will be ported to the hittable class //

struct Face {
    int face_vertices[3];
};

vector<point3> mesh_vertices; // list for all vertices
vector<Face> mesh_faces; // list for all faces of mesh

void read_obj_file(const string& filename) {
    ifstream obj(filename);

    if (!obj.is_open()) { // check if file can be opened
        cerr << "Error opening the file!" << "\n";
        return;
    }

    string line;	
    while (getline(obj, line)) { // iterate over every line in the file
		if (line.empty() || line[0] == '#') continue; // ignore comments

		istringstream stream(line);
        string prefix;
        stream >> prefix;

		if (prefix == "o") { // object name
		    string object_name;
			stream >> object_name;
		}

		else if (prefix == "v") { // vertex
            float x, y, z;
            stream >> x >> y >> z;
			mesh_vertices.push_back(point3(x,y,z));
		}

		else if (prefix == "s") { // smooth shading
		    bool smooth_shading;
			stream >> smooth_shading;
		}

		else if (prefix == "usemtl") { // material name
		    string material_name;
			stream >> material_name;
		}

		else if (prefix == "f") { // face
		    Face current_face; // initiate current struct
		    string token;
			int i = 0;
		    while (stream >> token) { // have to make a system that works with quads and triangulates the face
				replace(token.begin(), token.end(), '/', ' ');
				istringstream point_stream(token);
				int vertex_index;
				point_stream >> vertex_index;
				current_face.face_vertices[i] = vertex_index - 1; // (-1) because the indexes start at 1	
				i++;
			}
		    mesh_faces.push_back(current_face);
		}	

	}
    obj.close();
}



//########################################################################//
// ray tracer part //

bool get_ray_mesh_intersection(ray render_ray, point3 vertices[3]) {
    vec3 edge_1 = vertices[1] - vertices[0];
    vec3 edge_2 = vertices[2] - vertices[0];

    vec3 p_vector = cross(render_ray.direction(), edge_2);
    double determinant = dot(edge_1, p_vector);

    const float kepsilon = 1e-6f;
    if (fabs(determinant) < kepsilon) {
        return false; // ray is parallel or invalid
    }

    float inverse_determinant = 1 / determinant;
    vec3 ray_to_vertice_0 = render_ray.origin() - vertices[0];

    double u = dot(ray_to_vertice_0, p_vector) * inverse_determinant;
    if (u < 0 || u > 1) { // ...explanation
        return false;
    }

    vec3 q = cross(ray_to_vertice_0, edge_1);
    double v = dot(render_ray.direction(), q) * inverse_determinant;
    if (v < 0 || (u + v) > 1) { // ...explanation
        return false;
	}

    double t = dot(edge_2, q) * inverse_determinant;
    if (t > 0){
        //intersection_point = add(ray_origin, multiply_scalar(ray_direction, t))
        return true;
    }

    return false; // ray intersects triangle behind camera
}



// render image
int main() {
	const int image_width = 256;
	const int image_height = 256;
	
	const float start_x = -2;
	const float start_y = 1.5;

    const float pixel_size = -2 * start_x / image_width;

	const float center_pixel = pixel_size / 2;
	const int height = image_height / pixel_size;
    const int width = image_width / pixel_size;

	// initiate object 
    string path_to_file = "objects/monke.obj"; 
    read_obj_file(path_to_file);
	
	// render	
	cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; // PPM header 
    color pixel_color;
    float x_pos, y_pos;

	for (int j = 0; j < image_height; j++) { // row
		clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush; // progress meter
		y_pos = start_y - (pixel_size * j) - center_pixel;
		for (int i = 0; i < image_width; i++) { // column
		    x_pos = start_x + (pixel_size * i) + center_pixel;
			
			vec3 cell_center(x_pos,y_pos,6); // cell start has to not clip through the object
			vec3 ray_direction(0, 0, -1);

		    ray_direction = normalize(ray_direction);
            ray render_ray(cell_center, ray_direction);

		    pixel_color = color(0,0,0);
			for (const auto& current_face : mesh_faces) {
				point3 current_vertices[3];
				current_vertices[0] = mesh_vertices[current_face.face_vertices[0]];
				current_vertices[1] = mesh_vertices[current_face.face_vertices[1]];
				current_vertices[2] = mesh_vertices[current_face.face_vertices[2]];

				bool hit = get_ray_mesh_intersection(render_ray, current_vertices);
		    
			    if (hit) {
				    pixel_color = color(1, 0, 0);
					break;
			    }
			}
            write_color(std::cout, pixel_color);
		}
    }
	
	clog << "\rRender Done                            \n";
}
				



