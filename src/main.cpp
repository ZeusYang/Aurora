#include <iostream>

#include "ArScene.h"
#include "ArLight.h"
#include "ArShape.h"
#include "ArHitable.h"
#include "ArMaterial.h"
#include "ArIntegrator.h"
#include "ArPathIntegrator.h"

#include "ArParser.h"

using namespace std;
using namespace Aurora;

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);

	printf("Aurora (built %s at %s) [Detected %d cores]\n",
		__DATE__, __TIME__, numSystemCores());
	printf("Copyright (c)2021-Present Wencong Yang\n");
	printf(
		"The source code to Aurora is covered by the MIT License.\n");
	printf("See the file LICENSE.txt for the conditions of the license.\n");

	AScene::ptr scene = nullptr;
	ASampler::ptr sampler = nullptr;
	ASamplerIntegrator::ptr integrator = nullptr;

	AParser::parser("../scenes/cornellBox.json", scene, sampler, integrator);

	CHECK_NE(scene, nullptr);
	CHECK_NE(sampler, nullptr);
	CHECK_NE(integrator, nullptr);

	integrator->preprocess(*scene, *sampler);
	integrator->render(*scene);

	return 0;
}