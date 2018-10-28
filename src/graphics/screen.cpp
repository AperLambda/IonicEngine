/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/screen.h"
#include <algorithm>

namespace ionicengine
{
	uint32_t last_screen_manager_id{0};

	/*
	 * SCREEN
	 */

	void Screen::draw(Graphics *graphics)
	{
		for (GuiComponent *component : components)
			component->draw(graphics);
	}

	void Screen::update()
	{
		for (GuiComponent *component : components)
			component->update();
	}

	void Screen::refresh(uint32_t width, uint32_t height)
	{
		this->width = width;
		this->height = height;
		for (GuiComponent *component : components)
			delete component;
		components.clear();
	}

	GuiComponent *Screen::get_focused_component() const
	{
		if (focus == -1 || focus >= static_cast<int>(components.size()))
			return nullptr;
		return components[focus];
	}

	void Screen::change_focused_component(bool forward)
	{
		if (components.empty())
		{
			focus = -1;
			return;
		}
		else
		{
			bool has_focusable = false;
			for (GuiComponent *component : components)
			{
				if (component->is_focusable())
				{
					has_focusable = true;
					break;
				}
			}
			if (!has_focusable)
				return;
		}

		GuiComponent *old = get_focused_component();
		if (old != nullptr)
			old->set_hovered(false);

		if (forward) focus++; else focus--;
		focus = lambdacommon::maths::clamp_reset(focus, 0, static_cast<int>(components.size() - 1));
		GuiComponent *new_component = get_focused_component();
		while (!new_component->is_focusable())
		{
			if (forward) focus++; else focus--;
			focus = lambdacommon::maths::clamp_reset(focus, 0, static_cast<int>(components.size() - 1));
			new_component = get_focused_component();
		}
		new_component->set_hovered(true);
	}

	bool Screen::on_mouse_move(int x, int y)
	{
		for (GuiComponent *component : components)
		{
			if (component->is_visible())
			{
				if (component->is_hovered())
				{
					if (graphics::is_mouse_in_box(x, y, component->get_x(), component->get_y(), component->width,
												  component->height))
						return true;
					else
						component->set_hovered(false);
				}
				else
				{
					if (graphics::is_mouse_in_box(x, y, component->get_x(), component->get_y(), component->width,
												  component->height))
					{
						component->set_hovered(true);
						component->on_hover();
						return true;
					}
				}
			}
		}
		return false;
	}

