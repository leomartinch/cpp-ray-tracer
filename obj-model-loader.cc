#include <fstream>
#include <sstream> // stringstream
#include <string>
#include <algorithm>
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

void read_obj_file(const string& filename) {
    ifstream obj(filename);

    if (!obj.is_open()) { // check if file can be opened
        cerr << "Error opening the file!";
        return;
    }

    string line;
	
    while (getline(obj, line)) { // iterate over every line in the file
								 //
		if (line.empty() || line[0] == '#') continue;

		istringstream stream(line);

        string prefix;
        stream >> prefix;
		//cout << "Stream: " << stream << "\n";

		if (prefix == "v") { // vertex
            float x, y, z;
            stream >> x >> y >> z;
			cout << "Vertex: " << x << y << z << "\n";
		}

		else if (prefix == "f") { // face
		    string token;
		    while (stream >> token) { // dont understand yet exactly how this works
				replace(token.begin(), token.end(), '/', ' ');
				istringstream point_stream(token);

				int x, y, z;
				point_stream >> x >> y >> z;
				cout << "Face Indexes: " << x << y << z << "\n";
			}
		}



        //cout << line << endl;
	}

    // Close the file
    obj.close();
}

string path_to_file = "cube.obj";



int main() {
    read_obj_file(path_to_file);
	return 0;
}
				



