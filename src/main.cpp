#include <iostream>
#include "Tracer.h"
#include "Texture.h"
#include "Hitable.h"
#include "Material.h"
#include "SimpleMeshHitable.h"
#include "ModelHitable.h"
#include "Camera.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;
using namespace RayTracer;

Tracer tracer;

void randomScene()
{
	TextureMgr::ptr texMgr = tracer.getTextureMgr();
	MaterialMgr::ptr matMgr = tracer.getMaterialMgr();
	//    unsigned int checker_unit = texMgr->loadTexture(new CheckerTexture(
	//        new ConstantTexture(Vector3D(0.2f, 0.3f, 0.1f)),
	//        new ConstantTexture(Vector3D(0.9f, 0.9f, 0.9f))));
	unsigned int bottle_mat = matMgr->loadMaterial(new Dielectric(1.5f));
	//    tracer.addObjects(new Sphere(Vector3D(0, -1000.0, 0), 1000,
	//        matMgr->loadMaterial(new Metal(texMgr->loadTexture(new ConstantTexture(Vector3D(1.0,1.0,1.0))),
	//                                       0.1f))));
	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			float choose_mat = drand48();
			Vector3D center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
			if ((center - Vector3D(4, 0.2, 0)).getLength() > 0.9)
			{
				// diffuse.
				if (choose_mat < 0.8f)
					tracer.addObjects(new Sphere(center, 0.2,
						matMgr->loadMaterial(new Lambertian
						(texMgr->loadTexture(new ConstantTexture(
							Vector3D(drand48()*drand48(),
								drand48()*drand48(),
								drand48()*drand48())))))));
				// metal
				else if (choose_mat < 0.95f)
					tracer.addObjects(new Sphere(center, 0.2,
						matMgr->loadMaterial(new Metal
						(texMgr->loadTexture(new ConstantTexture(
							Vector3D(
								0.5f*(1.0f + drand48()),
								0.5f*(1.0f + drand48()),
								0.5f*(1.0f + drand48())))),
							0.5f*drand48()))));
				// glass
				else
					tracer.addObjects(new Sphere(center, 0.2, bottle_mat));
			}
		}
	}
	tracer.addObjects(new Sphere(Vector3D(0, 1, 0), 1.0, bottle_mat));
	tracer.addObjects(new Sphere(Vector3D(-4, 1, 0), 1.0,
		matMgr->loadMaterial(new Lambertian(texMgr->loadTexture(new ConstantTexture(
			Vector3D(0.4, 0.2, 0.1)))))));
	tracer.addObjects(new Sphere(Vector3D(4, 1, 0), 3.0,
		matMgr->loadMaterial(new Metal(texMgr->loadTexture(new ConstantTexture(
			Vector3D(0.7, 0.6, 0.5))), 0.0f))));
	tracer.setSkybox("./res/skybox1/", ".png");

	Camera *camera = tracer.getCamera();
	camera->setPosition(Vector3D(7, 3, 9));
	camera->setPosition(Vector3D(+9, 2, 4));
	camera->setTarget(Vector3D(0, 0, 0));
	camera->setFovy(60.0f);
}

void twoSpheresScene()
{
	TextureMgr::ptr texMgr = tracer.getTextureMgr();
	unsigned int checker_unit = texMgr->loadTexture(new CheckerTexture(
		new ConstantTexture(Vector3D(0.2f, 0.3f, 0.1f)),
		new ConstantTexture(Vector3D(0.9f, 0.9f, 0.9f))));
	unsigned int earth_unit = texMgr->loadTexture(new ImageTexture("./res/earth.jpg"));

	MaterialMgr::ptr matMgr = tracer.getMaterialMgr();
	unsigned int checker_mat = matMgr->loadMaterial(new Lambertian(checker_unit));
	unsigned int earth_mat = matMgr->loadMaterial(new Lambertian(earth_unit));
	unsigned int bottle_mat = matMgr->loadMaterial(new Dielectric(1.5f));

	tracer.addObjects(new Sphere(Vector3D(0, -1000, 0), 1000, checker_mat));
	tracer.addObjects(new Sphere(Vector3D(-2, +1.0, 0), 1.0, earth_mat));
	tracer.addObjects(new Sphere(Vector3D(2, +1.0, 0), 1.0, bottle_mat));
	tracer.setSkybox("./res/skybox0/", ".jpg");

	Camera *camera = tracer.getCamera();
	camera->setPosition(Vector3D(0, 5, 18));
	camera->setTarget(Vector3D(0, 5, 0));
	camera->setFovy(45.0f);
}

