/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_WINDOW_H
#define IONICENGINE_WINDOW_H

#include "monitor.h"
#include <optional>

namespace ionicengine
{
	struct WindowOptions
	{
		// Window
		bool visible = true;
		bool focused = true;
		bool resizable = true;
		bool maximized = false;
		bool decorated = true;
		bool floating = false;
		bool auto_iconify = true;
		bool center_cursor = true;
		bool transparent_framebuffer = false;

		// OpenGL
		uint16_t context_version_major = 1;
		uint16_t context_version_minor = 0;
		bool opengl_forward_compat = false;
		int opengl_profile = GLFW_OPENGL_ANY_PROFILE;
		int8_t samples = GLFW_DONT_CARE;
	};

	const WindowOptions DEFAULT_WINDOW_OPTIONS{};

	using namespace std::rel_ops;

	class IONICENGINE_API Window
	{
	private:
		GLFWwindow *_pointer;

	public:
		Window(GLFWwindow *glfwWindow);

		GLFWwindow *getHandle() const;

		void setTitle(const std::string &title);

		void setSize(uint32_t width, uint32_t height);

		std::pair<uint32_t, uint32_t> getSize() const;

		std::pair<uint32_t, uint32_t> getFramebufferSize() const;

		std::pair<int, int> getPosition() const;

		std::pair<double, double> getCursorPosition() const;

		void setPosition(int x, int y);

		std::pair<float, float> getContentScale();

		std::optional<Monitor> getMonitor() const;

		bool shouldClose() const;

		void setShouldClose(bool shouldClose);

		void show() const;

		void hide() const;

		void setVisible(bool visible) const;

		bool isVisible() const;

		void focus() const;

		bool isFocused() const;

		void iconify() const;

		void maximize() const;

		void restore() const;

		void setOpacity(float opacity);

		float getOpacity();

		void requestAttention() const;

		void requestContext() const;

		bool initContext() const;

		void destroy();

		bool operator==(const Window &window) const;

		bool operator<(const Window &window) const;

		explicit operator bool();
	};

	namespace window
	{
		extern Window IONICENGINE_API createWindow(const std::string &title, std::pair<uint32_t, uint32_t> size,
												   const WindowOptions &options = DEFAULT_WINDOW_OPTIONS,
												   const std::optional<const Monitor> &monitor = std::nullopt);

		extern Window IONICENGINE_API createWindow(const std::string &title, uint32_t width, uint32_t height,
												   const WindowOptions &options = DEFAULT_WINDOW_OPTIONS,
												   const std::optional<const Monitor> &monitor = std::nullopt);

		extern std::optional<Window> IONICENGINE_API getByHandle(GLFWwindow *window);

		extern void IONICENGINE_API destroyAll();
	}
}

#endif //IONICENGINE_WINDOW_H
