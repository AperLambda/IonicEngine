/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/gui.h"

namespace ionicengine
{
	/*
	 * Gui
	 */

	lambdacommon::Color Gui::get_background_color() const
	{
		return background_color;
	}

	void Gui::set_background_color(const lambdacommon::Color &color)
	{
		background_color = color;
	}

	/*
	 * BORDER
	 */

	Border::Border(const lambdacommon::Color &color) : color(color)
	{}

	const lambdacommon::Color &Border::get_color() const
	{
		return color;
	}

	void Border::set_color(const lambdacommon::Color &color)
	{
		Border::color = color;
	}

	void Border::draw(int x, int y, uint32_t width, uint32_t height, Graphics *graphics)
	{
		graphics->set_color(color);
		graphics->draw_quad_outline(x, y, width, height);
		/*// Top
		graphics->drawLine2D(x, y, x + width, y);
		// Left
		graphics->drawLine2D(x, y, x, y + height + 1);
		// Bottom
		graphics->draw_line_2d(x, y + height, x + width, y + height);
		// Right
		graphics->draw_line_2d(x + width, y, x + width, y + height);*/
	}

	void EmptyBorder::draw(int x, int y, uint32_t width, uint32_t height, Graphics *graphics)
	{
		// Draws nothing.
	}

	EmptyBorder::EmptyBorder() : Border(lambdacommon::color::from_hex(0xFFFFFFFF))
	{}

	/*
	 * GUI COMPONENT
	 */

	GuiComponent::GuiComponent(int x, int y) : x(x), y(y)
	{}

	GuiComponent::~GuiComponent()
	{
		delete border;
	}

	const lambdacommon::Color &GuiComponent::get_color() const
	{
		return color;
	}

	void GuiComponent::set_color(const lambdacommon::Color &color)
	{
		this->color = color;
	}

	Border *GuiComponent::get_border() const
	{
		return border;
	}

	void GuiComponent::set_border(Border *border)
	{
		delete this->border;
		this->border = border;
	}

	int GuiComponent::get_x() const
	{
		return x;
	}

	int GuiComponent::get_y() const
	{
		return y;
	}

	bool GuiComponent::is_visible() const
	{
		return visible;
	}

	void GuiComponent::set_visible(bool visible)
	{
		this->visible = visible;
	}

	bool GuiComponent::is_enabled() const
	{
		return enabled;
	}

	void GuiComponent::set_enabled(bool enabled)
	{
		GuiComponent::enabled = enabled;
	}

	bool GuiComponent::is_hovered() const
	{
		return hovered;
	}

	void GuiComponent::set_hovered(bool hovered)
	{
		GuiComponent::hovered = hovered;
	}

	bool GuiComponent::is_clicked() const
	{
		return clicked;
	}

	void GuiComponent::set_clicked(bool clicked)
	{
		GuiComponent::clicked = clicked;
	}

	bool GuiComponent::is_focusable() const
	{
		return true;
	}

	bool GuiComponent::does_disable_key_activate() const
	{
		return false;
	}

	/*
	 * GUI PROGRESS BAR
	 */

	GuiProgressBar::GuiProgressBar(int x, int y, uint32_t width, uint32_t height) : GuiComponent(x, y)
	{
		this->width = width;
		this->height = height;
		this->background_color = lambdacommon::color::from_hex(0xE6E6E4FF);
		this->color = lambdacommon::color::from_hex(0x06B224FF);
		set_border(new Border(lambdacommon::color::from_hex(0xD6D5D6FF)));
	}

	int GuiProgressBar::getIndeterminateBoxLength() const
	{
		return width / 6;
	}

	void GuiProgressBar::init()
	{}

	void GuiProgressBar::draw(Graphics *graphics)
	{
		if (!is_visible())
			return;
		// Background
		graphics->set_color(background_color);
		graphics->draw_quad(x, y, width, height);
		if (is_enabled())
		{
			if (indeterminate)
			{
				graphics->set_color(color);
				int current_x = lambdacommon::maths::clamp(x + indeterminate_index, x, x + (int) width);
				if (current_x + getIndeterminateBoxLength() >= 0)
				{
					auto indeterminateWidth = static_cast<uint32_t>(getIndeterminateBoxLength());
					if (indeterminate_index < 0)
						indeterminateWidth += indeterminate_index;
					else if (indeterminate_index > (int) width - getIndeterminateBoxLength())
						indeterminateWidth = (x + width) - current_x;
					graphics->draw_quad(current_x, y, indeterminateWidth, height);
				}
			}
			else
			{
				graphics->set_color(color);
				graphics->draw_quad(x, y, static_cast<uint32_t>((progress / 100.f) * width), height);
			}
		}
		border->draw(x, y, width, height, graphics);
	}

