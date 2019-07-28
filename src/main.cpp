#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "peglib.h"

#include "tracer/tracer.h"
#include "hitable/hitable.h"
#include "io/image.h"
#include "material/material.h"
#include "math/constants.h"
#include "texture/texture.h"
#include "scene/camera.h"

using namespace rt;

void manual_scene() {
	// create ray tracer
	//Raycaster raytracer(320, 240, 10, 10);
	Raycaster raytracer(Resolution::MEDIUM, Samples::MEDIUM, TraceDepth::LOW);
	raytracer.setBackgroundColor(vec3(0));
	//raytracer.setDebugMode(DebugMode::POINTS);

	// create materials
	auto light = std::make_shared<DiffuseLight>(new_color(vec3(4)));
	auto blue = std::make_shared<Lambertian>(new_color(vec3(0, 0, 1)));
	auto green = std::make_shared<Lambertian>(new_color(vec3(0, 1, 0)));
	auto red = std::make_shared<Lambertian>(new_color(vec3(1, 0, 0)));
	auto glass = std::make_shared<Dielectric>(1.2, new_color(vec3(1)));
	auto normal = std::make_shared<NormalMaterial>();

	// create objects
	auto bunny = load_mesh("./bunny.obj", normal, true, true, true);
	auto lucy   = load_mesh("./lucy-low-poly.obj", normal, true, true, true);
	//auto buddha = load_mesh("./buddha.obj", normal, true, true, true);
	auto dragon = load_mesh("./dragon-low-poly.obj", normal, true, true, true);
	auto plane = std::make_shared<Rectangle>(vec3(0, -0.5, 0), vec3(10, 0, 0), vec3(0, 0, -10), normal);
	auto plane2 = std::make_shared<Rectangle>(vec3(0, 0, -5), vec3(10, 0, 0), vec3(0, 10, 0), normal);
	auto sphere = std::make_shared<Sphere>(vec3(0, 5, 2), 1.0, light);

	// transformed objects
	auto b1 = std::make_shared<Translation>(std::make_shared<Rotation>(bunny, vec3(0, 1, 0), 45.0), vec3(-1, 0, 0));
	auto b2 = std::make_shared<Translation>(std::make_shared<Rotation>(lucy, vec3(0, 1, 0), 90.0), vec3(0, 0, 1));
	auto b3 = std::make_shared<Translation>(std::make_shared<Rotation>(dragon, vec3(0, 1, 0), 200.0), vec3(1, 0, 0));

	// create world
	std::shared_ptr<BVH> world = std::make_shared<BVH>(1, 10);
	world->insert(plane);
	world->insert(plane2);
	world->insert(b1);
	world->insert(b2);
	world->insert(b3);
	world->build();

	raytracer.setHitable(world);

	// create camera
	auto cam = std::make_shared<Camera>(vec3(0, 0, 3.5), vec3(0), vec3(0, 1, 0), 45.0, raytracer.aspect());
	raytracer.setCamera(cam);

	// perform ray tracing
	raytracer.run();
	raytracer.write("./image/myimage.png");
}

int main() {
	manual_scene();

    return 0;
}