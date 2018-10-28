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

		GLFWwindow *get_handle() const;

		void set_title(const std::string &title);

		void set_size(uint32_t width, uint32_t height);

		Dimension2D_u32 get_size() const;

		Dimension2D_u32 get_framebuffer_size() const;

		std::pair<int, int> get_position() const;

		std::pair<double, double> get_cursor_position() const;

		void set_position(int x, int y);

		std::pair<float, float> get_content_scale();

		std::optional<Monitor> get_monitor() const;

		bool should_close() const;

		void set_should_close(bool should_close);

		void show() const;

		void hide() const;

		void set_visible(bool visible) const;

		bool is_visible() const;

		void focus() const;

		bool is_focused() const;

		void iconify() const;

		void maximize() const;

		void restore() const;

		void set_opacity(float opacity);

		float get_opacity();

		void request_attention() const;

		void request_context() const;

		void destroy();

		bool operator==(const Window &window) const;

		bool operator<(const Window &window) const;

		explicit operator bool();
	};

	namespace window
	{
		extern Window IONICENGINE_API create_window(const std::string &title, std::pair<uint32_t, uint32_t> size,
													const WindowOptions &options = DEFAULT_WINDOW_OPTIONS,
													const std::optional<const Monitor> &monitor = std::nullopt);

		extern Window IONICENGINE_API create_window(const std::string &title, uint32_t width, uint32_t height,
													const WindowOptions &options = DEFAULT_WINDOW_OPTIONS,
													const std::optional<const Monitor> &monitor = std::nullopt);

		extern std::optional<Window> IONICENGINE_API get_by_handle(GLFWwindow *window);

		extern void IONICENGINE_API destroy_all();
	}
}

#endif //IONICENGINE_WINDOW_H
