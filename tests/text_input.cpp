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
#include <lambdacommon/maths.h>
#include <cuchar>
#include <clocale>

using namespace lambdacommon;
using namespace ionicengine;

std::string textArea{""};

class KeyboardListenerImpl : public KeyboardListener
{
public:
	void onKeyInput(Window &window, int key, int scancode, InputAction action, int mods) override
	{
		if (action == InputAction::PRESS)
		{
			if (key == GLFW_KEY_ESCAPE)
				window.setShouldClose(true);
			else if (key == GLFW_KEY_BACKSPACE)
				textArea = textArea.substr(0, textArea.length() - 1);
			else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER)
				textArea += "\n";
			else if (key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL)
			{
				glfwSetClipboardString(window.getHandle(), textArea.c_str());
			}
		}
		else if (action == InputAction::REPEAT)
		{
			if (key == GLFW_KEY_BACKSPACE)
				textArea = textArea.substr(0, textArea.length() - 1);
		}
	}

	void onCharInput(Window &window, char32_t codepoint) override
	{
		textArea += static_cast<char>(codepoint);
	}
};

class TextArea : public Screen
{
private:
	Font font;
	Texture texture;
	bool increaseOpacity = true;
	float opacity = 0.f;

public:
	TextArea(const Font &font, const Texture &texture) : font(font), texture(texture)
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
				textureWidth, textureHeight, ratio5Width, ratio5Height, quadWidth, quadHeight));
		graphics->setColor({0.f, 0.f, 0.f, .25f});
		graphics->drawQuad(ratio5Width, ratio5Height, quadWidth, quadHeight);
		graphics->setColor(Color::COLOR_BLACK);
		graphics->drawLine2D(ratio5Width, ratio5Height, ratio5Width + quadWidth, ratio5Height);
		graphics->drawLine2D(ratio5Width, ratio5Height, ratio5Width, ratio5Height + quadHeight);
		graphics->drawLine2D(ratio5Width, ratio5Height + quadHeight, ratio5Width + quadWidth,
							 ratio5Height + quadHeight);
		graphics->drawLine2D(ratio5Width + quadWidth, ratio5Height, ratio5Width + quadWidth, ratio5Height + quadHeight);

		graphics->setColor(Color::COLOR_WHITE);
		graphics->drawText(font, ratio5Width + 5.f, ratio5Height + 5.f, textArea, quadWidth - 10.f, quadHeight - 10.f);
		auto splitted = lambdastring::split(textArea + "a", '\n');
		uint32_t textLength = 0;
		if (splitted.size() != 0)
			textLength = font.getTextLength(splitted[splitted.size() - 1]) - font.getTextLength("a");
		auto cursorX = maths::clamp(ratio5Width + 5 + textLength + 2, ratio5Width + 7, ratio5Width + quadWidth);
		auto cursorY = ratio5Height + 2.f + font.getHeight() * maths::clamp(static_cast<uint32_t>(splitted.size() - 1), static_cast<uint32_t>(0), quadHeight);
		graphics->setColor({1.f, 1.f, 1.f, opacity});
		graphics->drawLine2D(cursorX, static_cast<int>(cursorY), cursorX, static_cast<int>(cursorY + font.getHeight()));
	}

	void update() override
	{
		if (increaseOpacity)
		{
			if (opacity >= 1.f)
				increaseOpacity = false;
			else
			{
				opacity += .05f;
			}
		}
		else
		{
			if (opacity <= 0.f)
				increaseOpacity = true;
			else
			{
				opacity -= .05f;
			}
		}
	}
};

int main()
{
	std::setlocale(LC_ALL, "en_US.utf8");
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

	auto window = window::createWindow("IonicEngine - Input (Text Area)", 948, 648, options);
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

	auto font = ionicengine::getFontManager()->loadFont(std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	OverlayFPS overlay{font.value()};
	screens.registerOverlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screens.addActiveOverlay(IONICENGINE_OVERLAYS_FPS);

	ResourceName screensImage{"ionic_tests:screens/image"};

	TextArea screen{font.value(), texture_blurred.value()};
	screens.registerScreen(screensImage, &screen);
	screens.setActiveScreen(screensImage);

	getGraphicsManager()->init();

	screens.attachWindow(window);

	screens.startLoop();

	ionicengine::shutdown();

	return EXIT_SUCCESS;
}