void simpleLightScene()
{
	// load texture.
	TextureMgr::ptr texMgr = tracer.getTextureMgr();
	unsigned int checker_unit = texMgr->loadTexture(new CheckerTexture(
		new ConstantTexture(Vector3D(0.0f, 0.0f, 0.0f)),
		new ConstantTexture(Vector3D(0.9f, 0.9f, 0.9f))));
	unsigned int earth_unit = texMgr->loadTexture(new ImageTexture("./res/earth.jpg"));
	unsigned int white_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(1.0, 1.0, 1.0)));
	unsigned int gray_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(0.75, 0.75, 0.75)));
	unsigned int tree_unit = texMgr->loadTexture(new ImageTexture("./res/tree.png"));
	unsigned int lowPoly_unit = texMgr->loadTexture(new ImageTexture("./res/lowPolyTree.png"));
	unsigned int board_unit = texMgr->loadTexture(new ImageTexture("./res/board.bmp"));
	unsigned int person_unit = texMgr->loadTexture(new ImageTexture("./res/person.png"));
	unsigned int robot_unit = texMgr->loadTexture(new ImageTexture("./res/robot.jpg"));

	// load material.
	MaterialMgr::ptr matMgr = tracer.getMaterialMgr();
	unsigned int bottle_mat = matMgr->loadMaterial(new Dielectric(1.5f));
	unsigned int earth_mat = matMgr->loadMaterial(new Lambertian(earth_unit));
	unsigned int whiteMetal_mat = matMgr->loadMaterial(new Metal(white_unit, 0.0f));
	unsigned int grayMetal_mat = matMgr->loadMaterial(new Metal(gray_unit, 0.2f));
	unsigned int checkerLambert_mat = matMgr->loadMaterial(new Lambertian(checker_unit));
	unsigned int treeLambert_mat = matMgr->loadMaterial(new Lambertian(tree_unit));
	unsigned int lowPolyTreeLambert_mat = matMgr->loadMaterial(new Lambertian(lowPoly_unit));
	unsigned int boardLambert_mat = matMgr->loadMaterial(new Lambertian(board_unit));
	unsigned int personLambert_mat = matMgr->loadMaterial(new Lambertian(person_unit));
	unsigned int robotMetal_mat = matMgr->loadMaterial(new Metal(robot_unit, 0.00f));

	Vector3D p0(0, 4, 0), p1(-4, 0, 0), p2(4, 0, 0);
	tracer.addObjects(new Sphere(Vector3D(+5.5, +1.0f, 0), 1.0, bottle_mat));
	tracer.addObjects(new Sphere(Vector3D(-6.5, +1.0f, 3), 1.0, earth_mat));
	tracer.addObjects(new Sphere(Vector3D(-5.5, +2.0f, -3.0), 2.0, whiteMetal_mat));
	Plane *plane = new Plane(Vector3D(0, 0, 0), Vector3D(100, 100, 100), grayMetal_mat);
	Cube *box = new Cube(Vector3D(0, 1.5, -4), Vector3D(1, 1.5, 1), checkerLambert_mat);
	//    ModelHitable *bunny = new ModelHitable("./res/stall.obj", Vector3D(0,2.5,0),
	//                                           Vector3D(1.0,1.0,1.0),
	//                                           new Lambertian(new ImageTexture("./res/stall.png")));
	//    bunny->rotate(Vector3D(0,1,0), 180.0f);
	//        ModelHitable *bunny = new ModelHitable("./res/tank.obj", Vector3D(0,3,0),
	//                                               Vector3D(0.5,0.5,0.5),new Lambertian(
	//                                                   new ConstantTexture(
	//                                                       Vector3D(0.7, 0.0, 0.0))));
	ModelHitable *tree1 = new ModelHitable("./res/tree.obj", Vector3D(-11, 1.2, -5),
		Vector3D(2.7, 2.7, 2.7), treeLambert_mat);
	ModelHitable *tree2 = new ModelHitable("./res/tree.obj", Vector3D(4, 1.2, -4),
		Vector3D(2.0, 2.0, 2.0), treeLambert_mat);
	//    ModelHitable *tree3 = new ModelHitable("./res/lowPolyTree.obj", Vector3D(-1, +12, +4),
	//        Vector3D(0.15, 0.15, 0.15), lowPolyTreeLambert_mat);
	ModelHitable *tree4 = new ModelHitable("./res/tree.obj", Vector3D(-8, 1.2, -6),
		Vector3D(3.0, 3.0, 3.0), treeLambert_mat);
	ModelHitable *tree5 = new ModelHitable("./res/tree.obj", Vector3D(-12, 1.2, -9),
		Vector3D(3.5, 3.5, 3.5), treeLambert_mat);
	ModelHitable *board = new ModelHitable("./res/board.obj", Vector3D(0, +3, -9),
		Vector3D(1.0, 1.0, 1.0), boardLambert_mat);
	ModelHitable *robot = new ModelHitable("./res/robot.obj", Vector3D(+8, +2.8, -6),
		Vector3D(3.2, 3.2, 3.2), robotMetal_mat);
	//    ModelHitable *robot = new ModelHitable("./res/robot.obj", Vector3D(-4.5, +2.8, +10),
	//        Vector3D(3.2, 3.2, 3.2), robotMetal_mat);
	ModelHitable *person = new ModelHitable("./res/person.obj", Vector3D(+0, +5.3, -0),
		Vector3D(0.5, 0.5, 0.5), personLambert_mat);
	ModelHitable *dragon = new ModelHitable("./res/dragon.obj", Vector3D(+5, +5, 5),
		Vector3D(1.0, 1.0, 1.0), bottle_mat);
	//ModelHitable *bunny = new ModelHitable("./res/bunny.obj", Vector3D(+5, +5, 5),
	//        Vector3D(0.6, 0.6, 0.6), bottle_mat);
	robot->rotate(Vector3D(0, 1, 0), -30.0f);
	board->rotate(Vector3D(0, 1, 0), 90.0f);
	box->rotate(Vector3D(0, 1, 0), 45.0f);
	tracer.addObjects(plane);
	tracer.addObjects(box);
	tracer.addObjects(tree1);
	tracer.addObjects(tree2);
	//tracer.addObjects(tree3);
	tracer.addObjects(tree4);
	tracer.addObjects(tree5);
	tracer.addObjects(board);
	tracer.addObjects(robot);
	tracer.addObjects(person);
	//tracer.addObjects(bunny);
	tracer.addObjects(dragon);
	//    m_list.push_back(new Sphere(Vector3D(0, +8.0f, 0), 2.0, new DiffuseLight(
	//                                    new ConstantTexture(Vector3D(4.0f, 4.0f, 4.0f)))));
	tracer.setSkybox("./res/skybox0/", ".jpg");

	Camera *camera = tracer.getCamera();
	camera->setPosition(Vector3D(0, 5, 22));
	camera->setTarget(Vector3D(0, 5, 0));
	camera->setFovy(45.0f);
}

