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
	//App app(160, 120, 1000, 100);
	//App app(320, 240, 1000, 100);
	rt::App app(1280, 960, 1000, 100);
	app.setBackgroundColor(rt::vec3(0));

	// create textures
	Image image; image.read("./image/test.jpg");
	auto tex = std::make_shared<ImageTexture>(image);

	// create materials
	auto light = std::make_shared<DiffuseLight>(create_color(vec3(4)));
	auto glass = std::make_shared<Dielectric>(1.2);
	auto metal = std::make_shared<Metal>(create_color(vec3(0.6)));
	auto texmaterial = std::make_shared<Lambertian>(tex);
	auto white = std::make_shared<Lambertian>(create_color(vec3(1)));
	auto red = std::make_shared<Lambertian>(create_color(vec3(1,0,0)));
	auto green = std::make_shared<Lambertian>(create_color(vec3(0,1,0)));
	auto blue = std::make_shared<Lambertian>(create_color(vec3(0,0,1)));

	// create world
    std::shared_ptr<BVH> world = std::make_shared<BVH>(1, 10);
	world->insert(std::make_shared<Rectangle>(vec3(0, 0, 2), vec3(-2,0,0), vec3(0,2,0), white));             // back
	world->insert(std::make_shared<Rectangle>(vec3(0,-2, 0), vec3(-2, 0, 0), vec3(0, 0, 2), white));         // bottom
	world->insert(std::make_shared<Rectangle>(vec3(0, 2, 0), vec3(-2, 0, 0), vec3(0, 0, -2), white));        // top
	world->insert(std::make_shared<Rectangle>(vec3(2, 0, 0), vec3(0, 0, 2), vec3(0, 2, 0), green));          // left
	world->insert(std::make_shared<Rectangle>(vec3(-2, 0, 0), vec3(0, 0, -2), vec3(0, 2, 0), red));          // right
	world->insert(std::make_shared<Rectangle>(vec3(0, 1.99, 0), vec3(-0.5, 0, 0), vec3(0, 0, -0.5), light)); // light
	// spheres
	world->insert(std::make_shared<Sphere>(vec3(0, 0.5, 0), 0.5, texmaterial));
	world->insert(std::make_shared<Sphere>(vec3(1, -0.5, 0), 0.5, glass));
	world->insert(std::make_shared<Sphere>(vec3(-1, -0.5, 0), 0.5, metal));
	world->build();
	app.setHitable(world);

	// create camera
    auto cam = std::make_shared<Camera>(vec3(0,0,-5.5), vec3(0), vec3(0,1,0), 45, app.aspect());
	app.setCamera(cam);


	// perform ray tracing
	app.run();
	app.write("./image/myimage.png");

    return 0;
}