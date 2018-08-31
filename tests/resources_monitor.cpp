/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <ionicengine/graphics/screen.h>
#include <ionicengine/graphics/animation.h>
#include <ionicengine/input/inputmanager.h>
#include <lambdacommon/system/system.h>

using namespace ionicengine;
using namespace lambdacommon;

class MainScreen : public Screen
{
private:
	Font _font;
	GuiProgressBar *progressBar = nullptr;

public:
	explicit MainScreen(const Font &font) : _font(font)
	{
		setBackgroundColor(color::fromHex(0xEEEEEEFF));
	}

	void init() override
	{
		progressBar = new GuiProgressBar(5, 10 + _font.getHeight(), 300, 15);
		progressBar->setColor(color::fromHex(0x26A1DCFF));
		components.push_back(progressBar);
	}

	void draw(Graphics *graphics) override
	{
		graphics->setColor(Color::COLOR_BLACK);
		graphics->drawText(_font, 5.f, 5.f,
						   "Memory: " + std::to_string(system::getMemoryUsed() / 1073741824.0) + "GB / " +
						   std::to_string(system::getMemoryTotal() / 1073741824.0) + "GB (" +
						   std::to_string(progressBar->getProgress()) + "%)");
		Screen::draw(graphics);
	}

	void update() override
	{
		Screen::update();
		float usage = static_cast<float>(system::getMemoryUsed()) / system::getMemoryTotal();
		progressBar->setProgress(static_cast<uint32_t>(usage * 100));
	}
};

int main()
{
	terminal::setup();
	std::cout << "Running ionic_image with IonicEngine v" + ionicengine::getVersion() << "...\n";

	IonicOptions ionicOptions;
	ionicOptions.useControllers = false;
	ionicOptions.debug = true;
	if (!ionicengine::init(ionicOptions))
		return EXIT_FAILURE;

	ScreenManager screens{};

	WindowOptions options{};
	options.context_version_major = 3;
	options.context_version_minor = 3;
	options.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
#ifdef LAMBDA_MAC_OSX
	options.opengl_forward_compat = true;
#endif

	auto window = window::createWindow("IonicEngine - Basic resources monitor", 310, 50, options);
	window.requestContext();
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		ionicengine::shutdown();
		return err;
	}

	auto font = ionicengine::getFontManager().loadFont(std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	ResourceName screensImage{"ionic_tests:screens/resources_monitor"};

	MainScreen screen{font.value()};
	screens.registerScreen(screensImage, &screen);
	screens.setActiveScreen(screensImage);

	getGraphicsManager()->init();

	screens.attachWindow(window);

	screens.startLoop();

	ionicengine::shutdown();

	return EXIT_SUCCESS;
}