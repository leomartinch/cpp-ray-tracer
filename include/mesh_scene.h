#include <vector>
#include <memory>
#include "mesh.h"
#include "ray.h"

class MeshScene {
public:
    // add mesh to the scene
    void add(const std::shared_ptr<Mesh>& mesh) {
        meshes.push_back(mesh);
    }

    // cast a ray and return the closest hit among all meshes in the scene
    RayHit hit(const ray& render_ray) const {
        RayHit closest_hit;
        closest_hit.hit_time = -1;  // no hit
		closest_hit.face_id = -1;
        double closest_time = std::numeric_limits<double>::max();

        for (const auto& mesh : meshes) {
		//for (const auto& bounding_box : bounding_boxes) {

		    // add bounding box hierarchy!!!
			//BoundHit bound_hit = mesh->bound_hit(render_ray);
			//if (bound_hit.is_hit) {
			//	continue;
			//}

            RayHit temp_hit = mesh->hit(render_ray);
            if (temp_hit.hit_time > 0 && temp_hit.hit_time < closest_time) {
               closest_time = temp_hit.hit_time;
		       temp_hit.hit_object = mesh.get(); // pointer to the object that was hit
               closest_hit = temp_hit;
            }
        }
        return closest_hit;
    }


// have to clean up the names etc (error correction from chatgpt (only one line was wrong but he still changed many names)
color trace_path(ray render_ray, const int& samples, const int& max_bounces) const {
    color accumulated_color(0, 0, 0);

    // --- 1. Compute the primary hit once.
    RayHit primary_hit;
    primary_hit.hit_time = -1;  // no hit yet
    double closest_time = std::numeric_limits<double>::max();
    for (const auto& mesh : meshes) {
        // Early out if the primary ray misses the bounding box.
        if (!mesh->bound_hit(render_ray)) {
            continue;
        }
        RayHit temp_hit = mesh->hit(render_ray);
        if (temp_hit.hit_time > 0.001 && temp_hit.hit_time < closest_time) {
            closest_time = temp_hit.hit_time;
            temp_hit.hit_object = mesh.get(); // record which object was hit
            primary_hit = temp_hit;
        }
    }
    if (primary_hit.hit_time <= 0.001) {
        // No primary hit: return background (or zero)
        return accumulated_color;
    }

    Mesh* primary_mesh = dynamic_cast<Mesh*>(primary_hit.hit_object);
    point3 primary_hit_point = render_ray.at(primary_hit.hit_time);
    vec3 primary_normal = primary_mesh->get_normal_vector(primary_hit.face_id, render_ray);
    float primary_roughness = primary_mesh->get_roughness();
    color primary_emission = primary_mesh->get_emission();
    // Get the primary hit's diffuse color.
    color primary_diffuse = primary_mesh->get_color();

    // --- 2. For each sample, simulate the path starting at the primary hit.
    for (int i = 0; i < samples; i++) {
        color throughput(1, 1, 1);
        color sample_color(0, 0, 0);

        // At the primary hit, add its emission and update throughput with its diffuse color.
        sample_color += throughput * primary_emission;
        throughput = throughput * primary_diffuse;

        // Compute an initial outgoing ray direction from the primary hit.
        vec3 primary_specular = primary_mesh->get_specular_direction(render_ray, primary_normal);
        vec3 primary_diffuse_direction  = primary_mesh->get_diffuse_direction(primary_normal);
        vec3 reflection_direction = lerp(primary_diffuse_direction, primary_specular, primary_roughness);
        ray current_ray(primary_hit_point, reflection_direction);

        // --- Bounce loop: propagate the ray for further bounces.
        for (int j = 1; j < max_bounces; j++) {
            RayHit bounce_hit;
            bounce_hit.hit_time = -1;  // no hit
            double bounce_closest = std::numeric_limits<double>::max();

            // Find the closest hit in this bounce.
            for (const auto& mesh : meshes) {
                if (!mesh->bound_hit(current_ray)) {
                    continue;
                }
                RayHit temp_hit = mesh->hit(current_ray);
                if (temp_hit.hit_time > 0.001 && temp_hit.hit_time < bounce_closest) {
                    bounce_closest = temp_hit.hit_time;
                    temp_hit.hit_object = mesh.get();
                    bounce_hit = temp_hit;
                }
            }

            // If a bounce hit is found, update the ray and throughput.
            if (bounce_hit.hit_time > 0.001) {
                Mesh* bounce_mesh = dynamic_cast<Mesh*>(bounce_hit.hit_object);
                point3 bounce_hit_point = current_ray.at(bounce_hit.hit_time);
                vec3 bounce_normal = bounce_mesh->get_normal_vector(bounce_hit.face_id, current_ray);
                color bounce_emission = bounce_mesh->get_emission();
                color bounce_diffuse = bounce_mesh->get_color();
                float bounce_roughness = bounce_mesh->get_roughness();

                // Add the emission contribution at the bounce.
                sample_color += throughput * bounce_emission;

                // Propagate the effect of the bounce's surface color.
                throughput = throughput * bounce_diffuse;

                // Compute a new ray direction for this bounce.
                vec3 bounce_specular = bounce_mesh->get_specular_direction(current_ray, bounce_normal);
                vec3 bounce_diffuse_direction = bounce_mesh->get_diffuse_direction(bounce_normal);
                vec3 bounce_reflection_direction = lerp(bounce_diffuse_direction, bounce_specular, bounce_roughness);
                current_ray = ray(bounce_hit_point, bounce_reflection_direction);
            }
            else {
                // No further hit: break out.
                break;
            }
        }
        accumulated_color += sample_color;
    }
    return accumulated_color / samples;
}


private:
    std::vector<std::shared_ptr<Mesh>> meshes;
};

