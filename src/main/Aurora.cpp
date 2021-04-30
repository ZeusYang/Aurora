/*

Aurora - The MIT License (MIT)

Copyright (c) 2021-Present, Wencong Yang (yangwc3@mail2.sysu.edu.cn).

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

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

static void usage(const char *msg = nullptr) 
{
	if (msg)
	{
		fprintf(stderr, "Aurora: %s\n\n", msg);
	}

	fprintf(stderr, R"(usage: Aurora [<options>] <filename.json...>
Rendering options:
  --help               Print this help text.

Logging options:
  --logdir <dir>       Specify directory that log files should be written to.
                       Default: system temp directory (e.g. $TMPDIR or /tmp).

  --logtostderr        Print all logging messages to stderr.

  --minloglevel <num>  Log messages at or above this level (0 -> INFO,
                       1 -> WARNING, 2 -> ERROR, 3-> FATAL). Default: 0.

  --v <verbosity>      Set VLOG verbosity.
)");
	exit(msg ? 1 : 0);
}

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);

	if (argc < 2)
	{
		usage("need to set an input file path");
		return 0;
	}

	std::vector<std::string> filenames;
	for (int i = 1; i < argc; ++i) 
	{
		if (!strcmp(argv[i], "--logdir") || !strcmp(argv[i], "-logdir")) 
		{
			if (i + 1 == argc)
				usage("missing value after --logdir argument");
			FLAGS_log_dir = argv[++i];
		}
		else if (!strncmp(argv[i], "--logdir=", 9)) 
		{
			FLAGS_log_dir = &argv[i][9];
		}
		else if (!strcmp(argv[i], "--minloglevel") ||
			!strcmp(argv[i], "-minloglevel")) 
		{
			if (i + 1 == argc)
				usage("missing value after --minloglevel argument");
			FLAGS_minloglevel = atoi(argv[++i]);
		}
		else if (!strncmp(argv[i], "--minloglevel=", 14)) 
		{
			FLAGS_minloglevel = atoi(&argv[i][14]);
		}
		else if (!strcmp(argv[i], "--v") || !strcmp(argv[i], "-v")) 
		{
			if (i + 1 == argc)
				usage("missing value after --v argument");
			FLAGS_v = atoi(argv[++i]);
		}
		else if (!strncmp(argv[i], "--v=", 4)) 
		{
			FLAGS_v = atoi(argv[i] + 4);
		}
		else if (!strcmp(argv[i], "--logtostderr")) 
		{
			FLAGS_logtostderr = true;
		}
		else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-help") ||
			!strcmp(argv[i], "-h")) 
		{
			usage();
			return 0;
		}
		else
		{
			filenames.push_back(argv[i]);
		}
	}

	//Banner
	{
		printf("Aurora (built %s at %s) [Detected %d cores]\n", __DATE__, __TIME__, numSystemCores());
		printf("Copyright (c)2021-Present Wencong Yang\n");
		printf("The source code to Aurora is covered by the MIT License.\n");
		printf("See the file LICENSE.txt for the conditions of the license.\n");
	}

	auto parsing_func = [](const std::string &filename) -> void
	{
		AScene::ptr scene = nullptr;
		ASampler::ptr sampler = nullptr;
		ASamplerIntegrator::ptr integrator = nullptr;

		AParser::parser(filename, scene, sampler, integrator);

		CHECK_NE(scene, nullptr);
		CHECK_NE(sampler, nullptr);
		CHECK_NE(integrator, nullptr);

		integrator->preprocess(*scene, *sampler);
		integrator->render(*scene);
	};
	
	for (const auto & f : filenames)
	{
		parsing_func(f);
	}

	return 0;
}