void cornellBoxScene()
{
	TextureMgr::ptr texMgr = tracer.getTextureMgr();
	unsigned int white_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(0.73, 0.73, 0.73)));
	unsigned int green_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(0.12, 0.45, 0.15)));
	unsigned int red_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(0.65, 0.05, 0.05)));
	unsigned int light_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(4.0, 4.0, 4.0)));
	unsigned int yellow_light_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(1.0, 0.82, 0.53)));

	MaterialMgr::ptr matMgr = tracer.getMaterialMgr();
	unsigned int whiteLambert_mat = matMgr->loadMaterial(new Lambertian(white_unit));
	unsigned int greenLambert_mat = matMgr->loadMaterial(new Lambertian(green_unit));
	unsigned int redLambert_mat = matMgr->loadMaterial(new Lambertian(red_unit));
	unsigned int lightDiffuse_mat = matMgr->loadMaterial(new DiffuseLight(light_unit, green_unit));
	unsigned int lightDiffuse_mat1 = matMgr->loadMaterial(new DiffuseLight(light_unit, green_unit));
	unsigned int whiteMetal_mat = matMgr->loadMaterial(new Metal(white_unit, 0.0f));
	unsigned int bottle_mat = matMgr->loadMaterial(new Dielectric(1.5f));
	unsigned int board_mat = matMgr->loadMaterial(new Metal(white_unit, 0.0f));
	//unsigned int diamond_mat = matMgr->loadMaterial(new Dielectric(2.5f));

	Plane *bottom = new Plane(Vector3D(0, 0, 0), Vector3D(5, 5, 5), whiteLambert_mat);
	Plane *top = new Plane(Vector3D(0, 0, 0), Vector3D(5, 5, 5), whiteLambert_mat);
	Plane *back = new Plane(Vector3D(0, 0, 0), Vector3D(5, 5, 5), whiteLambert_mat);
	Plane *left = new Plane(Vector3D(0, 0, 0), Vector3D(5, 5, 5), greenLambert_mat);
	Plane *right = new Plane(Vector3D(0, 0, 0), Vector3D(5, 5, 5), redLambert_mat);
	Plane *light = new Plane(Vector3D(0, 0, 0), Vector3D(2, 2, 2), lightDiffuse_mat);
	//Plane *light = new Plane(Vector3D(0,0,0), Vector3D(2,2,2), redLambert_mat);
	Cube *cube1 = new Cube(Vector3D(-2, 3, -2), Vector3D(1.5, 3.0, 1.5), whiteMetal_mat);
	Cube *cube2 = new Cube(Vector3D(+2.5, 1.5, +2), Vector3D(1.5, 1.5, 1.5), whiteLambert_mat);
	Sphere *sphere = new Sphere(Vector3D(+2.5, 4.5, +2), +1.5f, bottle_mat);
	Sphere *sphereLight = new Sphere(Vector3D(+0,0.5,+3.9), +0.5f, lightDiffuse_mat1);
	//ModelHitable *person = new ModelHitable("./res/person.obj", Vector3D(-2, +5.3, +3),
	//    Vector3D(0.5, 0.5, 0.5), diamond_mat);
	//ModelHitable *dragon = new ModelHitable("./res/dragon.obj", Vector3D(-1.5, +5, +2.5),
	//	Vector3D(0.5, 0.5, 0.5), whiteMetal_mat);

	cube1->rotate(Vector3D(0, 1, 0), +18);
	cube2->rotate(Vector3D(0, 1, 0), -15);
	//dragon->rotate(Vector3D(0, 1, 0), +25);
	top->translate(Vector3D(0, 10, 0));
	back->rotate(Vector3D(1, 0, 0), 90.0f);
	back->translate(Vector3D(0, 5, -5));
	left->rotate(Vector3D(0, 0, 1), 90.0f);
	left->translate(Vector3D(-5, 5, 0));
	right->rotate(Vector3D(0, 0, 1), 90.0f);
	right->translate(Vector3D(+5, 5, 0));
	light->translate(Vector3D(0, 10, 0));
	//person->rotate(Vector3D(0,1,0), 30.0f);
	light->setName("light");
	right->setName("right");

	tracer.addObjects(bottom);
	tracer.addObjects(top);
	tracer.addObjects(back);
	tracer.addObjects(left);
	tracer.addObjects(right);
	tracer.addObjects(cube1);
	tracer.addObjects(cube2);
	//tracer.addObjects(dragon);
	tracer.addImportantSampling(light);
	tracer.addImportantSampling(sphere);
	tracer.addImportantSampling(sphereLight);
	//tracer.setSkybox("./res/skybox0/", ".jpg");
	//tracer.addObjects(person);

	Camera *camera = tracer.getCamera();
	camera->setPosition(Vector3D(0, 5, 18));
	camera->setTarget(Vector3D(0, 5, 0));
	camera->setFovy(45.0f);
}

