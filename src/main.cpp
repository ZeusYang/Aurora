#include <iostream>
#include "Tracer.h"
#include "Texture.h"
#include "Hitable.h"
#include "Material.h"
#include "MeshHitable.h"
#include "Camera.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace Aurora;

Tracer tracer;

void cornellBoxScene()
{
	TextureMgr::ptr texMgr = tracer.getTextureMgr();
	unsigned int white_unit = texMgr->loadTexture(new ConstantTexture(AVector3f(0.73, 0.73, 0.73)));
	unsigned int green_unit = texMgr->loadTexture(new ConstantTexture(AVector3f(0.12, 0.45, 0.15)));
	unsigned int red_unit = texMgr->loadTexture(new ConstantTexture(AVector3f(0.65, 0.05, 0.05)));
	unsigned int light_unit = texMgr->loadTexture(new ConstantTexture(AVector3f(4.0, 4.0, 4.0)));
	unsigned int yellow_light_unit = texMgr->loadTexture(new ConstantTexture(AVector3f(1.0, 0.82, 0.53)));

	Material::ptr whiteLambert_mat = std::make_shared<Lambertian>(white_unit);
	Material::ptr greenLambert_mat = std::make_shared<Lambertian>(green_unit);
	Material::ptr redLambert_mat = std::make_shared<Lambertian>(red_unit);
	Material::ptr lightDiffuse_mat = std::make_shared<DiffuseLight>(light_unit, green_unit);
	Material::ptr whiteMetal_mat = std::make_shared<Metal>(white_unit, 0.0f);
	Material::ptr bottle_mat = std::make_shared<Dielectric>(1.5f);
	Material::ptr board_mat = std::make_shared<Metal>(white_unit, 0.0f);

	Plane *bottom = new Plane(whiteLambert_mat, AVector3f(0, 0, 0), AVector3f(5, 5, 5));
	Plane *top = new Plane(whiteLambert_mat, AVector3f(0, 0, 0), AVector3f(5, 5, 5));
	Plane *back = new Plane(whiteLambert_mat, AVector3f(0, 0, 0), AVector3f(5, 5, 5));
	Plane *left = new Plane(greenLambert_mat, AVector3f(0, 0, 0), AVector3f(5, 5, 5));
	Plane *right = new Plane(redLambert_mat, AVector3f(0, 0, 0), AVector3f(5, 5, 5));
	Plane *light = new Plane(lightDiffuse_mat, AVector3f(0, 0, 0), AVector3f(2, 2, 2));
	//Plane *light = new Plane(Vector3D(0,0,0), Vector3D(2,2,2), redLambert_mat);
	//Cube *cube1 = new Cube(AVector3f(-2, 3, -2), AVector3f(1.5, 3.0, 1.5), whiteMetal_mat);
	//Cube *cube2 = new Cube(AVector3f(+2.5, 1.5, +2), AVector3f(1.5, 1.5, 1.5), whiteLambert_mat);
	Sphere *sphere1 = new Sphere(bottle_mat, AVector3f(+1.5, 1.5, +2), +1.5f);
	Sphere *sphere2 = new Sphere(whiteLambert_mat, AVector3f(-1.5, 2.5, +0), +2.5f);
	//Sphere *sphereLight = new Sphere(AVector3f(+0,0.5,+3.9), +0.5f, lightDiffuse_mat1);

	//cube1->rotate(AVector3f(0, 1, 0), -18.0f);
	//cube2->rotate(AVector3f(0, 1, 0), -15.0f);
	//dragon->rotate(Vector3D(0, 1, 0), +25);
	top->translate(AVector3f(0, 10, 0));
	back->rotate(AVector3f(1, 0, 0), 90.0f);
	back->translate(AVector3f(0, 5, -5));
	left->rotate(AVector3f(0, 0, 1), 90.0f);
	left->translate(AVector3f(-5, 5, 0));
	right->rotate(AVector3f(0, 0, 1), 90.0f);
	right->translate(AVector3f(+5, 5, 0));
	light->translate(AVector3f(0, 10, 0));
	//person->rotate(Vector3D(0,1,0), 30.0f);
	light->setName("light");
	right->setName("right");

	tracer.addObjects(bottom);
	tracer.addObjects(top);
	tracer.addObjects(back);
	tracer.addObjects(left);
	tracer.addObjects(right);
	//tracer.addObjects(cube1);
	//tracer.addObjects(cube2);
	tracer.addImportantSampling(light);
	tracer.addImportantSampling(sphere1);
	tracer.addImportantSampling(sphere2);
	//tracer.addImportantSampling(sphereLight);

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