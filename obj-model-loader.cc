#include <fstream>
#include <sstream> // stringstream
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
using namespace std;

// OBJ MODEL-LOADER //
// Leo Martin (2025) //


// .obj file format //
/*
# = comment
v = vertice
f = face
usemtl = material name
s = smooth (1 = on, 0 = off)
o = object name
*/


struct Face {
    int face_vertices[3];
};

std::vector<Face> mesh_faces; // list for all faces of mesh

void read_obj_file(const string& filename) {
    ifstream obj(filename);

    if (!obj.is_open()) { // check if file can be opened
        cerr << "Error opening the file!" << "\n";
        return;
    }

    string line;
	
    while (getline(obj, line)) { // iterate over every line in the file
		if (line.empty() || line[0] == '#') continue;

		istringstream stream(line);

        string prefix;
        stream >> prefix;


		if (prefix == "o") { // object name
		    string object_name;
			stream >> object_name;
			cout << "Object Name: " << object_name << "\n";
		}

		else if (prefix == "v") { // vertex
            float x, y, z;
            stream >> x >> y >> z;
			cout << "Vertex: " << x << y << z << "\n";
		}

		else if (prefix == "s") { // smooth shading
		    bool smooth_shading;
			stream >> smooth_shading;
			cout << "Smooth Shading: " << smooth_shading << "\n";
		}

		else if (prefix == "usemtl") { // material name
		    string material_name;
			stream >> material_name;
			cout << "Material Name: " << material_name << "\n";
		}

		else if (prefix == "f") { // face
		    Face current_face; // initiate current struct
		    string token;
			int i = 0;
		    while (stream >> token) { // have to make a system that works with quads and triangulates the face
				// break down face and only take the index (vertice/vertex_texture/vertex_normal)
				int vertex_index;
				
				replace(token.begin(), token.end(), '/', ' ');
				istringstream point_stream(token);

				point_stream >> vertex_index;
				current_face.face_vertices[i] = vertex_index - 1; // -1 because the indexes start at 1
				
				i++;
			}
		    cout << "Face Indexes: " << current_face.face_vertices[0] << current_face.face_vertices[1] << current_face.face_vertices[2] << "\n";
 
		}
	}

    obj.close();
}

string path_to_file = "cube.obj";



int main() {
    read_obj_file(path_to_file);
	return 0;
}
				



