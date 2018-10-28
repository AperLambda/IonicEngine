/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/window/window.h"
#include "../../include/ionicengine/input/inputmanager.h"

namespace ionicengine
{
	Window::Window(GLFWwindow *glfwWindow) : _pointer(glfwWindow)
	{}

	GLFWwindow *Window::get_handle() const
	{
		return _pointer;
	}

	void Window::set_title(const std::string &title)
	{
		glfwSetWindowTitle(_pointer, title.c_str());
	}

	void Window::set_size(uint32_t width, uint32_t height)
	{
		glfwSetWindowSize(_pointer, width, height);
	}

	Dimension2D_u32 Window::get_size() const
	{
		int width, height;
		glfwGetWindowSize(_pointer, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	Dimension2D_u32 Window::get_framebuffer_size() const
	{
		int width, height;
		glfwGetFramebufferSize(_pointer, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	std::pair<int, int> Window::get_position() const
	{
		int x, y;
		glfwGetWindowPos(_pointer, &x, &y);
		return {x, y};
	}

	std::pair<double, double> Window::get_cursor_position() const
	{
		double x, y;
		glfwGetCursorPos(_pointer, &x, &y);
		return {x, y};
	}

	void Window::set_position(int x, int y)
	{
		glfwSetWindowPos(_pointer, x, y);
	}

	std::pair<float, float> Window::get_content_scale()
	{
		float xscale, yscale;
		glfwGetWindowContentScale(_pointer, &xscale, &yscale);
		return {xscale, yscale};
	}

	std::optional<Monitor> Window::get_monitor() const
	{
		auto glfw_monitor = glfwGetWindowMonitor(_pointer);
		if (glfw_monitor == nullptr)
			return {};
		for (auto monitor : monitor::get_monitors())
		{
			if (monitor.get_handle() == glfw_monitor)
				return {monitor};
		}
		return {};
	}

	bool Window::should_close() const
	{
		return glfwWindowShouldClose(_pointer) == GLFW_TRUE;
	}

	void Window::set_should_close(bool should_close)
	{
		glfwSetWindowShouldClose(_pointer, should_close);
	}

	void Window::show() const
	{
		glfwShowWindow(_pointer);
	}

	void Window::hide() const
	{
		glfwHideWindow(_pointer);
	}

	void Window::set_visible(bool visible) const
	{
		if (visible) show(); else hide();
	}

	bool Window::is_visible() const
	{
		return glfwGetWindowAttrib(_pointer, GLFW_VISIBLE) == GLFW_TRUE;
	}

	void Window::focus() const
	{
		glfwFocusWindow(_pointer);
	}

	bool Window::is_focused() const
	{
		return glfwGetWindowAttrib(_pointer, GLFW_FOCUSED) == GLFW_TRUE;
	}

	void Window::iconify() const
	{
		glfwIconifyWindow(_pointer);
	}

	void Window::maximize() const
	{
		glfwMaximizeWindow(_pointer);
	}

	void Window::restore() const
	{
		glfwRestoreWindow(_pointer);
	}

	void Window::set_opacity(float opacity)
	{
		glfwSetWindowOpacity(_pointer, opacity);
	}

	float Window::get_opacity()
	{
		return glfwGetWindowOpacity(_pointer);
	}

	void Window::request_attention() const
	{
		glfwRequestWindowAttention(_pointer);
	}

	void Window::request_context() const
	{
		glfwMakeContextCurrent(_pointer);
	}

	void Window::destroy()
	{
		// I don't understand this.
		if (!*this)
			glfwDestroyWindow(_pointer);
		_pointer = nullptr;
	}

	bool Window::operator==(const Window &window) const
	{
		return _pointer == window._pointer;
	}

	bool Window::operator<(const Window &window) const
	{
		return _pointer < window._pointer;
	}

	Window::operator bool()
	{
		return _pointer != nullptr;
	}

	namespace window
	{
		std::vector<Window> windows;

		int to_glfw_bool(bool aBool)
		{
			return aBool ? GLFW_TRUE : GLFW_FALSE;
		}

		Window create_window(const std::string &title, std::pair<uint32_t, uint32_t> size, const WindowOptions &options,
							 const std::optional<const Monitor> &monitor)
		{
			return create_window(title, size.first, size.second, options, monitor);
		}

		Window create_window(const std::string &title, uint32_t width, uint32_t height, const WindowOptions &options,
							 const std::optional<const Monitor> &monitor)
		{
			glfwWindowHint(GLFW_VISIBLE, to_glfw_bool(options.visible));
			glfwWindowHint(GLFW_FOCUSED, to_glfw_bool(options.focused));
			glfwWindowHint(GLFW_RESIZABLE, to_glfw_bool(options.resizable));
			glfwWindowHint(GLFW_MAXIMIZED, to_glfw_bool(options.maximized));
			glfwWindowHint(GLFW_DECORATED, to_glfw_bool(options.decorated));
			glfwWindowHint(GLFW_FLOATING, to_glfw_bool(options.floating));
			glfwWindowHint(GLFW_AUTO_ICONIFY, to_glfw_bool(options.auto_iconify));
			glfwWindowHint(GLFW_CENTER_CURSOR, to_glfw_bool(options.center_cursor));
			glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, to_glfw_bool(options.transparent_framebuffer));
			// OpenGL
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.context_version_major);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.context_version_minor);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, options.opengl_forward_compat);
			glfwWindowHint(GLFW_OPENGL_PROFILE, options.opengl_profile);
			glfwWindowHint(GLFW_SAMPLES, options.samples);

			if (monitor.has_value())
			{
				auto vid_mode = monitor->get_video_mode();
				glfwWindowHint(GLFW_RED_BITS, vid_mode.red_bits);
				glfwWindowHint(GLFW_GREEN_BITS, vid_mode.green_bits);
				glfwWindowHint(GLFW_BLUE_BITS, vid_mode.blue_bits);
				glfwWindowHint(GLFW_REFRESH_RATE, vid_mode.refresh_rate);
				GLFWwindow *window = glfwCreateWindow(vid_mode.width, vid_mode.height, title.c_str(),
													  monitor->get_handle(), nullptr);
				Window ionic_window{window};
				windows.push_back(ionic_window);
				InputManager::INPUT_MANAGER.attach_window(ionic_window);
				return ionic_window;
			}
			else
			{
				GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
				Window ionic_window{window};
				windows.push_back(ionic_window);
				InputManager::INPUT_MANAGER.attach_window(ionic_window);
				return ionic_window;
			}
		}

		std::optional<Window> IONICENGINE_API get_by_handle(GLFWwindow *window)
		{
			for (const Window &ionic_window : windows)
				if (ionic_window.get_handle() == window)
					return {ionic_window};
			return std::nullopt;
		}

		void IONICENGINE_API destroy_all()
		{
			for (auto window : windows)
				window.destroy();
			windows.clear();
		}
	}
}