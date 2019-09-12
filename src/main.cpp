#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "peglib.h"

#include "tracer/tracer.h"
#include "hitable/hitable.h"
#include "io/image.h"
#include "io/sceneio.h"
#include "material/material.h"
#include "math/constants.h"
#include "texture/texture.h"
#include "scene/camera.h"

using namespace rt;

void env_images() {
	// setup raycaster
	Raytracer raytracer(Resolution::HIGH, Samples::HIGH, TraceDepth::MEDIUM);
	raytracer.setBackgroundColor(vec3(0));

	// load images
	auto [tex1, status1] = read_image("./image/abstract.png");
	auto [tex2, status2] = read_image("./image/blonde.png");
	auto [tex3, status3] = read_image("./image/brownandyellow.png");
	auto [tex4, status4] = read_image("./image/floatingonwaves.png");
	auto [tex5, status5] = read_image("./image/profile.jpg");
	auto [tex6, status6] = read_image("./image/winterdepression.png");

	if (!(status1 && status2 && status3 && status4 && status5 && status6)) {
		console::println("Something went wrong while loading the images");
		exit(1);
	}

	// create textures
	auto cubemap = std::make_shared<CubeMap>(tex1, tex2, tex3, tex4, tex5, tex6);

	// create materials
	auto lightcubemap = std::make_shared<DiffuseLight>(cubemap);
	auto glass = std::make_shared<Dielectric>(4.3, new_color(1));

	// create objects
	auto cube = std::make_shared<Cube>(vec3(0), 10, 10, 10, lightcubemap, true);
	auto sphere = std::make_shared<Sphere>(vec3(0), 1, glass);

	// setup scene
	auto world = std::make_shared<BVH>();
	world->insert(cube);
	world->insert(sphere);
	world->build();
	raytracer.setHitable(world);

	// create camera
	auto cam = std::make_shared<DOFCamera>(vec3(2, -1, 3.5), vec3(0), vec3(0, 1, 0), 45.0, raytracer.aspect(), 0.1);
	raytracer.setCamera(cam);

	// run
	raytracer.run();
	raytracer.write("./image/myimage.png");
}

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

	// run tracer
	scene->tracer->setBackgroundColor(vec3(0, 0, 0));
	scene->tracer->run();
	scene->tracer->write(imagepath);
	console::println("Saved result at " + imagepath);

    return 0;
}