	bool Screen::on_mouse_pressed(Window &window, int button, int mouseX, int mouseY)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			GuiComponent *component = components[i];
			if (component->is_enabled() && component->is_visible() &&
				graphics::is_mouse_in_box(mouseX, mouseY, component->get_x(), component->get_y(), component->width,
										  component->height))
			{
				focus = static_cast<int32_t>(i);
				component->set_clicked(true);
				component->on_mouse_pressed(window, button, mouseX, mouseY);
				return true;
			}
		}
		return false;
	}

	bool Screen::on_mouse_released(Window &window, int button, int mouseX, int mouseY)
	{
		for (GuiComponent *component : components)
		{
			if (component->is_enabled() && component->is_visible() && mouseX >= component->get_x() &&
				mouseY >= component->get_y() && mouseX < component->get_x() + static_cast<int>(component->width) &&
				mouseY < component->get_y() + static_cast<int>(component->height))
			{
				component->set_clicked(false);
				component->on_mouse_released(window, button, mouseX, mouseY);
				return true;
			}
		}
		return false;
	}

	bool Screen::on_key_input(Window &window, int key, int scancode, InputAction action, int mods)
	{
		if (!components.empty() && key == GLFW_KEY_TAB && action == InputAction::PRESS)
		{
			change_focused_component();
			return true;
		}

		GuiComponent *component = get_focused_component();
		if (component == nullptr)
			return false;
		if (component->is_enabled() && component->is_visible())
		{
			if (!component->does_disable_key_activate() && (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_SPACE))
				component->on_activate(window);
			else
				component->on_key_input(window, key, scancode, action, mods);
			return true;
		}
		return false;
	}

	bool Screen::on_gamepad_button_input(Window &window, InputAction action, uint8_t button)
	{
		if (!components.empty() && (action == InputAction::PRESS || action == InputAction::REPEAT))
		{
			if (button == GLFW_GAMEPAD_BUTTON_DPAD_DOWN)
			{
				change_focused_component();
				return true;
			}
			else if (button == GLFW_GAMEPAD_BUTTON_DPAD_UP)
			{
				change_focused_component(false);
				return true;
			}
		}

		GuiComponent *component = get_focused_component();
		if (component == nullptr)
			return false;
		if (component->is_enabled() && component->is_visible())
		{
			if (!component->does_disable_key_activate() && button == GLFW_GAMEPAD_BUTTON_A && action == InputAction::PRESS)
				component->on_activate(window);
			else
				component->on_gamepad_button_input(window, action, button);
			return true;
		}

		return false;
	}

	/*
	 * NULLSCREEN
	 */

	class NullScreen : public Screen
	{
	public:
		void init() override
		{}

		void draw(Graphics *graphics) override
		{
			// Draws nothing it's a null screen.
		}

		void update() override
		{}
	};

	/*
	 * OVERLAY
	 */

	lambdacommon::Color Overlay::get_background_color() const
	{
		return lambdacommon::Color(.0f, .0f, .0f, .0f);
	}

	/*
	 * OVERLAY FPS
	 */

	OverlayFPS::OverlayFPS(const Font &font) : Overlay(), _font(font)
	{}

	void OverlayFPS::init()
	{}

	void OverlayFPS::draw(Graphics *graphics)
	{
		graphics->set_color(lambdacommon::Color::COLOR_WHITE);
		graphics->draw_text(_font, 2, 3, "FPS: " + std::to_string(_fps));
	}

	void OverlayFPS::update()
	{}

	void OverlayFPS::updateFPS(int fps)
	{
		_fps = fps;
	}

	/*
	 * SCREENMANAGER
	 */

	ScreenManager::ScreenManager()
	{
		_id = last_screen_manager_id;
		last_screen_manager_id++;
		lambdacommon::ResourceName nullScreen{"ionicengine", "screens/null"};
		register_screen(nullScreen, new NullScreen());
		_active_screen = nullScreen;
	}

	ScreenManager::~ScreenManager()
	{
		delete graphics;
	}

	void ScreenManager::register_screen(const lambdacommon::ResourceName &name, Screen *screen)
	{
		if (_screens.count(name))
			throw std::invalid_argument(
					"ScreenManager has already registered the screen with name '" + name.to_string() + "'!");
		_screens[name] = screen;
	}

	bool ScreenManager::has_screen(const lambdacommon::ResourceName &name) const
	{
		return static_cast<bool>(_screens.count(name));
	}

	Screen *ScreenManager::get_screen(const lambdacommon::ResourceName &name) const
	{
		if (!has_screen(name))
			return nullptr;
		return _screens.at(name);
	}

	void ScreenManager::register_overlay(const lambdacommon::ResourceName &name, Overlay *overlay)
	{
		if (_overlays.count(name))
			throw std::invalid_argument(
					"GraphicsManager has already registered the overlay with name '" + name.to_string() + "'!");
		_overlays[name] = overlay;
	}

	bool ScreenManager::has_overlay(const lambdacommon::ResourceName &name) const
	{
		return static_cast<bool>(_overlays.count(name));
	}

	Overlay *ScreenManager::get_overlay(const lambdacommon::ResourceName &name) const
	{
		if (!has_overlay(name))
			return nullptr;
		return _overlays.at(name);
	}

	Screen *ScreenManager::get_active_screen() const
	{
		return _screens.at(_active_screen);
	}

	void ScreenManager::set_active_screen(const lambdacommon::ResourceName &name)
	{
		_active_screen = name;
		auto screen = get_active_screen();
		if (screen != nullptr)
		{
			screen->refresh(old_framebuffer_size.get_width(), old_framebuffer_size.get_height());
			screen->init();
		}
	}

	std::vector<lambdacommon::ResourceName> ScreenManager::get_active_overlays() const
	{
		return _active_overlays;
	}

	void ScreenManager::add_active_overlay(const lambdacommon::ResourceName &name)
	{
		if (!is_overlay_active(name))
		{
			_active_overlays.emplace_back(name);
			auto overlay = _overlays.at(name);
			if (overlay != nullptr)
			{
				overlay->refresh(old_framebuffer_size.get_width(), old_framebuffer_size.get_height());
				overlay->init();
			}
		}
	}

	bool ScreenManager::is_overlay_active(const lambdacommon::ResourceName &name)
	{
		return std::find(_active_overlays.begin(), _active_overlays.end(), name) != _active_overlays.end();
	}

	void ScreenManager::remove_active_overlay(const lambdacommon::ResourceName &name)
	{
		if (is_overlay_active(name))
			_active_overlays.erase(std::find(_active_overlays.begin(), _active_overlays.end(), name));
	}

	std::optional<Window> ScreenManager::get_attached_window() const
	{
		return _window;
	}

	int ScreenManager::get_fps() const
	{
		return fps;
	}

	int ScreenManager::get_updates() const
	{
		return updates;
	}

	float ScreenManager::get_delta_time() const
	{
		return delta_time;
	}

	void ScreenManager::set_delta_time(float delta_time)
	{
		ScreenManager::delta_time = delta_time;
	}

	bool ScreenManager::on_mouse_move(int x, int y)
	{
		if (!_window)
			return false;

		for (const auto &active_overlay : _active_overlays)
		{
			if (has_overlay(active_overlay))
			{
				Overlay *overlay = get_overlay(active_overlay);
				if (overlay->on_mouse_move(x, y))
					return true;
			}
		}

		auto screen = get_active_screen();
		return screen->on_mouse_move(x, y);
	}

	bool ScreenManager::on_mouse_button(int button, InputAction action, int mods)
	{
		if (!_window)
			return false;

		auto cursor_position = _window->get_cursor_position();

		for (const auto &active_overlay : _active_overlays)
		{
			if (has_overlay(active_overlay))
			{
				Overlay *overlay = get_overlay(active_overlay);
				if (action == InputAction::PRESS)
				{
					if (overlay->on_mouse_pressed(_window.value(), button, static_cast<int>(cursor_position.first),
												  static_cast<int>(cursor_position.second)))
						return true;
				}
				else if (action == InputAction::RELEASE)
				{
					if (overlay->on_mouse_released(_window.value(), button, static_cast<int>(cursor_position.first),
												   static_cast<int>(cursor_position.second)))
						return true;
				}
			}
		}

		auto screen = get_active_screen();
		if (action == InputAction::PRESS)
		{
			if (screen->on_mouse_pressed(_window.value(), button, static_cast<int>(cursor_position.first),
										 static_cast<int>(cursor_position.second)))
				return true;
		}
		else if (action == InputAction::RELEASE)
		{
			if (screen->on_mouse_released(_window.value(), button, static_cast<int>(cursor_position.first),
										  static_cast<int>(cursor_position.second)))
				return true;
		}

		return false;
	}

	bool ScreenManager::on_key_input(int key, int scancode, InputAction action, int mods)
	{
		if (!_window)
			return false;

		for (const auto &active_overlay : _active_overlays)
		{
			if (has_overlay(active_overlay))
			{
				if (get_overlay(active_overlay)->on_key_input(_window.value(), key, scancode, action, mods))
					return true;
			}
		}

		return get_active_screen()->on_key_input(_window.value(), key, scancode, action, mods);
	}

	bool ScreenManager::on_gamepad_button_input(InputAction action, uint8_t button)
	{
		if (!_window)
			return false;

		for (const auto &active_overlay : _active_overlays)
		{
			if (has_overlay(active_overlay))
			{
				if (get_overlay(active_overlay)->on_gamepad_button_input(_window.value(), action, button))
					return true;
			}
		}

		return get_active_screen()->on_gamepad_button_input(_window.value(), action, button);
	}

	void ScreenManager::render()
	{
		if (_window)
		{
			_window->request_context();
			auto screen = get_active_screen();
			auto background_color = lambdacommon::Color::COLOR_BLACK;
			if (screen != nullptr)
				background_color = screen->get_background_color();
			glClearColor(background_color.red(), background_color.green(), background_color.blue(),
						 background_color.alpha());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (screen != nullptr)
			{
				screen->draw(graphics);
				graphics->reset_transform();
			}
			for (const auto &active_overlay : _active_overlays)
			{
				if (has_overlay(active_overlay))
				{
					Overlay *overlay = get_overlay(active_overlay);
					overlay->draw(graphics);
					graphics->reset_transform();
				}
			}
		}
	}

	void ScreenManager::update()
	{
		auto screen = get_active_screen();
		if (screen != nullptr)
			screen->update();
		for (const auto &active_overlay : _active_overlays)
		{
			if (_overlays.count(active_overlay))
			{
				Overlay *overlay = _overlays.at(active_overlay);
				overlay->update();
			}
		}
	}

	void ScreenManager::start()
	{
		if (!_window)
			throw std::runtime_error("Cannot start loop without a Window");

		auto size = _window->get_framebuffer_size();
		graphics = get_graphics_manager()->new_graphics(size);
		old_framebuffer_size = {0, 0};
	}

	void ScreenManager::start_loop()
	{
		start();

		double lastTime = glfwGetTime(), timer = lastTime;
		double deltaTime = 0, nowTime = 0;
		int frames = 0, updates = 0;

		glfwSetFramebufferSizeCallback(_window->get_handle(), [](GLFWwindow *window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});

		while (!_window->should_close())
		{
			auto current_size = _window->get_size();
			if (old_framebuffer_size != current_size)
			{
				graphics->update_framebuffer_size(current_size.get_width(), current_size.get_height());
				old_framebuffer_size = current_size;
				auto screen = get_active_screen();
				if (screen != nullptr)
				{
					screen->refresh(current_size.get_width(), current_size.get_height());
					screen->init();
				}
				for (const auto &active_overlay : _active_overlays)
				{
					if (has_overlay(active_overlay))
					{
						Overlay *overlay = get_overlay(active_overlay);
						overlay->refresh(current_size.get_width(), current_size.get_height());
						overlay->init();
					}
				}
			}

			this->render();

			nowTime = glfwGetTime();
			deltaTime += (nowTime - lastTime);
			lastTime = nowTime;

			this->delta_time = static_cast<float>(deltaTime);

			if (deltaTime >= 0.020)
			{
				this->update();
				updates++;
				deltaTime = 0;
			}
			frames++;

			glfwSwapBuffers(_window->get_handle());
			glfwPollEvents();

			// - Reset after one second
			if (glfwGetTime() - timer > 1.0)
			{
				timer++;
				this->fps = frames;
				this->updates = updates;
				if (has_overlay(IONICENGINE_OVERLAYS_FPS))
					((OverlayFPS *) get_overlay(IONICENGINE_OVERLAYS_FPS))->updateFPS(frames);
				updates = 0, frames = 0;
			}
		}

		LCOMMON_DELETE_POINTER(graphics);

		_window->destroy();
	}

	bool ScreenManager::operator==(const ScreenManager &other) const
	{
		return _id == other._id;
	}

	bool ScreenManager::operator<(const ScreenManager &other) const
	{
		return _id < other._id;
	}

	namespace screen
	{
		std::map<Window, ScreenManager *> screen_managers;

		ScreenManager *get_screen_manager(const Window &window)
		{
			return screen_managers.at(window);
		}
	}

	void ScreenManager::attach_window(const Window &window)
	{
		_window = {window};
		screen::screen_managers.insert({window, this});
	}
}