	void GuiProgressBar::update()
	{
		if (indeterminate)
		{
			indeterminate_index += 2;
			if (indeterminate_index >= (int) width + getIndeterminateBoxLength())
				indeterminate_index = -getIndeterminateBoxLength();
		}
	}

	void GuiProgressBar::on_hover()
	{}

	void GuiProgressBar::on_activate(Window &window)
	{}

	void GuiProgressBar::on_mouse_pressed(Window &window, int button, int x, int y)
	{}

	void GuiProgressBar::on_mouse_released(Window &window, int button, int x, int y)
	{}

	void GuiProgressBar::on_key_input(Window &window, int key, int scancode, InputAction action, int mods)
	{}

	void GuiProgressBar::on_gamepad_button_input(Window &window, InputAction action, uint8_t button)
	{}

	uint32_t GuiProgressBar::get_progress() const
	{
		return progress;
	}

	void GuiProgressBar::set_progress(uint32_t progress)
	{
		this->progress = lambdacommon::maths::clamp(progress, (uint32_t) 0, (uint32_t) 100);
	}

	bool GuiProgressBar::is_indeterminate() const
	{
		return indeterminate;
	}

	void GuiProgressBar::set_indeterminate(bool indeterminate)
	{
		this->indeterminate = indeterminate;
	}

	bool GuiProgressBar::is_focusable() const
	{
		return false;
	}

	/*
	 * GUI BUTTON
	 */

	GuiButton::GuiButton(int x, int y, uint32_t width, uint32_t height, const std::string &text) : GuiComponent(x, y),
																								   text(text)
	{
		this->width = width;
		this->height = height;
		font = get_font_manager()->get_default_font();
		background_color = lambdacommon::color::from_hex(0xE1E1E1FF);
		color = lambdacommon::Color::COLOR_BLACK;
		set_border(new Border(lambdacommon::color::from_hex(0xADADADFF)));
	}

	void GuiButton::init()
	{

	}

	void GuiButton::draw(Graphics *graphics)
	{
		if (!is_visible())
			return;

		if (is_clicked())
		{
			graphics->set_color(lambdacommon::color::from_hex(0xC9E0F7FF));
			border->set_color(lambdacommon::color::from_hex(0x62A2E4FF));
		}
		else if (is_hovered())
		{
			graphics->set_color(hover_color);
			border->set_color(lambdacommon::color::from_hex(0x0078D7FF));
		}
		else
		{
			graphics->set_color(background_color);
			border->set_color(lambdacommon::color::from_hex(0xADADADFF));
		}
		graphics->draw_quad(x, y, width, height);
		graphics->set_color(color);
		auto drawable_text = text;
		auto text_height = font->get_text_height(drawable_text);
		while (text_height > height - 4)
		{
			auto new_line = drawable_text.find_last_of('\n');
			if (new_line == std::string::npos)
				// Fuck it and draw it.
				break;
			drawable_text = drawable_text.substr(0, new_line + 1);
			text_height = font->get_text_height(drawable_text);
		}
		drawable_text = font->trim_text_to_length_dotted(drawable_text, width - 4);
		graphics->draw_text(*font, x + ((width / 2) - (font->get_text_length(drawable_text) / 2)),
							y + ((height / 2) - (text_height / 2)) + 4, drawable_text);

		border->draw(x, y, width, height, graphics);
	}

	void GuiButton::update()
	{

	}

	void GuiButton::on_hover()
	{}

	void GuiButton::on_activate(Window &window)
	{
		_on_activate_listener(window);
	}

	void GuiButton::on_mouse_pressed(Window &window, int button, int x, int y)
	{
		if (button == GLFW_MOUSE_BUTTON_1)
			_on_activate_listener(window);
	}

	void GuiButton::on_mouse_released(Window &window, int button, int x, int y)
	{}

	void GuiButton::on_key_input(Window &window, int key, int scancode, InputAction action, int mods)
	{}

	void GuiButton::on_gamepad_button_input(Window &window, InputAction action, uint8_t button)
	{}

	const lambdacommon::Color &GuiButton::get_hover_color() const
	{
		return hover_color;
	}

	void GuiButton::set_hover_color(const lambdacommon::Color &hoverColor)
	{
		GuiButton::hover_color = hoverColor;
	}

	const lambdacommon::Color &GuiButton::get_click_color() const
	{
		return click_color;
	}

	void GuiButton::set_click_color(const lambdacommon::Color &clickColor)
	{
		GuiButton::click_color = clickColor;
	}

	void GuiButton::set_activate_listener(const std::function<void(Window &window)> &on_activate_listener)
	{
		_on_activate_listener = on_activate_listener;
	}

	const std::string &GuiButton::get_text() const
	{
		return text;
	}

	void GuiButton::set_text(const std::string &text)
	{
		GuiButton::text = text;
	}

	Font *GuiButton::get_font() const
	{
		return font;
	}

	void GuiButton::set_font(Font *font)
	{
		GuiButton::font = font;
	}
}