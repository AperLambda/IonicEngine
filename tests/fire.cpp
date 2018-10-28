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
#include <ionicengine/sound/wav.h>
#include <lambdacommon/system/terminal.h>

using namespace ionicengine;
using namespace lambdacommon;

BitmapAnimation *fire_animation = nullptr, *cat_animation = nullptr;
TexturesAnimation *fire_light_animation = nullptr;

bool sounds_state = true;

class KeyboardListenerImpl : public KeyboardListener
{
public:
	void on_key_input(Window &window, int key, int scancode, InputAction action, int mods) override
	{
		if (action == InputAction::PRESS)
		{
			if (key == GLFW_KEY_ESCAPE)
				window.set_should_close(true);
			else if (key == GLFW_KEY_SPACE)
			{
				if (fire_animation->is_running())
					fire_animation->stop();
				else
					fire_animation->start();
				if (fire_light_animation->is_running())
					fire_light_animation->stop();
				else
					fire_light_animation->start();
				if (cat_animation->is_running())
					cat_animation->stop();
				else
					cat_animation->start();

				sounds_state = !sounds_state;
			}
		}
	}

	void on_char_input(Window &window, char32_t codepoint) override
	{}
};

class MainScreen : public Screen
{
private:
	Color grass_color = color::mix(Color::COLOR_GREEN, Color::COLOR_BLACK, 0.5f);
	uint32_t fire_x = 0, fire_y = 0, quad_y = 0;
	int fire_place = 0, crickets = 0;

public:
	MainScreen()
	{
		set_background_color(Color::COLOR_BLUE);
		fire_place = sound::play({"ionic_tests:sounds/fireplace"}, true);
		crickets = sound::play({"ionic_tests:sounds/129678__freethinkeranon__crickets"}, true);
	}

	void init() override
	{
		fire_x = width / 2 - 256;
		fire_y = height / 2 - 128;
		quad_y = height / 2 + 128;
	}

	void draw(Graphics *graphics) override
	{
		graphics->draw_image({"ionicengine:textures/sky/night01"}, 0, 0, width, height);
		graphics->set_color(grass_color);
		graphics->draw_quad(0, quad_y, width, height - quad_y);
		graphics->set_color(Color::COLOR_WHITE);
		fire_animation->set_x(fire_x);
		fire_animation->set_y(fire_y);
		fire_animation->render(graphics);
		fire_light_animation->set_x(fire_x);
		fire_light_animation->set_y(fire_y + 256);
		fire_light_animation->render(graphics);
		cat_animation->set_x(width / 2 + 32);
		cat_animation->set_y(height / 2 - 128 + 32);
		cat_animation->render(graphics);

		Screen::draw(graphics);
	}

	void update() override
	{
		Screen::update();
		fire_animation->update();
		fire_light_animation->update();
		cat_animation->update();
		if (sounds_state)
			sound::resume_all();
		else
			sound::pause_all();
	}
};

int main()
{
	terminal::setup();
	std::cout << "Running ionic_image with IonicEngine v" + ionicengine::get_version() << "...\n";

	IonicOptions ionic_options;
	ionic_options.use_controllers = false;
	ionic_options.debug = true;
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

	auto window = window::create_window("IonicEngine - Fire and Cat Animation", 948, 648, options);
	window.request_context();
	if (!ionicengine::post_init())
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	auto sky = texture::load({"ionicengine:textures/sky/night01"}, "png", CLAMP, LINEAR);
	auto fire = texture::load({"ionic_tests:textures/fire_animation"}, "png", CLAMP, NEAREST);
	//auto fireLight = texture::load({"ionic_tests:textures/fire_light_animation"}, "png", CLAMP, NEAREST);
	auto fire_light_01 = texture::load({"ionic_tests:textures/fire_light_animation01"}, "png", CLAMP, NEAREST);
	auto fire_light_02 = texture::load({"ionic_tests:textures/fire_light_animation02"}, "png", CLAMP, NEAREST);
	auto fire_light_03 = texture::load({"ionic_tests:textures/fire_light_animation03"}, "png", CLAMP, NEAREST);
	auto fire_light_04 = texture::load({"ionic_tests:textures/fire_light_animation04"}, "png", CLAMP, NEAREST);
	auto cat = texture::load({"ionic_tests:textures/cat"}, "png", CLAMP,
							 NEAREST);
	if (!sky || !fire || !fire_light_01 || !fire_light_02 || !fire_light_03 || !fire_light_04 || !cat)
	{
		std::cerr << "Cannot load textures!\n";
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	auto bitmap_animation = new BitmapAnimation{0, 0, 256, 256, fire.value(), 256};
	bitmap_animation->set_frame_time(4);
	fire_animation = bitmap_animation;

	auto textures_animation = new TexturesAnimation{0, 0, 256, 256,
												   {fire_light_01.value(), fire_light_03.value(), fire_light_02.value(),
													fire_light_04.value(), fire_light_01.value(), fire_light_04.value(),
													fire_light_02.value(), fire_light_03.value()}};
	textures_animation->set_frame_time(2);
	fire_light_animation = textures_animation;

	bitmap_animation = new BitmapAnimation{0, 0, 256, 256, cat.value(), 32};
	bitmap_animation->set_frame_time(13);
	bitmap_animation->set_invert_on_repeat(true);
	cat_animation = bitmap_animation;

	auto font = ionicengine::get_font_manager()->load_font({"google:fonts/roboto"}, std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	if (sound::wav::load({"ionic_tests", "sounds/fireplace"}) < 0 ||
		sound::wav::load({"ionic_tests", "sounds/129678__freethinkeranon__crickets"}) < 0)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	OverlayFPS overlay{font.value()};
	screens.register_overlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screens.add_active_overlay(IONICENGINE_OVERLAYS_FPS);

	ResourceName screens_image{"ionic_tests:screens/image"};

	MainScreen screen{};
	screens.register_screen(screens_image, &screen);
	screens.set_active_screen(screens_image);

	get_graphics_manager()->init();

	screens.attach_window(window);

	screens.start_loop();

	delete fire_animation;
	delete fire_light_animation;
	delete cat_animation;
	ionicengine::shutdown();

	return 0;
}