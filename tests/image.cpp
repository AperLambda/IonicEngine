/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <ionicengine/graphics/screen.h>
#include <ionicengine/input/inputmanager.h>
#include <lambdacommon/system/system.h>

using namespace lambdacommon;
using namespace ionicengine;

class KeyboardListenerImpl : public KeyboardListener
{
public:
	void onKeyInput(Window &window, int key, int scancode, InputAction action, int mods) override
	{
		if (action == InputAction::PRESS)
		{
			if (key == GLFW_KEY_ESCAPE)
				window.setShouldClose(true);
		}
	}

	void onCharInput(Window &window, char32_t codepoint) override
	{}
};

class MainScreen : public Screen
{
private:
	Texture texture;

public:
	MainScreen(const Texture &texture) : texture(texture)
	{}

	void init() override
	{}

	void draw(Graphics *graphics) override
	{
		auto ratio5Width = static_cast<uint32_t>(.10f * width), ratio5Height = static_cast<uint32_t>(.10f * height);
		auto textureWidth = width, textureHeight = height;
		uint32_t quadWidth = textureWidth - (ratio5Width * 2), quadHeight = textureHeight - (ratio5Height * 2);
		graphics->setColor(Color::COLOR_WHITE);
		graphics->drawImage({"ionic_tests:textures/conifer-dark-green-daylight-572937"}, 0, 0, textureWidth,
							textureHeight);
		graphics->drawImage(texture, ratio5Width, ratio5Height, quadWidth, quadHeight, texture::newTextureRegion(
				textureWidth, textureHeight,
				ratio5Width, ratio5Height,
				quadWidth, quadHeight));
		graphics->setColor({0.f, 0.f, 0.f, .25f});
		graphics->drawQuad(ratio5Width, ratio5Height, quadWidth, quadHeight);
		graphics->setColor(Color::COLOR_BLACK);
		graphics->drawLine2D(ratio5Width, ratio5Height, ratio5Width + quadWidth, ratio5Height);
		graphics->drawLine2D(ratio5Width, ratio5Height, ratio5Width, ratio5Height + quadHeight);
		graphics->drawLine2D(ratio5Width, ratio5Height + quadHeight, ratio5Width + quadWidth,
							 ratio5Height + quadHeight);
		graphics->drawLine2D(ratio5Width + quadWidth, ratio5Height, ratio5Width + quadWidth, ratio5Height + quadHeight);
	}

	void update() override
	{}
};

int main()
{
	terminal::setup();
	std::cout << "Running ionic_image with IonicEngine v" + ionicengine::getVersion() << "...\n";

	IonicOptions ionicOptions;
	ionicOptions.debug = true;
	ionicOptions.useControllers = false;
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

	auto window = window::createWindow("IonicEngine - Image", 948, 648, options);
	window.requestContext();
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		ionicengine::shutdown();
		return err;
	}

	auto texture = texture::load({"ionic_tests:textures/conifer-dark-green-daylight-572937"}, "jpg", CLAMP,
								 LINEAR_MIPMAP_LINEAR);
	auto texture_blurred = texture::load({"ionic_tests:textures/conifer-dark-green-daylight-572937-blurred"}, "jpg",
										 CLAMP, LINEAR_MIPMAP_LINEAR);
	if (!texture || !texture_blurred)
	{
		std::cerr << "Cannot load textures!\n";
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	auto font = ionicengine::getFontManager()->loadFont({"google:fonts/roboto"}, std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	OverlayFPS overlay{font.value()};
	screens.registerOverlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screens.addActiveOverlay(IONICENGINE_OVERLAYS_FPS);

	ResourceName screensImage{"ionic_tests:screens/image"};

	MainScreen screen{texture_blurred.value()};
	screens.registerScreen(screensImage, &screen);
	screens.setActiveScreen(screensImage);

	getGraphicsManager()->init();

	screens.attachWindow(window);

	screens.startLoop();

	ionicengine::shutdown();

	return EXIT_SUCCESS;
}