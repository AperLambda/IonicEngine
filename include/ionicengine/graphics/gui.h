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

		const lambdacommon::Color &getColor() const;

		void setColor(const lambdacommon::Color &color);

		Border *getBorder() const;

		void setBorder(Border *border);

		int getX() const;

		int getY() const;

		/*!
		 * Checks whether the component is visible or not.
		 * @return True if the component is visible, else false.
		 */
		bool isVisible() const;

		/*!
		 * Sets whether the component is visible or not.
		 * @param visible True if the component is visible, else false.
		 */
		void setVisible(bool visible);

		bool isEnabled() const;

		void setEnabled(bool enabled);

		bool isHovered() const;

		void setHovered(bool hovered);

		bool isClicked() const;

		void setClicked(bool clicked);

		virtual void onHover() = 0;

		virtual void onMousePressed(Window &window, int button, int mouseX, int mouseY) = 0;

		virtual void onMouseReleased(Window &window, int button, int mouseX, int mouseY) = 0;
	};

	class IONICENGINE_API GuiProgressBar : public GuiComponent
	{
	private:
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

		void onHover() override;

		void onMousePressed(Window &window, int button, int x, int y) override;

		void onMouseReleased(Window &window, int button, int x, int y) override;
	};

	class IONICENGINE_API GuiButton : public GuiComponent
	{
	private:
		std::string text;
		lambdacommon::Color hoverColor = lambdacommon::color::fromHex(
				0xE5F1FBFF), clickColor = lambdacommon::color::fromHex(0x6CBADFFF);
		Font *font;
		std::function<void(Window &window)> onButtonClickListener = [](Window &window){};

	public:
		GuiButton(int x, int y, uint32_t width, uint32_t height, const std::string &text);

		void init() override;

		void draw(Graphics *graphics) override;

		void update() override;

		void onHover() override;

		void onMousePressed(Window &window, int button, int x, int y) override;

		void onMouseReleased(Window &window, int button, int x, int y) override;

		const lambdacommon::Color &getHoverColor() const;

		void setHoverColor(const lambdacommon::Color &hoverColor);

		const lambdacommon::Color &getClickColor() const;

		void setClickColor(const lambdacommon::Color &clickColor);

		void setClickListener(const std::function<void(Window &window)> &onButtonClickListener);

		const std::string &getText() const;

		void setText(const std::string &text);

		Font *getFont() const;

		void setFont(Font *font);
	};
}

#endif //IONICENGINE_GUI_H