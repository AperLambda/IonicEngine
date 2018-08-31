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
#include <lambdacommon/system/terminal.h>

using namespace ionicengine;
using namespace lambdacommon;

BitmapAnimation *fireAnimation = nullptr, *catAnimation = nullptr;
TexturesAnimation *fireLightAnimation = nullptr;

class KeyboardListenerImpl : public KeyboardListener
{
public:
	void onKeyInput(Window &window, int key, int scancode, InputAction action, int mods) override
	{
		if (action == InputAction::PRESS)
		{
			if (key == GLFW_KEY_ESCAPE)
				window.setShouldClose(true);
			else if (key == GLFW_KEY_SPACE)
			{
				if (fireAnimation->isRunning())
					fireAnimation->stop();
				else
					fireAnimation->start();
				if (fireLightAnimation->isRunning())
					fireLightAnimation->stop();
				else
					fireLightAnimation->start();
				if (catAnimation->isRunning())
					catAnimation->stop();
				else
					catAnimation->start();
			}
		}
	}

	void onCharInput(Window &window, char32_t codepoint) override
	{}
};

class MainScreen : public Screen
{
private:
	Color grassColor = color::mix(Color::COLOR_GREEN, Color::COLOR_BLACK, 0.5f);
	uint32_t fireX = 0, fireY = 0, quadY = 0;

public:
	MainScreen()
	{
		setBackgroundColor(Color::COLOR_BLUE);
	}

	void init() override
	{
		fireX = width / 2 - 256;
		fireY = height / 2 - 128;
		quadY = height / 2 + 128;
	}

	void draw(Graphics *graphics) override
	{
		graphics->drawImage({"ionicengine:textures/sky/night01"}, 0, 0, width, height);
		graphics->setColor(grassColor);
		graphics->drawQuad(0, quadY, width, height - quadY);
		graphics->setColor(Color::COLOR_WHITE);
		fireAnimation->setX(fireX);
		fireAnimation->setY(fireY);
		fireAnimation->render(graphics);
		fireLightAnimation->setX(fireX);
		fireLightAnimation->setY(fireY + 256);
		fireLightAnimation->render(graphics);
		catAnimation->setX(width / 2 + 32);
		catAnimation->setY(height / 2 - 128 + 32);
		catAnimation->render(graphics);

		Screen::draw(graphics);
	}

	void update() override
	{
		Screen::update();
		fireAnimation->update();
		fireLightAnimation->update();
		catAnimation->update();
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

	KeyboardListenerImpl keyboardListener{};
	InputManager::INPUT_MANAGER.addKeyboardListener(&keyboardListener);

	ScreenManager screens{};

	WindowOptions options{};
	options.context_version_major = 3;
	options.context_version_minor = 3;
	options.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
#ifdef LAMBDA_MAC_OSX
	options.opengl_forward_compat = true;
#endif

	auto window = window::createWindow("IonicEngine - Fire and Cat Animation", 948, 648, options);
	window.requestContext();
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		ionicengine::shutdown();
		return err;
	}

	auto sky = texture::load({"ionicengine:textures/sky/night01"}, "png", CLAMP, LINEAR);
	auto fire = texture::load({"ionic_tests:textures/fire_animation"}, "png", CLAMP, NEAREST);
	//auto fireLight = texture::load({"ionic_tests:textures/fire_light_animation"}, "png", CLAMP, NEAREST);
	auto fireLight01 = texture::load({"ionic_tests:textures/fire_light_animation01"}, "png", CLAMP, NEAREST);
	auto fireLight02 = texture::load({"ionic_tests:textures/fire_light_animation02"}, "png", CLAMP, NEAREST);
	auto fireLight03 = texture::load({"ionic_tests:textures/fire_light_animation03"}, "png", CLAMP, NEAREST);
	auto fireLight04 = texture::load({"ionic_tests:textures/fire_light_animation04"}, "png", CLAMP, NEAREST);
	auto cat = texture::load({"ionic_tests:textures/cat"}, "png", CLAMP,
							 NEAREST);
	if (!sky || !fire || !fireLight01 || !fireLight02 || !fireLight03 || !fireLight04 || !cat)
	{
		std::cerr << "Cannot load textures!\n";
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	auto bitmapAnimation = new BitmapAnimation{0, 0, 256, 256, fire.value(), 256};
	bitmapAnimation->setFrameTime(4);
	fireAnimation = bitmapAnimation;

	auto texturesAnimation = new TexturesAnimation{0, 0, 256, 256,
												   {fireLight01.value(), fireLight03.value(), fireLight02.value(),
													fireLight04.value(), fireLight01.value(), fireLight04.value(),
													fireLight02.value(), fireLight03.value()}};
	texturesAnimation->setFrameTime(2);
	fireLightAnimation = texturesAnimation;

	bitmapAnimation = new BitmapAnimation{0, 0, 256, 256, cat.value(), 32};
	bitmapAnimation->setFrameTime(13);
	bitmapAnimation->setInvertOnRepeat(true);
	catAnimation = bitmapAnimation;

	auto font = ionicengine::getFontManager().loadFont(std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	OverlayFPS overlay{font.value()};
	screens.registerOverlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screens.addActiveOverlay(IONICENGINE_OVERLAYS_FPS);

	ResourceName screensImage{"ionic_tests:screens/image"};

	MainScreen screen{};
	screens.registerScreen(screensImage, &screen);
	screens.setActiveScreen(screensImage);

	getGraphicsManager()->init();

	screens.attachWindow(window);

	screens.startLoop();

	delete fireAnimation;
	delete fireLightAnimation;
	delete catAnimation;
	ionicengine::shutdown();

	return EXIT_SUCCESS;
}