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
	GuiProgressBar *progress_bar = nullptr;

public:
	explicit MainScreen(const Font &font) : _font(font)
	{
		set_background_color(color::from_hex(0xEEEEEEFF));
	}

	void init() override
	{
		progress_bar = new GuiProgressBar(5, 10 + _font.get_height(), 300, 15);
		progress_bar->set_color(color::from_hex(0x26A1DCFF));
		components.push_back(progress_bar);
		auto button = new GuiButton(5, 20 + 15 + _font.get_height(), 150, 24, "Quit");
		button->set_font(&_font);
		button->set_activate_listener([](Window &window) { window.set_should_close(true); });
		components.push_back(button);
	}

	void draw(Graphics *graphics) override
	{
		graphics->set_color(Color::COLOR_BLACK);
		graphics->draw_text(_font, 5, 5,
							"Memory: " + std::to_string(system::get_memory_used() / 1073741824.0) + "GB / " +
							std::to_string(system::get_memory_total() / 1073741824.0) + "GB (" +
							std::to_string(progress_bar->get_progress()) + "%)");
		Screen::draw(graphics);
	}

	void update() override
	{
		Screen::update();
		float usage = static_cast<float>(system::get_memory_used()) / system::get_memory_total();
		progress_bar->set_progress(static_cast<uint32_t>(usage * 100));
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

	ScreenManager screens{};

	WindowOptions options{};
	options.context_version_major = 3;
	options.context_version_minor = 3;
	options.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
#ifdef LAMBDA_MAC_OSX
	options.opengl_forward_compat = true;
#endif

	auto window = window::create_window("IonicEngine - Basic resources monitor", 310, 80, options);
	window.request_context();
	if (!ionicengine::post_init())
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	auto font = ionicengine::get_font_manager()->load_font({"google:fonts/roboto"}, std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	ResourceName screens_image{"ionic_tests:screens/resources_monitor"};

	MainScreen screen{font.value()};
	screens.register_screen(screens_image, &screen);
	screens.set_active_screen(screens_image);

	get_graphics_manager()->init();

	screens.attach_window(window);

	screens.start_loop();

	ionicengine::shutdown();

	return EXIT_SUCCESS;
}