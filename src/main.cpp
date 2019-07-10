#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "tracer/raytracer.h"
#include "hitable/hitable.h"
#include "io/image.h"
#include "material/material.h"
#include "math/constants.h"
#include "texture/texture.h"
#include "scene/camera.h"

using namespace rt;

int main() {
	// create ray tracer
	Raytracer raytracer(Resolution::MEDIUM, Samples::HIGH, TraceDepth::LOW);
	raytracer.setBackgroundColor(vec3(0));

	// create textures
	Image right; right.read("./image/cubemap/pos-x.png");
	Image left; left.read("./image/cubemap/neg-x.png");
	Image top; top.read("./image/cubemap/pos-y.png");
	Image bottom; bottom.read("./image/cubemap/neg-y.png");
	Image back; back.read("./image/cubemap/pos-z.png");
	Image front; front.read("./image/cubemap/neg-z.png");
	auto cubemap = std::make_shared<CubeMap>(right, left, top, bottom, back, front);

	// create materials
	auto light = std::make_shared<DiffuseLight>(cubemap);
	auto metal = std::make_shared<Metal>(create_color(0.8));
	auto whiteglass = std::make_shared<Dielectric>(1.4);
	auto redglass = std::make_shared<Dielectric>(1.4, create_color(vec3(1, 0.3, 0.3)));

	// create world
    std::shared_ptr<BVH> world = std::make_shared<BVH>(1, 10);
	world->insert(std::make_shared<Sphere>(vec3(-2, 0, -2), 1.f, whiteglass));
	world->insert(std::make_shared<Sphere>(vec3( 2, 0, -2), 1.f, redglass));
	world->insert(std::make_shared<Sphere>(vec3(0, 0, -4), 1.f, metal));
	world->insert(std::make_shared<Cube>(vec3(0, 0, 0), 10, 10, 10, light, true));
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