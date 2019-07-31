#ifndef SCENE_IO_H
#define SCENE_IO_H

#include <memory>
#include <string>
#include <fstream>
#include <streambuf>

#include "peglib.h"
#include "math/vec3.h"

namespace rt {
	struct SceneData {

	};

	struct TracerObj { 
		std::string type; 
		int width, height, samples, depth; 
	};
	struct CameraObj { 
		std::string type; 
		vec3 pos, lookat, up; 
		double fov; 
	};
	struct MaterialObj {
		std::string type;
		std::string name;
		vec3        color;
		std::map<std::string, std::string> options;
	};
	struct ObjectObj {
		std::string type;
		std::string name;
		vec3        pos;
		std::string material;
		std::map<std::string, std::string> options;
	};
	struct SceneElement {
		std::string name;
	};
	struct SceneObj {
		std::string type;
		std::map<std::string, SceneElement> elements;
	};

	std::shared_ptr<SceneData> read_scene(std::string scenepath);
}

#endif//SCENE_IO_H