void finalLightScene()
{
	// load texture.
	TextureMgr::ptr texMgr = tracer.getTextureMgr();
	unsigned int gray_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(0.75, 0.75, 0.75)));
	unsigned int light1 = texMgr->loadTexture(new ConstantTexture(Vector3D(1.00, 0.08, 0.58)));
	unsigned int light2 = texMgr->loadTexture(new ConstantTexture(Vector3D(0.54, 0.17, 0.89)));
	unsigned int light3 = texMgr->loadTexture(new ConstantTexture(Vector3D(0.00, 1.00, 1.00)));
	unsigned int light4 = texMgr->loadTexture(new ConstantTexture(Vector3D(0.00, 1.00, 0.50)));
	unsigned int light5 = texMgr->loadTexture(new ConstantTexture(Vector3D(1.00, 1.00, 0.00)));
	unsigned int stall_tex = texMgr->loadTexture(new ImageTexture("./res/stall.png"));
	unsigned int person_tex = texMgr->loadTexture(new ImageTexture("./res/person.png"));
	unsigned int yellow_light_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(1.0, 0.82, 0.53)));
	unsigned int white_light_unit = texMgr->loadTexture(new ConstantTexture(Vector3D(6.0, 6.0, 6.0)));

	// load material.
	MaterialMgr::ptr matMgr = tracer.getMaterialMgr();
	unsigned int diamond_mat = matMgr->loadMaterial(new Dielectric(1.5f));
	unsigned int metal_mat = matMgr->loadMaterial(new Metal(gray_unit, 0.1f));
	unsigned int board_amt = matMgr->loadMaterial(new Lambertian(gray_unit));
	unsigned int lightDiffuse_mat = matMgr->loadMaterial(new DiffuseLight(yellow_light_unit, gray_unit));
	unsigned int stall_mat = matMgr->loadMaterial(new Lambertian(stall_tex));
	unsigned int white_light_mat = matMgr->loadMaterial(new DiffuseLight(white_light_unit, gray_unit));
	unsigned int person_mat = matMgr->loadMaterial(new Lambertian(person_tex));
	unsigned int light_mat[5];
	light_mat[0] = matMgr->loadMaterial(new DiffuseLight(light1, gray_unit));
	light_mat[1] = matMgr->loadMaterial(new DiffuseLight(light2, gray_unit));
	light_mat[2] = matMgr->loadMaterial(new DiffuseLight(light3, gray_unit));
	light_mat[3] = matMgr->loadMaterial(new DiffuseLight(light4, gray_unit));
	light_mat[4] = matMgr->loadMaterial(new DiffuseLight(light5, gray_unit));

	Plane *plane = new Plane(Vector3D(0, 0, 0), Vector3D(100, 100, 100), board_amt);
	ModelHitable *person = new ModelHitable("./res/person.obj", Vector3D(-2.0, +5.3, +4.0),
		Vector3D(0.3, 0.3, 0.3), person_mat);
	person->rotate(Vector3D(0, 1, 0), 30.0f);
	//    ModelHitable *model = new ModelHitable("./res/robot.obj", Vector3D(+0, +2.8, -0),
	//        Vector3D(3.2, 3.2, 3.2), metal_mat);
	ModelHitable *model = new ModelHitable("./res/stall.obj", Vector3D(+0, +2.6, +2.0),
		Vector3D(0.8, 0.8, 0.8), stall_mat);
	model->rotate(Vector3D(0, 1, 0), 180.0f);

	float max_radius = 8.0f;
	float min_radius = 5.0f;
	float delta_radius = max_radius - min_radius;
	int nums = 80;
	float step = 360.0f / nums;
	float perRad = 0.2f;
	for (int x = 0; x < nums; ++x)
	{
		Vector3D center;
		float cur_rad = min_radius + delta_radius * drand48();
		center.x = cur_rad * cos(radians(x * step));
		center.y = perRad;
		center.z = cur_rad * sin(radians(x * step));
		unsigned int target_mat = light_mat[x % 5];
		tracer.addImportantSampling(new Sphere(center, perRad, target_mat));
	}
	Cube *cube = new Cube(Vector3D(0.5f, +0.5f, +3.0f), Vector3D(0.5, 0.5, 0.5), metal_mat);
	Sphere *bottle = new Sphere(Vector3D(+1.5f, 0.5f, +3.0f), 0.5f, diamond_mat);
	Sphere *light = new Sphere(Vector3D(-4.0, 6.0, 0.0), 0.8f, lightDiffuse_mat);

	cube->rotate(Vector3D(0, 1, 0), 45.0f);
	tracer.addObjects(plane);
	tracer.addObjects(model);
	tracer.addObjects(person);
	tracer.addObjects(cube);
	tracer.addObjects(bottle);
	tracer.addImportantSampling(new Sphere(Vector3D(+1.0, +2.2, +1.8), 0.4f, white_light_mat));
	tracer.addImportantSampling(light);
	//tracer.setSkybox("./res/skybox1/", ".png");
	tracer.setSkybox("./res/skybox0/", ".jpg");

	Camera *camera = tracer.getCamera();
	camera->setPosition(Vector3D(0, 3, 14));
	camera->setTarget(Vector3D(0, 0, 0));
	camera->setFovy(45.0f);
}


int main()
{
	// initialize.
	tracer.initialize(800, 600);
	tracer.setRecursionDepth(50);
	tracer.setSamplingNums(16);

	// create scene.
	//simpleLightScene();
	//randomScene();
	//twoSpheresScene();
	cornellBoxScene();
	//finalLightScene();
	//Camera *camera = tracer.getCamera();

	// render.
	double totalTime = 0.0f;
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