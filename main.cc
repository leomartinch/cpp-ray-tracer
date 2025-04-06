#include "leo-raytracer.h"
#include "mesh_scene.h"
#include "mesh.h"
#include "ray.h"
#include "color.h"

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

// RAY-TRACER //
// Leo Martin (2025) //


// render image
int main() {
	const int image_width = 256;
	const int image_height = 256;
	
	const float start_x = -2;
	const float start_y = 2;

    const float pixel_size = -2 * start_x / image_width;

	const float center_pixel = pixel_size / 2;
	const int height = image_height / pixel_size;
    const int width = image_width / pixel_size;

	// initiate scene (populate with meshes)
	MeshScene scene;

	scene.add(make_shared<Mesh>("objects/icosphere.obj"));
    scene.add(make_shared<Mesh>("objects/cone.obj"));
    //scene.add(make_shared<Mesh>("objects/cone2.obj"));
	//scene.add(make_shared<Mesh>("objects/cone3.obj"));
	//scene.add(make_shared<Mesh>("objects/cone4.obj"));
	
	// render
	cout << "P3\n" << image_width << ' ' << image_height << "\n255\n"; // PPM header 
    color pixel_color;
    float x_pos, y_pos;

	for (int j = 0; j < image_height; j++) { // row
		clog << "\rScanlines remaining: " << (image_height - j) << ' ' << flush; // progress meter
		y_pos = start_y - (pixel_size * j) - center_pixel;
		for (int i = 0; i < image_width; i++) { // column
		    x_pos = start_x + (pixel_size * i) + center_pixel;
			
			vec3 cell_center(x_pos,y_pos,6); // cell start has to not clip through the object
			vec3 ray_direction(0, 0, -1);

		    ray_direction = normalize(ray_direction);
            ray render_ray(cell_center, ray_direction);

		    pixel_color = color(0,0,0);
			RayHit hit = scene.hit(render_ray);
	
			if (hit.hit_time > 0.0) {	// if there is an intersection with a triangle
				point3 intersection_point = render_ray.at(hit.hit_time);
				vec3 normal_vec = dynamic_cast<Mesh*>(hit.hit_object)->get_normal_vector(hit.face_id, render_ray);

				vec3 mapped_normal = 0.5 * (normal_vec + vec3(1,1,1)); // for rgb coloring
				pixel_color = color(mapped_normal[0], mapped_normal[1], mapped_normal[2]);	
		    }
		    write_color(cout, pixel_color);
		}

    }
	clog << "\rRender Done                            \n";
}
				



