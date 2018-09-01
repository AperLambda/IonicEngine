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

	/*
	 * GUI PROGRESS BAR
	 */

	GuiProgressBar::GuiProgressBar(int x, int y, uint32_t width, uint32_t height) : x(x), y(y)
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
		// Background
		graphics->setColor(backgroundColor);
		graphics->drawQuad(x, y, width, height);
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
}