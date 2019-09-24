#include <string>

#include "io/sceneio.h"

using namespace rt;

int main(int argc, char* argv[]) {
	
	// list of required parameters
	std::string scenepath;
	std::string imagepath = "unnamed.png";

	// grab parameters from console input
	if (argc == 2) {
		scenepath = std::string(argv[1]);
	}
	else if (argc == 3) {
		scenepath = std::string(argv[1]);
		imagepath = std::string(argv[2]);
	}
	else {
		console::println("Invalid number of command line arguments!");
		console::println("Should be SCENE_PATH (OUTPUT_FILE_PATH)");
		exit(-1);
	}

	// derive tracer, camera and world from scene file
	auto scene = read_scene(scenepath);
	if (!scene->success) return 1;

	// run tracer
	scene->tracer->setBackgroundColor(vec3(0, 0, 0));
	scene->tracer->run();
	scene->tracer->write(imagepath);
	console::println("Saved result at " + imagepath);

    return 0;
}