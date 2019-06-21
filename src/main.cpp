#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "app/app.h"
#include "hitable/sphere.h"
#include "hitable/hitablelist.h"
#include "scene/camera.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/dielectric.h"

int main() {
	App app(640, 480, 10);

	// create world
    std::shared_ptr<HitableList> world = std::make_shared<HitableList>(HitableList());
	world->add(std::make_shared<Sphere>(Sphere(vec3(0, -1000.5, 0), 1000, std::make_shared<Lambertian>(Lambertian(vec3(1, 1, 1))))));
    world->add(std::make_shared<Sphere>(Sphere(vec3(0, 0, 0), 0.5, std::make_shared<Lambertian>(Lambertian(vec3(0.1, 0.2, 0.5))))));
	world->add(std::make_shared<Sphere>(Sphere(vec3(-1, 0, 0), 0.5, std::make_shared<Metal>(Metal(vec3(0.2, 0.2, 0.2))))));
	world->add(std::make_shared<Sphere>(Sphere(vec3(1, 0, 0), 0.5, std::make_shared<Dielectric>(Dielectric(1.5)))));
	app.setHitable(world);

	// setup camera
	vec3 pos(0, 0, -3);
	vec3 lookAt(0, 0, 0);
	vec3 up(0, 1, 0);
    Camera cam(pos, lookAt, up, 45, app.aspect());
	app.setCamera(cam);

	// perform ray tracing
	app.run();
	app.write(IMG_DIR"/myimage.png");

    return 0;
}