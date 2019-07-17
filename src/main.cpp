#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "tracer/tracer.h"
#include "hitable/hitable.h"
#include "io/image.h"
#include "material/material.h"
#include "math/constants.h"
#include "texture/texture.h"
#include "scene/camera.h"

using namespace rt;

int main() {
	// create ray tracer
	Debugtracer raytracer(Resolution::LOW, Samples::ONE, TraceDepth::MEDIUM);
	raytracer.setBackgroundColor(vec3(0));

	// create materials
	auto light = std::make_shared<DiffuseLight>(create_color(vec3(4)));
	auto blue = std::make_shared<Lambertian>(create_color(vec3(0, 0, 1)));
	auto green = std::make_shared<Lambertian>(create_color(vec3(0, 1, 0)));
	auto red = std::make_shared<Lambertian>(create_color(vec3(1, 0, 0)));
	auto glass = std::make_shared<Dielectric>(1.4, create_color(vec3(1)));
	auto normal = std::make_shared<NormalMaterial>();
	
	// create shared objects
	auto bunny = loadmesh("./bunny.obj", glass, true, true);
	auto plane = std::make_shared<Rectangle>(vec3(0,-0.5,0), vec3(3,0,0), vec3(0,0,-3), normal);

	// create world
    std::shared_ptr<BVH> world = std::make_shared<BVH>(1, 10);
	world->insert(plane);
	world->insert(bunny);
	world->build();
	raytracer.setHitable(world);

	// create camera
    auto cam = std::make_shared<Camera>(vec3(0,0,3), vec3(0), vec3(0,1,0), 45, raytracer.aspect());
	raytracer.setCamera(cam);

	// perform ray tracing
	raytracer.run();
	raytracer.write("./image/myimage.png");

    return 0;
}