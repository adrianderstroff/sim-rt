#include <iostream>
#include <string>
#include <chrono>
#include <math.h>

#include "app/app.h"
#include "hitable/sphere.h"
#include "hitable/mesh.h"
#include "hitable/hitablelist.h"
#include "scene/camera.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/dielectric.h"

int main() {
	//App app(1280, 960, 20);
	App app(320, 240, 1, 1);

	// read obj
	//std::shared_ptr<Mesh> mesh = loadmesh("bunny.obj", std::make_shared<Dielectric>(Dielectric(1.5)));
	std::shared_ptr<Mesh> mesh = loadmesh("bunny.obj", std::make_shared<Lambertian>(Lambertian(vec3(0.1, 0.2, 0.5))));
	mesh->normalize();

	// create world
    std::shared_ptr<HitableList> world = std::make_shared<HitableList>(HitableList());
	world->add(std::make_shared<Sphere>(Sphere(vec3(0, -1000.5, 0), 1000, std::make_shared<Lambertian>(Lambertian(vec3(1, 1, 1))))));
	/*
	world->add(std::make_shared<Sphere>(Sphere(vec3(0, 0, 0), 0.5, std::make_shared<Lambertian>(Lambertian(vec3(0.1, 0.2, 0.5))))));
	world->add(std::make_shared<Sphere>(Sphere(vec3(-1, 0, 0), 0.5, std::make_shared<Metal>(Metal(vec3(0.2, 0.2, 0.2))))));
	world->add(std::make_shared<Sphere>(Sphere(vec3(1, 0, 0), 0.5, std::make_shared<Dielectric>(Dielectric(1.5)))));
	*/
	world->add(mesh);
	app.setHitable(world);

	// setup camera
    Camera cam(vec3(0,0,-3), vec3(0,0,0), vec3(0,1,0), 45, app.aspect());
	app.setCamera(cam);

	// perform ray tracing
	app.run();
	app.write("myimage.png");

    return 0;
}