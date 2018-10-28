/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_GUI_H
#define IONICENGINE_GUI_H

#include "graphics.h"
#include "../input/inputmanager.h"

namespace ionicengine
{
	class IONICENGINE_API Gui
	{
	protected:
		lambdacommon::Color background_color = lambdacommon::Color::COLOR_BLACK;

	public:
		uint32_t width{}, height{};

		virtual ~Gui() = default;

		virtual void init() = 0;

		virtual void draw(Graphics *graphics) = 0;

		virtual void update() = 0;

		virtual lambdacommon::Color get_background_color() const;

		void set_background_color(const lambdacommon::Color &color);
	};

	class IONICENGINE_API Border
	{
	private:
		lambdacommon::Color color;

	public:
		Border(const lambdacommon::Color &color);

		const lambdacommon::Color &get_color() const;

		void set_color(const lambdacommon::Color &color);

		virtual void draw(int x, int y, uint32_t width, uint32_t height, Graphics *graphics);
	};

	class IONICENGINE_API EmptyBorder : public Border
	{
	private:
	public:
		EmptyBorder();

		void draw(int x, int y, uint32_t width, uint32_t height, Graphics *graphics) override;
	};

	class IONICENGINE_API GuiComponent : public Gui
	{
	protected:
		lambdacommon::Color color = lambdacommon::Color::COLOR_WHITE;
		Border *border = new Border(lambdacommon::Color::COLOR_BLACK);
		int x, y;
		bool visible = true, enabled = true, hovered = false, clicked = false;

	public:
		GuiComponent(int x, int y);

		virtual ~GuiComponent() override;

		const lambdacommon::Color &get_color() const;

		void set_color(const lambdacommon::Color &color);

		Border *get_border() const;

		void set_border(Border *border);

		int get_x() const;

		int get_y() const;

		/*!
		 * Checks whether the component is visible or not.
		 * @return True if the component is visible, else false.
		 */
		bool is_visible() const;

		/*!
		 * Sets whether the component is visible or not.
		 * @param visible True if the component is visible, else false.
		 */
		void set_visible(bool visible);

		bool is_enabled() const;

		void set_enabled(bool enabled);

		bool is_hovered() const;

		void set_hovered(bool hovered);

		bool is_clicked() const;

		void set_clicked(bool clicked);

		virtual bool is_focusable() const;

		virtual bool does_disable_key_activate() const;

		virtual void on_hover() = 0;

		virtual void on_activate(Window &window) = 0;

		virtual void on_mouse_pressed(Window &window, int button, int mouseX, int mouseY) = 0;

		virtual void on_mouse_released(Window &window, int button, int mouseX, int mouseY) = 0;

		virtual void on_key_input(Window &window, int key, int scancode, InputAction action, int mods) = 0;

		virtual void on_gamepad_button_input(Window &window, InputAction action, uint8_t button) = 0;
	};

	class IONICENGINE_API GuiProgressBar : public GuiComponent
	{
	private:
		uint32_t progress{0};
		bool indeterminate = false;
		int indeterminate_index = 0;

		int getIndeterminateBoxLength() const;

	public:
		GuiProgressBar(int x, int y, uint32_t width, uint32_t height);

		uint32_t get_progress() const;

		void set_progress(uint32_t progress);

		bool is_indeterminate() const;

		void set_indeterminate(bool indeterminate);

		void init() override;

		void draw(Graphics *graphics) override;

		void update() override;

		bool is_focusable() const override;

		void on_hover() override;

		void on_activate(Window &window) override;

		void on_mouse_pressed(Window &window, int button, int x, int y) override;

		void on_mouse_released(Window &window, int button, int x, int y) override;

		void on_key_input(Window &window, int key, int scancode, InputAction action, int mods) override;

		void on_gamepad_button_input(Window &window, InputAction action, uint8_t button) override;
	};

	class IONICENGINE_API GuiButton : public GuiComponent
	{
	private:
		std::string text;
		lambdacommon::Color hover_color = lambdacommon::color::from_hex(0xE5F1FBFF),
				click_color = lambdacommon::color::from_hex(0x6CBADFFF);
		Font *font;
		std::function<void(Window &window)> _on_activate_listener = [](Window &window) {};

	public:
		GuiButton(int x, int y, uint32_t width, uint32_t height, const std::string &text);

		void init() override;

		void draw(Graphics *graphics) override;

		void update() override;

		void on_hover() override;

		void on_activate(Window &window) override;

		void on_mouse_pressed(Window &window, int button, int x, int y) override;

		void on_mouse_released(Window &window, int button, int x, int y) override;

		void on_key_input(Window &window, int key, int scancode, InputAction action, int mods) override;

		void on_gamepad_button_input(Window &window, InputAction action, uint8_t button) override;

		const lambdacommon::Color &get_hover_color() const;

		void set_hover_color(const lambdacommon::Color &hoverColor);

		const lambdacommon::Color &get_click_color() const;

		void set_click_color(const lambdacommon::Color &clickColor);

		void set_activate_listener(const std::function<void(Window &window)> &on_activate_listener);

		const std::string &get_text() const;

		void set_text(const std::string &text);

		Font *get_font() const;

		void set_font(Font *font);
	};
}

#endif //IONICENGINE_GUI_H