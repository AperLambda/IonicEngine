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
	void on_key_input(Window &window, int key, int scancode, InputAction action, int mods) override
	{
		if (action == InputAction::PRESS)
		{
			if (key == GLFW_KEY_ESCAPE)
				window.set_should_close(true);
		}
	}

	void on_char_input(Window &window, char32_t codepoint) override
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
		auto ratio5_width = static_cast<int>(.10f * width), ratio5_height = static_cast<int>(.10f * height);
		auto texture_width = width, texture_height = height;
		uint32_t quad_width = texture_width - (ratio5_width * 2), qued_height = texture_height - (ratio5_height * 2);
		graphics->set_color(Color::COLOR_WHITE);
		graphics->draw_image({"ionic_tests:textures/conifer-dark-green-daylight-572937"}, 0, 0, texture_width,
							 texture_height);
		graphics->draw_image(texture, ratio5_width, ratio5_height, quad_width, qued_height, texture::new_texture_region(
				texture_width, texture_height,
				static_cast<uint32_t>(ratio5_width), static_cast<uint32_t>(ratio5_height),
				quad_width, qued_height));
		graphics->set_color({0.f, 0.f, 0.f, .25f});
		graphics->draw_quad(ratio5_width, ratio5_height, quad_width, qued_height);
		graphics->set_color(Color::COLOR_BLACK);
		graphics->draw_line_2d(ratio5_width, ratio5_height, ratio5_width + quad_width, ratio5_height);
		graphics->draw_line_2d(ratio5_width, ratio5_height, ratio5_width, ratio5_height + qued_height);
		graphics->draw_line_2d(ratio5_width, ratio5_height + qued_height, ratio5_width + quad_width,
							   ratio5_height + qued_height);
		graphics->draw_line_2d(ratio5_width + quad_width, ratio5_height, ratio5_width + quad_width,
							   ratio5_height + qued_height);
	}

	void update() override
	{}
};

int main()
{
	terminal::setup();
	std::cout << "Running ionic_image with IonicEngine v" + ionicengine::get_version() << "...\n";

	IonicOptions ionic_options;
	ionic_options.debug = true;
	ionic_options.use_controllers = false;
	if (!ionicengine::init(ionic_options))
		return EXIT_FAILURE;

	KeyboardListenerImpl keyboard_listener{};
	InputManager::INPUT_MANAGER.add_keyboard_listener(&keyboard_listener);

	ScreenManager screens{};

	WindowOptions options{};
	options.context_version_major = 3;
	options.context_version_minor = 3;
	options.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
#ifdef LAMBDA_MAC_OSX
	options.opengl_forward_compat = true;
#endif

	auto window = window::create_window("IonicEngine - Image", 948, 648, options);
	window.request_context();
	if (!ionicengine::post_init())
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
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

	auto font = ionicengine::get_font_manager()->load_font({"google:fonts/roboto"}, std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	OverlayFPS overlay{font.value()};
	screens.register_overlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screens.add_active_overlay(IONICENGINE_OVERLAYS_FPS);

	ResourceName screensImage{"ionic_tests:screens/image"};

	MainScreen screen{texture_blurred.value()};
	screens.register_screen(screensImage, &screen);
	screens.set_active_screen(screensImage);

	get_graphics_manager()->init();

	screens.attach_window(window);

	screens.start_loop();

	ionicengine::shutdown();

	return EXIT_SUCCESS;
}