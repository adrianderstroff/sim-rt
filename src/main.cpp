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
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/normal.h"
#include "math/constants.h"
#include "texture/constanttexture.h"
#include "texture/imagetexture.h"
#include "scene/camera.h"

using rt::vec3, rt::App, rt::Camera;
using rt::HitableList, rt::Sphere, rt::Rectangle, rt::BVH;
using rt::NormalMaterial, rt::Lambertian, rt::Metal, rt::Dielectric;
using rt::ConstantTexture, rt::ImageTexture, rt::Image;

int main() {
	//App app(160, 120, 20, 10);
	App app(320, 240, 1000, 100);
	//rt::App app(1280, 960, 50, 10);
	app.setBackgroundColor(rt::vec3(1));

	// textures
	Image image;
	image.read("./image/test.jpg");
	std::shared_ptr<ImageTexture> tex = std::make_shared<ImageTexture>(image);
	auto white = std::make_shared<ConstantTexture>(vec3(0.2, 0.9, 0.3));

	// create world
    std::shared_ptr<HitableList> world = std::make_shared<HitableList>(HitableList());
	//world->add(std::make_shared<Sphere>(vec3(0, 0, 0), 0.5, std::make_shared<Lambertian>(tex)));
	world->add(std::make_shared<Sphere>(vec3(0, 0, 0), 0.5, std::make_shared<Dielectric>(1.6, tex)));
	world->add(std::make_shared<Sphere>(vec3(0, -1000.5, 0), 1000, std::make_shared<Lambertian>(white)));
	app.setHitable(world);

	// setup camera
    auto cam = std::make_shared<Camera>(vec3(0), vec3(0), vec3(0,1,0), 45, app.aspect());
	app.setCamera(cam);

	int N = 36;
	float da = 360.f / N;
	float r = 1.5;
	for (int i = 0; i < N; ++i) {
		// update camera position
		float angle = da * i * rt::constants::DEG_TO_RAD + rt::constants::HALF_PI;
		float x = std::cos(angle) * r;
		float z = -std::sin(angle) * r;
		cam->setPosition(vec3(x, 0, z));

		// perform ray tracing
		app.run();
		app.write("./image/myimage"+std::to_string(i)+".png");
	}

    return 0;
}