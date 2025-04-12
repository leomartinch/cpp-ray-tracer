#include "leo-raytracer.h"

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

	const int samples = 10;
	const int max_bounces = 2;

    const float pixel_size = -2 * start_x / image_width;

	const float center_pixel = pixel_size / 2;
	const int height = image_height / pixel_size;
    const int width = image_width / pixel_size;

	// initiate scene (populate with meshes)
	MeshScene scene;

    scene.add(make_shared<Mesh>("objects/ball3.obj"));
    scene.add(make_shared<Mesh>("objects/ball4.obj"));
    scene.add(make_shared<Mesh>("objects/lightball.obj"));
	
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

			color path_color = scene.trace_path(render_ray, samples, max_bounces);
		    write_color(cout, path_color);
		}

    }
	clog << "\rRender Done                            \n";
}
				



