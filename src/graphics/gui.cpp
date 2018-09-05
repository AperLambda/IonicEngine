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

	lambdacommon::Color Gui::getBackgroundColor() const
	{
		return backgroundColor;
	}

	void Gui::setBackgroundColor(const lambdacommon::Color &color)
	{
		backgroundColor = color;
	}

	/*
	 * BORDER
	 */

	Border::Border(const lambdacommon::Color &color) : color(color)
	{}

	const lambdacommon::Color &Border::getColor() const
	{
		return color;
	}

	void Border::setColor(const lambdacommon::Color &color)
	{
		Border::color = color;
	}

	void Border::draw(int x, int y, uint32_t width, uint32_t height, Graphics *graphics)
	{
		graphics->setColor(color);
		graphics->drawQuadOutline(x, y, width, height);
		/*// Top
		graphics->drawLine2D(x, y, x + width, y);
		// Left
		graphics->drawLine2D(x, y, x, y + height + 1);
		// Bottom
		graphics->drawLine2D(x, y + height, x + width, y + height);
		// Right
		graphics->drawLine2D(x + width, y, x + width, y + height);*/
	}

	void EmptyBorder::draw(int x, int y, uint32_t width, uint32_t height, Graphics *graphics)
	{
		// Draws nothing.
	}

	EmptyBorder::EmptyBorder() : Border(lambdacommon::color::fromHex(0xFFFFFFFF))
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

	const lambdacommon::Color &GuiComponent::getColor() const
	{
		return color;
	}

	void GuiComponent::setColor(const lambdacommon::Color &color)
	{
		this->color = color;
	}

	Border *GuiComponent::getBorder() const
	{
		return border;
	}

	void GuiComponent::setBorder(Border *border)
	{
		delete this->border;
		this->border = border;
	}

	int GuiComponent::getX() const
	{
		return x;
	}

	int GuiComponent::getY() const
	{
		return y;
	}

	bool GuiComponent::isVisible() const
	{
		return visible;
	}

	void GuiComponent::setVisible(bool visible)
	{
		this->visible = visible;
	}

	bool GuiComponent::isEnabled() const
	{
		return enabled;
	}

	void GuiComponent::setEnabled(bool enabled)
	{
		GuiComponent::enabled = enabled;
	}

	bool GuiComponent::isHovered() const
	{
		return hovered;
	}

	void GuiComponent::setHovered(bool hovered)
	{
		GuiComponent::hovered = hovered;
	}

	/*
	 * GUI PROGRESS BAR
	 */

	GuiProgressBar::GuiProgressBar(int x, int y, uint32_t width, uint32_t height) : GuiComponent(x, y)
	{
		this->width = width;
		this->height = height;
		this->backgroundColor = lambdacommon::color::fromHex(0xE6E6E4FF);
		this->color = lambdacommon::color::fromHex(0x06B224FF);
		setBorder(new Border(lambdacommon::color::fromHex(0xD6D5D6FF)));
	}

	int GuiProgressBar::getIndeterminateBoxLength() const
	{
		return width / 6;
	}

	void GuiProgressBar::init()
	{}

	void GuiProgressBar::draw(Graphics *graphics)
	{
		if (!isVisible())
			return;
		// Background
		graphics->setColor(backgroundColor);
		graphics->drawQuad(x, y, width, height);
		if (isEnabled())
		{
			if (indeterminate)
			{
				graphics->setColor(color);
				int currentX = lambdacommon::maths::clamp(x + indeterminateIndex, x, x + (int) width);
				if (currentX + getIndeterminateBoxLength() >= 0)
				{
					auto indeterminateWidth = static_cast<uint32_t>(getIndeterminateBoxLength());
					if (indeterminateIndex < 0)
						indeterminateWidth += indeterminateIndex;
					else if (indeterminateIndex > (int) width - getIndeterminateBoxLength())
						indeterminateWidth = (x + width) - currentX;
					graphics->drawQuad(currentX, y, indeterminateWidth, height);
				}
			}
			else
			{
				graphics->setColor(color);
				graphics->drawQuad(x, y, static_cast<uint32_t>((progress / 100.f) * width), height);
			}
		}
		border->draw(x, y, width, height, graphics);
	}

	void GuiProgressBar::update()
	{
		if (indeterminate)
		{
			indeterminateIndex += 2;
			if (indeterminateIndex >= (int) width + getIndeterminateBoxLength())
				indeterminateIndex = -getIndeterminateBoxLength();
		}
	}

	void GuiProgressBar::onHover()
	{}

	void GuiProgressBar::onMousePressed(Window &window, int button, int x, int y)
	{}

	void GuiProgressBar::onMouseReleased(Window &window, int button, int x, int y)
	{}

	uint32_t GuiProgressBar::getProgress() const
	{
		return progress;
	}

	void GuiProgressBar::setProgress(uint32_t progress)
	{
		this->progress = lambdacommon::maths::clamp(progress, (uint32_t) 0, (uint32_t) 100);
	}

	bool GuiProgressBar::isIndeterminate() const
	{
		return indeterminate;
	}

	void GuiProgressBar::setIndeterminate(bool indeterminate)
	{
		this->indeterminate = indeterminate;
	}

	/*
	 * GUI BUTTON
	 */

	GuiButton::GuiButton(int x, int y, uint32_t width, uint32_t height, const std::string &text) : GuiComponent(x, y), text(text)
	{
		this->width = width;
		this->height = height;
		font = getFontManager()->getDefaultFont();
		backgroundColor = lambdacommon::color::fromHex(0xE6E6E4FF);
		color = lambdacommon::Color::COLOR_BLACK;
		setBorder(new Border(lambdacommon::color::fromHex(0xD6D5D6FF)));
	}

	void GuiButton::init()
	{

	}

	void GuiButton::draw(Graphics *graphics)
	{
		if (!isVisible())
			return;

		if (isHovered())
			graphics->setColor(hoverColor);
		else
			graphics->setColor(backgroundColor);
		graphics->drawQuad(x, y, width, height);
		graphics->setColor(color);
		auto drawableText = text;
		auto textHeight = font->getTextHeight(drawableText);
		while (textHeight > height - 4)
		{
			auto newLine = drawableText.find_last_of('\n');
			if (newLine == std::string::npos)
				// Fuck it and draw it.
				break;
			drawableText = drawableText.substr(0, newLine + 1);
			textHeight = font->getTextHeight(drawableText);
		}
		drawableText = font->trimTextToLengthDotted(drawableText, width - 4);
		graphics->drawText(*font, x + ((width / 2.f) - (font->getTextLength(drawableText) / 2.f)),
						   y + ((height / 2.f) - (textHeight / 2.f)), drawableText);

		border->draw(x, y, width, height, graphics);
	}

	void GuiButton::update()
	{

	}

	void GuiButton::onHover()
	{}

	void GuiButton::onMousePressed(Window &window, int button, int x, int y)
	{
		if (button == GLFW_MOUSE_BUTTON_1)
			onButtonClickListener(window);
	}

	void GuiButton::onMouseReleased(Window &window, int button, int x, int y)
	{}

	const lambdacommon::Color &GuiButton::getHoverColor() const
	{
		return hoverColor;
	}

	void GuiButton::setHoverColor(const lambdacommon::Color &hoverColor)
	{
		GuiButton::hoverColor = hoverColor;
	}

	const lambdacommon::Color &GuiButton::getClickColor() const
	{
		return clickColor;
	}

	void GuiButton::setClickColor(const lambdacommon::Color &clickColor)
	{
		GuiButton::clickColor = clickColor;
	}

	void GuiButton::setClickListener(const std::function<void(Window &window)> &onButtonClickListener)
	{
		GuiButton::onButtonClickListener = onButtonClickListener;
	}
}