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

namespace ionicengine
{
	class IONICENGINE_API Gui
	{
	protected:
		lambdacommon::Color backgroundColor = lambdacommon::Color::COLOR_BLACK;

	public:
		uint32_t width{}, height{};

		virtual ~Gui() = default;

		virtual void init() = 0;

		virtual void draw(Graphics *graphics) = 0;

		virtual void update() = 0;

		virtual lambdacommon::Color getBackgroundColor() const;

		void setBackgroundColor(const lambdacommon::Color &color);
	};

	class IONICENGINE_API Border
	{
	private:
		lambdacommon::Color color;

	public:
		Border(const lambdacommon::Color &color);

		const lambdacommon::Color &getColor() const;

		void setColor(const lambdacommon::Color &color);

		virtual void draw(int x, int y, uint32_t width, uint32_t  height, Graphics *graphics);
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

	public:
		virtual ~GuiComponent() override;

		const lambdacommon::Color &getColor() const;

		void setColor(const lambdacommon::Color &color);

		Border *getBorder() const;

		void setBorder(Border *border);
	};

	class IONICENGINE_API GuiProgressBar : public GuiComponent
	{
	private:
		int x, y;
		uint32_t progress{0};
		bool indeterminate = false;
		int indeterminateIndex = 0;

		int getIndeterminateBoxLength() const;

	public:
		GuiProgressBar(int x, int y, uint32_t width, uint32_t height);

		uint32_t getProgress() const;

		void setProgress(uint32_t progress);

		bool isIndeterminate() const;

		void setIndeterminate(bool indeterminate);

		void init() override;

		void draw(Graphics *graphics) override;

		void update() override;
	};

	class IONICENGINE_API GuiButton : public GuiComponent
	{
	};
}

#endif //IONICENGINE_GUI_H