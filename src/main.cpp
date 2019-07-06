#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "app/app.h"
#include "hitable/bvh.h"
#include "hitable/sphere.h"
#include "hitable/mesh.h"
#include "hitable/hitablelist.h"
#include "hitable/rectangle.h"
#include "io/image.h"
#include "material/dielectric.h"
#include "material/diffuselight.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/normal.h"
#include "math/constants.h"
#include "texture/constanttexture.h"
#include "texture/imagetexture.h"
#include "scene/camera.h"

using namespace rt;

int main() {
	// create ray tracer
	rt::App app(Resolution::THUMBNAIL, Samples::HIGH, TraceDepth::LOW);
	app.setBackgroundColor(rt::vec3(0.0));

	// create textures
	Image image; image.read("./image/test.jpg");
	auto tex = std::make_shared<ImageTexture>(image);

	// create materials
	auto redlight   = std::make_shared<DiffuseLight>(create_color(vec3(4,0,0)));
	auto greenlight = std::make_shared<DiffuseLight>(create_color(vec3(0,4,0)));
	auto bluelight  = std::make_shared<DiffuseLight>(create_color(vec3(0,0,4)));
	auto glass      = std::make_shared<Dielectric>(1.3);
	auto white      = std::make_shared<Lambertian>(create_color(vec3(1)));

	// create world
    std::shared_ptr<BVH> world = std::make_shared<BVH>(1, 10);
	// bottom
	world->insert(std::make_shared<Rectangle>(vec3(0,-2, 0), vec3(-10, 0, 0), vec3(0, 0, 10), white));
	// object
	world->insert(loadmesh("./bunny.obj", glass, true, true));
	// sphere lights
	world->insert(std::make_shared<Sphere>(vec3(-5, 5, 4), 0.5, redlight));
	world->insert(std::make_shared<Sphere>(vec3(0, 5, -1), 0.5, greenlight));
	world->insert(std::make_shared<Sphere>(vec3(5, 5, 4), 0.5, bluelight));
	world->build();
	app.setHitable(world);

	// create camera
    auto cam = std::make_shared<Camera>(vec3(0,0,-2), vec3(0), vec3(0,1,0), 45, app.aspect());
	app.setCamera(cam);

	// perform ray tracing
	app.run();
	app.write("./image/myimage.png");

    return 0;
}