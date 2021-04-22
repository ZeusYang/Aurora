#include <iostream>
#include "Tracer.h"
#include "Hitable.h"
#include "Material.h"
#include "Camera.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace Aurora;

Tracer tracer;

void cornellBoxScene()
{
	const AVector3f white_unit = AVector3f(0.73, 0.73, 0.73);
	const AVector3f green_unit = AVector3f(0.12, 0.45, 0.15);
	const AVector3f red_unit = AVector3f(0.65, 0.05, 0.05);
	const AVector3f light_unit = AVector3f(4.0, 4.0, 4.0);
	const AVector3f yellow_light_unit = AVector3f(1.0, 0.82, 0.53);

	Material::ptr whiteLambert_mat = std::make_shared<Lambertian>(white_unit);
	Material::ptr greenLambert_mat = std::make_shared<Lambertian>(green_unit);
	Material::ptr redLambert_mat = std::make_shared<Lambertian>(red_unit);
	Material::ptr lightDiffuse_mat = std::make_shared<DiffuseLight>(light_unit, green_unit);
	Material::ptr whiteMetal_mat = std::make_shared<Metal>(white_unit, 0.0f);
	Material::ptr bottle_mat = std::make_shared<Dielectric>(1.5f);
	Material::ptr board_mat = std::make_shared<Metal>(white_unit, 0.0f);

	Sphere::ptr sphere1(new Sphere(bottle_mat, AVector3f(+1.5, 1.5, +2), +1.5f));
	Sphere::ptr sphere2(new Sphere(whiteLambert_mat, AVector3f(-1.5, 2.5, +0), +2.5f));

	//Bottom wall
	Triangle::ptr bot1(new Triangle(whiteLambert_mat, AVector3f(-5, 0, -5), AVector3f(-5, 0, +5),
		AVector3f(+5, 0, -5)));
	Triangle::ptr bot2(new Triangle(whiteLambert_mat, AVector3f(+5, 0, -5), AVector3f(-5, 0, +5),
		AVector3f(+5, 0, +5)));
	
	//Top wall
	Triangle::ptr top1(new Triangle(whiteLambert_mat, AVector3f(-5, 10, -5), AVector3f(+5, 10, -5),
		AVector3f(-5, 10, +5)));
	Triangle::ptr top2(new Triangle(whiteLambert_mat, AVector3f(+5, 10, -5), AVector3f(+5, 10, +5),
		AVector3f(-5, 10, +5)));

	//Left wall
	Triangle::ptr lef1(new Triangle(greenLambert_mat, AVector3f(-5, 10, +5), AVector3f(-5, 0, +5),
		AVector3f(-5, 10, -5)));
	Triangle::ptr lef2(new Triangle(greenLambert_mat, AVector3f(-5, 10, -5), AVector3f(-5, 0, +5),
		AVector3f(-5, 0, -5)));

	//Right wall
	Triangle::ptr rig1(new Triangle(redLambert_mat, AVector3f(+5, 10, +5), AVector3f(+5, 10, -5),
		AVector3f(+5, 0, +5)));
	Triangle::ptr rig2(new Triangle(redLambert_mat, AVector3f(+5, 10, -5), AVector3f(+5, 0, -5),
		AVector3f(+5, 0, +5)));

	//Back wall
	Triangle::ptr bak1(new Triangle(whiteLambert_mat, AVector3f(-5, 10, -5), AVector3f(-5, 0, -5),
		AVector3f(+5, 10, -5)));
	Triangle::ptr bak2(new Triangle(whiteLambert_mat, AVector3f(+5, 10, -5), AVector3f(-5, 0, -5),
		AVector3f(+5, 0, -5)));

	//lamp
	Triangle::ptr lamp1(new Triangle(lightDiffuse_mat, AVector3f(-2, 10, -2), AVector3f(+2, 10, -2),
		AVector3f(-2, 10, +2)));
	Triangle::ptr lamp2(new Triangle(lightDiffuse_mat, AVector3f(+2, 10, -2), AVector3f(+2, 10, +2),
		AVector3f(-2, 10, +2)));

	//tracer.addObjects(bottom);
	tracer.addObjects(bot1);
	tracer.addObjects(bot2);

	tracer.addObjects(top1);
	tracer.addObjects(top2);

	tracer.addObjects(lef1);
	tracer.addObjects(lef2);

	tracer.addObjects(rig1);
	tracer.addObjects(rig2);

	tracer.addObjects(bak1);
	tracer.addObjects(bak2);

	tracer.addObjects(lamp1);
	tracer.addObjects(lamp2);

	tracer.addObjects(sphere1);
	tracer.addObjects(sphere2);

	Camera *camera = tracer.getCamera();
	camera->setPosition(AVector3f(0, 5, 18));
	camera->setTarget(AVector3f(0, 5, 0));
	camera->setFovy(45.0f);
}

int main()
{
	// initialize.
	tracer.initialize(666, 500);
	tracer.setRecursionDepth(10);
	tracer.setSamplingNums(32);

	// create scene.
	cornellBoxScene();

	// render.
	Float totalTime = 0.0f;
	unsigned char * ret = nullptr;
	tracer.beginFrame();
	{
		ret = tracer.render(totalTime);
	}
	tracer.endFrame();

	// write to png image.
	stbi_flip_vertically_on_write(1);
	stbi_write_png("./result.png",
		tracer.getWidth(),
		tracer.getHeight(),
		4,
		static_cast<void*>(tracer.getImage()),
		tracer.getWidth() * 4);
	cout << "Rendering over!!\n";
	cout << "Total->" << totalTime << std::endl;
	return 0;
}