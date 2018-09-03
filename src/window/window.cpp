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

	GLFWwindow *Window::getHandle() const
	{
		return _pointer;
	}

	void Window::setTitle(const std::string &title)
	{
		glfwSetWindowTitle(_pointer, title.c_str());
	}

	void Window::setSize(uint32_t width, uint32_t height)
	{
		glfwSetWindowSize(_pointer, width, height);
	}

	std::pair<uint32_t, uint32_t> Window::getSize() const
	{
		int width, height;
		glfwGetWindowSize(_pointer, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	std::pair<uint32_t, uint32_t> Window::getFramebufferSize() const
	{
		int width, height;
		glfwGetFramebufferSize(_pointer, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	std::pair<int, int> Window::getPosition() const
	{
		int x, y;
		glfwGetWindowPos(_pointer, &x, &y);
		return {x, y};
	}

	std::pair<double, double> Window::getCursorPosition() const
	{
		double x, y;
		glfwGetCursorPos(_pointer, &x, &y);
		return {x, y};
	}

	void Window::setPosition(int x, int y)
	{
		glfwSetWindowPos(_pointer, x, y);
	}

	std::pair<float, float> Window::getContentScale()
	{
		float xscale, yscale;
		glfwGetWindowContentScale(_pointer, &xscale, &yscale);
		return {xscale, yscale};
	}

	std::optional<Monitor> Window::getMonitor() const
	{
		auto glfwMonitor = glfwGetWindowMonitor(_pointer);
		if (glfwMonitor == nullptr)
			return {};
		for (auto monitor : monitor::getMonitors())
		{
			if (monitor.getHandle() == glfwMonitor)
				return {monitor};
		}
		return {};
	}

	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(_pointer) == GLFW_TRUE;
	}

	void Window::setShouldClose(bool shouldClose)
	{
		glfwSetWindowShouldClose(_pointer, shouldClose);
	}

	void Window::show() const
	{
		glfwShowWindow(_pointer);
	}

	void Window::hide() const
	{
		glfwHideWindow(_pointer);
	}

	void Window::setVisible(bool visible) const
	{
		if (visible) show(); else hide();
	}

	bool Window::isVisible() const
	{
		return glfwGetWindowAttrib(_pointer, GLFW_VISIBLE) == GLFW_TRUE;
	}

	void Window::focus() const
	{
		glfwFocusWindow(_pointer);
	}

	bool Window::isFocused() const
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

	void Window::setOpacity(float opacity)
	{
		glfwSetWindowOpacity(_pointer, opacity);
	}

	float Window::getOpacity()
	{
		return glfwGetWindowOpacity(_pointer);
	}

	void Window::requestAttention() const
	{
		glfwRequestWindowAttention(_pointer);
	}

	void Window::requestContext() const
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

		int toGLFWBool(bool aBool)
		{
			return aBool ? GLFW_TRUE : GLFW_FALSE;
		}

		Window createWindow(const std::string &title, std::pair<uint32_t, uint32_t> size, const WindowOptions &options,
							const std::optional<const Monitor> &monitor)
		{
			return createWindow(title, size.first, size.second, options, monitor);
		}

		Window createWindow(const std::string &title, uint32_t width, uint32_t height, const WindowOptions &options,
							const std::optional<const Monitor> &monitor)
		{
			glfwWindowHint(GLFW_VISIBLE, toGLFWBool(options.visible));
			glfwWindowHint(GLFW_FOCUSED, toGLFWBool(options.focused));
			glfwWindowHint(GLFW_RESIZABLE, toGLFWBool(options.resizable));
			glfwWindowHint(GLFW_MAXIMIZED, toGLFWBool(options.maximized));
			glfwWindowHint(GLFW_DECORATED, toGLFWBool(options.decorated));
			glfwWindowHint(GLFW_FLOATING, toGLFWBool(options.floating));
			glfwWindowHint(GLFW_AUTO_ICONIFY, toGLFWBool(options.auto_iconify));
			glfwWindowHint(GLFW_CENTER_CURSOR, toGLFWBool(options.center_cursor));
			glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, toGLFWBool(options.transparent_framebuffer));
			// OpenGL
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, options.context_version_major);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, options.context_version_minor);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, options.opengl_forward_compat);
			glfwWindowHint(GLFW_OPENGL_PROFILE, options.opengl_profile);
			glfwWindowHint(GLFW_SAMPLES, options.samples);

			if (monitor.has_value())
			{
				auto vidMode = monitor->getVideoMode();
				glfwWindowHint(GLFW_RED_BITS, vidMode.redBits);
				glfwWindowHint(GLFW_GREEN_BITS, vidMode.greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, vidMode.blueBits);
				glfwWindowHint(GLFW_REFRESH_RATE, vidMode.refreshRate);
				GLFWwindow *window = glfwCreateWindow(vidMode.width, vidMode.height, title.c_str(),
													  monitor->getHandle(), nullptr);
				Window ionicWindow{window};
				windows.push_back(ionicWindow);
				InputManager::INPUT_MANAGER.attachWindow(ionicWindow);
				return ionicWindow;
			}
			else
			{
				GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
				Window ionicWindow{window};
				windows.push_back(ionicWindow);
				InputManager::INPUT_MANAGER.attachWindow(ionicWindow);
				return ionicWindow;
			}
		}

		std::optional<Window> IONICENGINE_API getByHandle(GLFWwindow *window)
		{
			for (const Window &ionicWindow : windows)
				if (ionicWindow.getHandle() == window)
					return {ionicWindow};
			return std::nullopt;
		}

		void IONICENGINE_API destroyAll()
		{
			for (auto window : windows)
				window.destroy();
			windows.clear();
		}
	}
}