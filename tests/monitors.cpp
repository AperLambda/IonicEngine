/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <ionicengine/window/monitor.h>
#include <lambdacommon/system/terminal.h>
#include <lambdacommon/lstring.h>

using namespace std;
using namespace lambdacommon;
using namespace ionicengine;

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

int main()
{
	cout << "Starting monitors.cpp with IonicEngine v" << ionicengine::getVersion() << "..." << endl;
	terminal::setup();
	ionicengine::init();
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return EXIT_FAILURE;

	cout << terminal::TermFormatting::YELLOW 
		<< "Primary monitor: " 
		<< terminal::CYAN 
		<< monitor::getPrimaryMonitor().getName()
	     << terminal::RESET << endl;
	cout << terminal::LIGHT_RED << "================" << terminal::RESET << endl;

	size_t counter(0);
	auto monitors = monitor::getMonitors();
	cout << terminal::YELLOW << "Detected monitors: " << terminal::CYAN << to_string(monitors.size()) << terminal::RESET
	     << endl;
	for (const auto &monitor : monitors)
	{
		counter++;
		cout << terminal::YELLOW << "Monitor #" << to_string(counter) << ": " << terminal::CYAN << monitor.getName()
		     << terminal::RESET << endl;
	}

	glfwTerminate();
}