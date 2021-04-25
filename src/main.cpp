#include <iostream>

#include "ArScene.h"
#include "ArLight.h"
#include "ArShape.h"
#include "ArHitable.h"
#include "ArMaterial.h"
#include "ArIntegrator.h"
#include "ArPathIntegrator.h"

#include "glog/logging.h"

using namespace std;
using namespace Aurora;

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);

	Float white[] = { 0.73f, 0.73f, 0.73f };
	Float green[] = { 0.12f, 0.45f, 0.15f };
	Float red[] = { 0.65f, 0.05f, 0.05f };
	Float blue[] = { 0.05f, 0.05f, 0.75f };
	Float light[] = { 4.0f, 4.0f, 4.0f };

	const ASpectrum white_unit = ASpectrum::fromRGB(white);
	const ASpectrum green_unit = ASpectrum::fromRGB(green);
	const ASpectrum red_unit = ASpectrum::fromRGB(red);
	const ASpectrum blue_unit = ASpectrum::fromRGB(blue);
	const ASpectrum light_unit = ASpectrum::fromRGB(light);

	//Material
	AMaterial::ptr whiteLambert_mat = std::make_shared<ALambertianMaterial>(white_unit);
	AMaterial::ptr greenLambert_mat = std::make_shared<ALambertianMaterial>(green_unit);
	AMaterial::ptr blueLambert_mat = std::make_shared<AMirrorMaterial>(blue_unit);
	AMaterial::ptr redLambert_mat = std::make_shared<ALambertianMaterial>(red_unit);

	ATransform trans1 = translate(AVector3f(+1.5, 1.5, +2));
	ASphereShape::ptr sphere1 = std::make_shared<ASphereShape>(trans1, inverse(trans1), 1.5f);
	AHitableEntity::ptr entity1 = std::make_shared<AHitableEntity>(sphere1, blueLambert_mat, nullptr);

	ATransform trans2 = translate(AVector3f(-1.5, 2.5, +0));
	ASphereShape::ptr sphere2 = std::make_shared<ASphereShape>(trans2, inverse(trans2), 2.5f);
	AHitableEntity::ptr entity2 = std::make_shared<AHitableEntity>(sphere2, whiteLambert_mat, nullptr);

	//Bottom wall
	ATransform identity(AMatrix4x4(1.0f));
	AVector3f t1[3] = { AVector3f(-5, 0, -5), AVector3f(-5, 0, +5), AVector3f(+5, 0, -5) };
	ATriangleShape::ptr bot1 = std::make_shared<ATriangleShape>(identity, identity, t1);
	AHitableEntity::ptr entity3 = std::make_shared<AHitableEntity>(bot1, whiteLambert_mat, nullptr);

	AVector3f t2[3] = { AVector3f(+5, 0, -5), AVector3f(-5, 0, +5), AVector3f(+5, 0, +5) };
	ATriangleShape::ptr bot2 = std::make_shared<ATriangleShape>(identity, identity, t2);
	AHitableEntity::ptr entity4 = std::make_shared<AHitableEntity>(bot2, whiteLambert_mat, nullptr);

	//Top wall
	AVector3f t3[3] = { AVector3f(-5, 10, -5), AVector3f(+5, 10, -5), AVector3f(-5, 10, +5) };
	ATriangleShape::ptr top1 = std::make_shared<ATriangleShape>(identity, identity, t3);
	AHitableEntity::ptr entity5 = std::make_shared<AHitableEntity>(top1, whiteLambert_mat, nullptr);

	AVector3f t4[3] = { AVector3f(+5, 10, -5), AVector3f(+5, 10, +5), AVector3f(-5, 10, +5) };
	ATriangleShape::ptr top2 = std::make_shared<ATriangleShape>(identity, identity, t4);
	AHitableEntity::ptr entity6 = std::make_shared<AHitableEntity>(top2, whiteLambert_mat, nullptr);

	//Left wall
	AVector3f t5[3] = { AVector3f(-5, 10, +5), AVector3f(-5, 10, -5), AVector3f(-5, 0, +5) };
	ATriangleShape::ptr lef1 = std::make_shared<ATriangleShape>(identity, identity, t5);
	AHitableEntity::ptr entity7 = std::make_shared<AHitableEntity>(lef1, greenLambert_mat, nullptr);

	AVector3f t6[3] = { AVector3f(-5, 10, -5), AVector3f(-5, 0, -5), AVector3f(-5, 0, +5) };
	ATriangleShape::ptr lef2 = std::make_shared<ATriangleShape>(identity, identity, t6);
	AHitableEntity::ptr entity8 = std::make_shared<AHitableEntity>(lef2, greenLambert_mat, nullptr);

	//Right wall
	AVector3f t7[3] = { AVector3f(+5, 10, +5), AVector3f(+5, 10, -5), AVector3f(+5, 0, +5) };
	ATriangleShape::ptr rig1 = std::make_shared<ATriangleShape>(identity, identity, t7);
	AHitableEntity::ptr entity9 = std::make_shared<AHitableEntity>(rig1, redLambert_mat, nullptr);

	AVector3f t8[3] = { AVector3f(+5, 10, -5), AVector3f(+5, 0, -5), AVector3f(+5, 0, +5) };
	ATriangleShape::ptr rig2 = std::make_shared<ATriangleShape>(identity, identity, t8);
	AHitableEntity::ptr entity10 = std::make_shared<AHitableEntity>(rig2, redLambert_mat, nullptr);

	//Back wall
	AVector3f t9[3] = { AVector3f(-5, 10, -5), AVector3f(+5, 10, -5), AVector3f(-5, 0, -5) };
	ATriangleShape::ptr bak1 = std::make_shared<ATriangleShape>(identity, identity, t9);
	AHitableEntity::ptr entity11 = std::make_shared<AHitableEntity>(bak1, whiteLambert_mat, nullptr);

	AVector3f t10[3] = { AVector3f(+5, 10, -5), AVector3f(+5, 0, -5), AVector3f(-5, 0, -5) };
	ATriangleShape::ptr bak2 = std::make_shared<ATriangleShape>(identity, identity, t10);
	AHitableEntity::ptr entity12 = std::make_shared<AHitableEntity>(bak2, whiteLambert_mat, nullptr);

	//light
	AVector3f t11[3] = { AVector3f(-2, 10, -2), AVector3f(+2, 10, -2), AVector3f(-2, 10, +2) };
	ATriangleShape::ptr lamp1 = std::make_shared<ATriangleShape>(identity, identity, t11);
	AAreaLight::ptr light1 = std::make_shared<ADiffuseAreaLight>(identity, light_unit, 8, lamp1);
	AHitableEntity::ptr entity13 = std::make_shared<AHitableEntity>(lamp1, whiteLambert_mat, light1);

	AVector3f t12[3] = { AVector3f(+2, 10, -2), AVector3f(+2, 10, +2), AVector3f(-2, 10, +2) };
	ATriangleShape::ptr lamp2 = std::make_shared<ATriangleShape>(identity, identity, t12);
	AAreaLight::ptr light2 = std::make_shared<ADiffuseAreaLight>(identity, light_unit, 8, lamp2);
	AHitableEntity::ptr entity14 = std::make_shared<AHitableEntity>(lamp2, whiteLambert_mat, light2);

	//ATransform trans3 = translate(AVector3f(0, 10, +0));
	//ASphereShape::ptr lamp3 = std::make_shared<ASphereShape>(trans3, inverse(trans3), 1.0f);
	//AAreaLight::ptr light3 = std::make_shared<ADiffuseAreaLight>(trans3, light_unit, 8, lamp3);
	//AHitableEntity::ptr entity15 = std::make_shared<AHitableEntity>(lamp3, whiteLambert_mat, light3);

	//Aggregate
	AHitableList::ptr aggregate = std::make_shared<AHitableList>();
	aggregate->addHitable(entity1);
	aggregate->addHitable(entity2);
	aggregate->addHitable(entity3);
	aggregate->addHitable(entity4);
	aggregate->addHitable(entity5);
	aggregate->addHitable(entity6);
	aggregate->addHitable(entity7);
	aggregate->addHitable(entity8);
	aggregate->addHitable(entity9);
	aggregate->addHitable(entity10);
	aggregate->addHitable(entity11);
	aggregate->addHitable(entity12);
	aggregate->addHitable(entity13);
	aggregate->addHitable(entity14);
	//aggregate->addHitable(entity15);

	//Scene
	std::vector<ALight::ptr> lights = { light1, light2 };
	AScene::ptr scene = std::make_shared<AScene>(aggregate, lights);

	int maxDepth = 4;
	int width = 666, height = 500;

	//Film & sampler
	AVector2i res(width, height);
	std::unique_ptr<AFilter> filter(new ABoxFilter(AVector2f(0.5f, 0.5f)));
	AFilm::ptr film = std::make_shared<AFilm>(res, ABounds2f(AVector2f(0,0), AVector2f(1,1)),
		std::move(filter), "../result.png");
	ASampler::ptr sampler = std::make_shared<ARandomSampler>(8);

	Float fovy = 45.0f;
	AVector3f eye(0, 5, 18), center(0, 5, 0);
	
	//Camera
	ABounds2f screen;
	Float frame = (Float)(width) / height;
	if (frame > 1.f)
	{
		screen.m_pMin.x = -frame;
		screen.m_pMax.x = frame;
		screen.m_pMin.y = -1.f;
		screen.m_pMax.y = 1.f;
	}
	else
	{
		screen.m_pMin.x = -1.f;
		screen.m_pMax.x = 1.f;
		screen.m_pMin.y = -1.f / frame;
		screen.m_pMax.y = 1.f / frame;
	}
	auto cameraToWorld = inverse(lookAt(eye, center, AVector3f(0, 1, 0)));
	ACamera::ptr camera = std::make_shared<APerspectiveCamera>(cameraToWorld, screen, fovy, film);

	ABounds2i pixelBound(AVector2i(0, 0), AVector2i(width, height));

	AWhittedIntegrator integrator(maxDepth, camera, sampler, pixelBound);

	printf("Aurora (built %s at %s) [Detected %d cores]\n",
		__DATE__, __TIME__, numSystemCores());
	printf("Copyright (c)2021-Present Wencong Yang\n");
	printf(
		"The source code to Aurora is covered by the MIT License.\n");
	printf("See the file LICENSE.txt for the conditions of the license.\n");

	integrator.preprocess(*scene, *sampler);
	integrator.render(*scene);

	return 0;
}