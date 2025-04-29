# C++ Ray-Tracer

This is a port of [python-ray-tracer](https://github.com/leomartinch/python-ray-tracer) to C++ to learn the language. 

## Features
- Path tracing
- OBJ loader (with materials)
- Smooth shading



## Installation

### Linux
```sh
git clone https://github.com/leomartinch/cpp-ray-tracer.git
cd cpp-ray-tracer/
mkdir build/
cd build/
cmake ..
```

### Windows (PowerShell)

```sh
git clone https://github.com/leomartinch/cpp-ray-tracer.git
cd your-project
mkdir build
cd build
cmake ..
cmake --build . --config Release
```


## Usage

To setup a scene go to the src/main.cc and set the scene (see customisation). 
Also set the render resolution and other settings.
Then compile the script:
```sh
cd build/
make
```

To run the ray tracer you have to be in the home directory.
Then run the program into a PPM file of your choosing.
```sh
./build/leo-raytracer > filename.ppm
```




> [!IMPORTANT]
> You must triangulate the mesh before exporting it to obj, as my ray tracer only supports triangles.

## Contribute

This project was created because I wanted to learn C++. There are several things I'd like to improve (see Issues tab).

If you have any suggestions on how I can improve the code I'd greatly appreciate it.

More information can be found on my website: [leomartin.ch](https://leomartin.ch/)
