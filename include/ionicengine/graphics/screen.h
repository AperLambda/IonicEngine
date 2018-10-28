/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_SCREEN_H
#define IONICENGINE_SCREEN_H

#include "gui.h"
#include "../window/window.h"
#include <thread>

namespace ionicengine
{
	class IONICENGINE_API Screen : public Gui
	{
	protected:
		std::vector<GuiComponent *> components;
		int32_t focus = -1;

	public:
		void draw(Graphics *graphics) override;

		void update() override;

		void refresh(uint32_t width, uint32_t height);

		GuiComponent *get_focused_component() const;

		void change_focused_component(bool forward = true);

		virtual bool on_mouse_move(int x, int y);

		virtual bool on_mouse_pressed(Window &window, int button, int mouseX, int mouseY);

		virtual bool on_mouse_released(Window &window, int button, int mouseX, int mouseY);

		virtual bool on_key_input(Window &window, int key, int scancode, InputAction action, int mods);

		virtual bool on_gamepad_button_input(Window &window, InputAction action, uint8_t button);
	};

	class IONICENGINE_API Overlay : public Screen
	{
		lambdacommon::Color get_background_color() const override;
	};

	class IONICENGINE_API OverlayFPS : public Overlay
	{
	private:
		Font _font;
		int _fps{0};

	public:
		explicit OverlayFPS(const Font &font);

		void init() override;

		void draw(Graphics *graphics) override;

		void update() override;

		void updateFPS(int fps);
	};

	using namespace std::rel_ops;

	class IONICENGINE_API ScreenManager
	{
	private:
		uint32_t _id;
		std::map<lambdacommon::ResourceName, Screen *> _screens;
		std::map<lambdacommon::ResourceName, Overlay *> _overlays;
		lambdacommon::ResourceName _active_screen = IONICENGINE_NULL_RESOURCE;
		std::vector<lambdacommon::ResourceName> _active_overlays;

		std::optional<Window> _window;
		Dimension2D_u32 old_framebuffer_size{0, 0};
		Graphics *graphics;

		int fps{0}, updates{0};
		float delta_time{0.f};

	public:
		ScreenManager();

		~ScreenManager();

		void register_screen(const lambdacommon::ResourceName &name, Screen *screen);

		bool has_screen(const lambdacommon::ResourceName &name) const;

		Screen *get_screen(const lambdacommon::ResourceName &name) const;

		void register_overlay(const lambdacommon::ResourceName &name, Overlay *overlay);

		bool has_overlay(const lambdacommon::ResourceName &name) const;

		Overlay *get_overlay(const lambdacommon::ResourceName &name) const;

		Screen *get_active_screen() const;

		void set_active_screen(const lambdacommon::ResourceName &name);

		std::vector<lambdacommon::ResourceName> get_active_overlays() const;

		void add_active_overlay(const lambdacommon::ResourceName &name);

		bool is_overlay_active(const lambdacommon::ResourceName &name);

		void remove_active_overlay(const lambdacommon::ResourceName &name);

		void attach_window(const Window &window);

		std::optional<Window> get_attached_window() const;

		int get_fps() const;

		int get_updates() const;

		void set_delta_time(float delta_time);

		float get_delta_time() const;

		bool on_mouse_move(int x, int y);

		bool on_mouse_button(int button, InputAction action, int mods);

		bool on_key_input(int key, int scancode, InputAction action, int mods);

		bool on_gamepad_button_input(InputAction action, uint8_t button);

		void render();

		void update();

		void start();

		void start_loop();

		bool operator==(const ScreenManager &other) const;

		bool operator<(const ScreenManager &other) const;
	};

	namespace screen
	{
		/*!
		 * Gets the attached ScreenManager of the specified Window.
		 * @param window The Window.
		 * @return The pointer of the ScreenManager to get.
		 */
		extern ScreenManager * IONICENGINE_API get_screen_manager(const Window &window);
	}
}

#endif //IONICENGINE_SCREEN_H