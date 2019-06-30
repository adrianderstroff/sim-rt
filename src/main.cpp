#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "app/app.h"
#include "hitable/sphere.h"
#include "hitable/mesh.h"
#include "hitable/hitablelist.h"
#include "hitable/rectangle.h"
#include "io/image.h"
#include "scene/camera.h"
#include "material/dielectric.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/normal.h"
#include "texture/constanttexture.h"
#include "texture/imagetexture.h"

using rt::vec3, rt::App, rt::Camera;
using rt::HitableList, rt::Sphere, rt::Rectangle;
using rt::NormalMaterial, rt::Lambertian, rt::Metal, rt::Dielectric;
using rt::ConstantTexture, rt::ImageTexture, rt::Image;

int main() {
	App app(160, 120, 20, 10);
	//rt::App app(1280, 960, 50, 10);
	app.setBackgroundColor(rt::vec3(0.1f, 0.2f, 0.5f));

	// textures
	Image image;
	image.read("./myimage2.png");
	std::shared_ptr<ImageTexture> tex = std::make_shared<ImageTexture>(image);

	// create world
    std::shared_ptr<HitableList> world = std::make_shared<HitableList>(HitableList());
	world->add(std::make_shared<Sphere>(vec3(1, 0, 0), 0.5, std::make_shared<Lambertian>(tex)));
	world->add(std::make_shared<Sphere>(vec3(0, -1000.5, 0), 1000, std::make_shared<NormalMaterial>()));
	app.setHitable(world);

	// setup camera
    Camera cam(vec3(0,0,-3), vec3(0,0,0), vec3(0,1,0), 45, app.aspect());
	app.setCamera(cam);

	// perform ray tracing
	app.run();
	app.write("myimage.png");

    return 0;
}