/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <ionicengine/graphics/screen.h>
#include <lambdacommon/system/system.h>

using namespace lambdacommon;
using namespace ionicengine;

class MainScreen : public Screen
{
private:
	Texture texture;

public:
	MainScreen(const Texture &texture) : texture(texture)
	{}

	void draw(Graphics *graphics) override
	{
		float ratio5Width = .10f * graphics->getWidth(), ratio5Height = .10f * graphics->getHeight();
		auto textureWidth = static_cast<float>(graphics->getWidth()), textureHeight = static_cast<float>(graphics->getHeight());
		float quadWidth = textureWidth - (ratio5Width * 2), quadHeight = textureHeight - (ratio5Height * 2);
		graphics->setColor(Color::COLOR_WHITE);
		graphics->drawImage({"ionic_tests:textures/conifer-dark-green-daylight-572937"}, 0, 0, textureWidth,
							textureHeight);
		graphics->drawImage(texture, ratio5Width, ratio5Height, quadWidth, quadHeight, texture::newTextureRegion(
				static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight),
				static_cast<uint32_t>(ratio5Width), static_cast<uint32_t>(ratio5Height),
				static_cast<uint32_t>(quadWidth), static_cast<uint32_t>(quadHeight)));
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

	if (!ionicengine::init(true))
		return EXIT_FAILURE;

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
	glfwSwapInterval(0);
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

	MainScreen screen{texture_blurred.value()};
	screens.registerScreen(screensImage, &screen);
	screens.setActiveScreen(screensImage);

	getGraphicsManager()->init();

	screens.attachWindow(window);

	screens.startLoop();

	ionicengine::shutdown();

	return EXIT_SUCCESS;
}