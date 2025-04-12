#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
//#include "vec3.h"
//using color = vec3;


class Material {
public:
    //Material(const color& a) : albedo(a) {}
	//Material(const color& diffuse_color) : diffuse(diffuse_color) {
		// constructor	
	Material(const std::string& filename, const std::string& material_name) {	

		//color Mesh::load_mtl(const string& filename) {
		std::ifstream mtl(filename);
		if (!mtl.is_open()) {
		    std::cerr << "Failed to load material from: " << filename << "\n";
		    //return false;
			// make a default (for example link to a default white color)
		}
		
		std::string line;
		while (std::getline(mtl, line)) {
            if (line.empty() || line[0] == '#')
				continue;

			std::istringstream stream(line);
			std::string prefix;
			stream >> prefix;

			// important!!! //
			// make sure to only load one material based on the material name, sometimes there are multiple materials in a file //
			// for now have presumtion that file only has one material

            if (prefix == "newmtl") { // material name
				std::string local_material_name;
				stream >> local_material_name;
				if (local_material_name == material_name) {
				    this->material_name = local_material_name;
				}
            }
		    else if (prefix == "Ns") { // shininess
                stream >> this->shininess;
            }
            else if (prefix == "Ka") { // ambient color
		        float r, g, b;
				stream >> r >> g >> b;
				this->ambient = color(r,g,b);
		    }
		    else if (prefix == "Kd") { // diffuse color
				float r, g, b;
				stream >> r >> g >> b;
				this->diffuse = color(r,g,b);
            }
		    else if (prefix == "Ks") { // specular color
				float r, g, b;
				stream >> r >> g >> b;
				this->specular = color(r,g,b);
            }
		    else if (prefix == "Ke") { // emission color
				float r, g, b;
				stream >> r >> g >> b;
				this->emission = color(r,g,b);
		    }
        }
		mtl.close();
    }

	color get_color() const {
		return diffuse;
    }

private:
    std::string material_name;
	float shininess;
	color ambient;
	color diffuse;
	color specular;
	color emission;
};

#endif

