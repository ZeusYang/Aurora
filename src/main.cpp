#include <iostream>
#include "Tracer.h"
#include "Hitable.h"
#include "Material.h"

using namespace std;
using namespace Aurora;

Tracer tracer;

void cornellBoxScene()
{
	Float white[] = { 0.73f, 0.73f, 0.73f };
	Float green[] = { 0.12, 0.45, 0.15 };
	Float red[] = { 0.65, 0.05, 0.05 };
	Float blue[] = { 0.05, 0.05, 0.75 };
	Float light[] = { 4.0, 4.0, 4.0 };

	const ASpectrum white_unit = ASpectrum::fromRGB(white);
	const ASpectrum green_unit = ASpectrum::fromRGB(green);
	const ASpectrum red_unit = ASpectrum::fromRGB(red);
	const ASpectrum blue_unit = ASpectrum::fromRGB(blue);
	const ASpectrum light_unit = ASpectrum::fromRGB(light);

	Material::ptr whiteLambert_mat = std::make_shared<Lambertian>(white_unit);
	Material::ptr greenLambert_mat = std::make_shared<Lambertian>(green_unit);
	Material::ptr blueLambert_mat = std::make_shared<Lambertian>(blue_unit);
	Material::ptr redLambert_mat = std::make_shared<Lambertian>(red_unit);
	Material::ptr lightDiffuse_mat = std::make_shared<DiffuseLight>(light_unit, green_unit);

	ATransform trans1 = translate(AVector3f(+1.5, 1.5, +2));
	ASphereShape::ptr sphere1 = std::make_shared<ASphereShape>(trans1, inverse(trans1), 1.5f);
	HitableEntity::ptr entity1 = std::make_shared<HitableEntity>(blueLambert_mat, sphere1);

	ATransform trans2 = translate(AVector3f(-1.5, 2.5, +0));
	ASphereShape::ptr sphere2 = std::make_shared<ASphereShape>(trans2, inverse(trans2), 2.5f);
	HitableEntity::ptr entity2 = std::make_shared<HitableEntity>(whiteLambert_mat, sphere2);

	//Bottom wall
	ATransform identity(AMatrix4x4(1.0f));

	AVector3f t1[3] = { AVector3f(-5, 0, -5), AVector3f(-5, 0, +5), AVector3f(+5, 0, -5) };
	ATriangleShape::ptr bot1 = std::make_shared<ATriangleShape>(identity, identity, t1);
	HitableEntity::ptr entity3 = std::make_shared<HitableEntity>(whiteLambert_mat, bot1);

	AVector3f t2[3] = { AVector3f(+5, 0, -5), AVector3f(-5, 0, +5), AVector3f(+5, 0, +5) };
	ATriangleShape::ptr bot2 = std::make_shared<ATriangleShape>(identity, identity, t2);
	HitableEntity::ptr entity4 = std::make_shared<HitableEntity>(whiteLambert_mat, bot2);
	
	//Top wall

	AVector3f t3[3] = { AVector3f(-5, 10, -5), AVector3f(+5, 10, -5), AVector3f(-5, 10, +5) };
	ATriangleShape::ptr top1 = std::make_shared<ATriangleShape>(identity, identity, t3);
	HitableEntity::ptr entity5 = std::make_shared<HitableEntity>(whiteLambert_mat, top1);

	AVector3f t4[3] = { AVector3f(+5, 10, -5), AVector3f(+5, 10, +5), AVector3f(-5, 10, +5) };
	ATriangleShape::ptr top2 = std::make_shared<ATriangleShape>(identity, identity, t4);
	HitableEntity::ptr entity6 = std::make_shared<HitableEntity>(whiteLambert_mat, top2);

	//Left wall

	AVector3f t5[3] = { AVector3f(-5, 10, +5), AVector3f(-5, 0, +5), AVector3f(-5, 10, -5) };
	ATriangleShape::ptr lef1 = std::make_shared<ATriangleShape>(identity, identity, t5);
	HitableEntity::ptr entity7 = std::make_shared<HitableEntity>(greenLambert_mat, lef1);

	AVector3f t6[3] = { AVector3f(-5, 10, -5), AVector3f(-5, 0, +5), AVector3f(-5, 0, -5) };
	ATriangleShape::ptr lef2 = std::make_shared<ATriangleShape>(identity, identity, t6);
	HitableEntity::ptr entity8 = std::make_shared<HitableEntity>(greenLambert_mat, lef2);

	//Right wall

	AVector3f t7[3] = { AVector3f(+5, 10, +5), AVector3f(+5, 10, -5), AVector3f(+5, 0, +5) };
	ATriangleShape::ptr rig1 = std::make_shared<ATriangleShape>(identity, identity, t7);
	HitableEntity::ptr entity9 = std::make_shared<HitableEntity>(redLambert_mat, rig1);

	AVector3f t8[3] = { AVector3f(+5, 10, -5), AVector3f(+5, 0, -5), AVector3f(+5, 0, +5) };
	ATriangleShape::ptr rig2 = std::make_shared<ATriangleShape>(identity, identity, t8);
	HitableEntity::ptr entity10 = std::make_shared<HitableEntity>(redLambert_mat, rig2);

	//Back wall

	AVector3f t9[3] = { AVector3f(-5, 10, -5), AVector3f(-5, 0, -5), AVector3f(+5, 10, -5) };
	ATriangleShape::ptr bak1 = std::make_shared<ATriangleShape>(identity, identity, t9);
	HitableEntity::ptr entity11 = std::make_shared<HitableEntity>(whiteLambert_mat, bak1);

	AVector3f t10[3] = { AVector3f(+5, 10, -5), AVector3f(-5, 0, -5), AVector3f(+5, 0, -5) };
	ATriangleShape::ptr bak2 = std::make_shared<ATriangleShape>(identity, identity, t10);
	HitableEntity::ptr entity12 = std::make_shared<HitableEntity>(whiteLambert_mat, bak2);

	//lamp

	AVector3f t11[3] = { AVector3f(-2, 10, -2), AVector3f(+2, 10, -2), AVector3f(-2, 10, +2) };
	ATriangleShape::ptr lamp1 = std::make_shared<ATriangleShape>(identity, identity, t11);
	HitableEntity::ptr entity13 = std::make_shared<HitableEntity>(lightDiffuse_mat, lamp1);

	AVector3f t12[3] = { AVector3f(+2, 10, -2), AVector3f(+2, 10, +2), AVector3f(-2, 10, +2) };
	ATriangleShape::ptr lamp2 = std::make_shared<ATriangleShape>(identity, identity, t12);
	HitableEntity::ptr entity14 = std::make_shared<HitableEntity>(lightDiffuse_mat, lamp2);

	tracer.addObjects(entity1);
	tracer.addObjects(entity2);
	tracer.addObjects(entity3);
	tracer.addObjects(entity4);
	tracer.addObjects(entity5);
	tracer.addObjects(entity6);
	tracer.addObjects(entity7);
	tracer.addObjects(entity8);
	tracer.addObjects(entity9);
	tracer.addObjects(entity10);
	tracer.addObjects(entity11);
	tracer.addObjects(entity12);
	tracer.addObjects(entity13);
	tracer.addObjects(entity14);

}

int main()
{
	// initialize.
	AVector3f eye(0, 5, 18);
	AVector3f center(0, 5, 0);
	tracer.initialize(666, 500, 32, 10, eye, center, 45.0f);

	// create scene.
	cornellBoxScene();

	// render.
	Float totalTime = 0.0f;

	tracer.beginFrame();
	tracer.render(totalTime);
	tracer.endFrame();

	cout << "Rendering over!!\n";
	cout << "Total->" << totalTime << std::endl;

	return 0;
}