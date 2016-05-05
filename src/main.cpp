// The MIT License (MIT)

// Copyright (c) 2014 nabijaczleweli

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "app/application.hpp"
#include "reference/container.hpp"
#include "cimpoler-meta.hpp"
#include <audiere.h>
#include <stdexcept>
#include <iostream>


using namespace std;


static pair<string, int> check_config();
static void init_app(application & app, int argc, char * argv[]);
static void init_deps(application & app);


int main(int argc, char * argv[]) {
	{
		const auto cfg_result = check_config();
		if(cfg_result.second) {
			cerr << "Configuration error: " << cfg_result.first << '\n';
			return cfg_result.second;
		}
	}

	application app;
	init_deps(app);
	init_app(app, argc, argv);
	const auto result = app.run();
	if(result)
		cerr << "`app.run()` failed with " << result << "! Oh noes!";
	return result;
}


static pair<string, int> check_config() {
	if(!app_configuration.vsync && !app_configuration.FPS)
		return {"VSync not enabled and FPS set to 0", -1};

	return {"", 0};
}

static void init_app(application &, int, char * []) {}

static void init_deps(application &) {
	cout << "Initializing dependencies under " << CIMPOLER_META_OS_NAME << "...\n\n"

	     << CIMPOLER_META_COMPILER_NAME " version " << cimpoler_meta::version() << " doesn\'t need initialization.\n"
	     << "SFML version " << SFML_VERSION_MAJOR << '.' << SFML_VERSION_MINOR << " doesn\'t need initialization.\n"
	     << "audiere version " << audiere::GetVersion() << " doesn\'t need initialization.\n"
	     << "cereal version " << CEREAL_VERSION << " doesn\'t need initialization.\n"
	     << "cimpoler-meta version " << CIMPOLER_META_VERSION << " doesn\'t need initialization.\n"
	     << "Cpp-NBT version " << CPP_NBT_VERSION << " doesn\'t need initialization.\n"
	     << "RapidJSON version " << RAPIDJSON_VERSION << " doesn\'t need initialization.\n"
	     << "seed11 version " << SEED11_VERSION << " doesn\'t need initialization.\n"
	     << "whereami-cpp version " << WHEREAMI_CPP_VERSION << " doesn\'t need initialization.\n"
	     << "\n"

	     << "All dependencies initialized.\n\n";
}
