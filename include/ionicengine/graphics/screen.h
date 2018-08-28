/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_SCREEN_H
#define IONICENGINE_SCREEN_H

#include "graphics.h"
#include "../window/window.h"

namespace ionicengine
{
	class IONICENGINE_API Screen
	{
	private:
		lambdacommon::Color backgroundColor = lambdacommon::Color::COLOR_BLACK;
	public:
		virtual void draw(Graphics *graphics) = 0;

		virtual void update() = 0;

		virtual lambdacommon::Color getBackgroundColor() const;

		void setBackgroundColor(const lambdacommon::Color &color);
	};

	class IONICENGINE_API Overlay : public Screen
	{
		lambdacommon::Color getBackgroundColor() const override;
	};

	class IONICENGINE_API OverlayFPS : public Overlay
	{
	private:
		Font _font;
		int _fps{0};

	public:
		explicit OverlayFPS(const Font &font);

		void draw(Graphics *graphics) override;

		void update() override;

		void updateFPS(int fps);
	};

	class IONICENGINE_API ScreenManager
	{
	private:
		std::map<lambdacommon::ResourceName, Screen *> _screens;
		std::map<lambdacommon::ResourceName, Overlay *> _overlays;
		lambdacommon::ResourceName _activeScreen = IONICENGINE_NULL_RESOURCE;
		std::vector<lambdacommon::ResourceName> _activeOverlays;

		std::optional<Window> _window;

		int fps{0}, updates{0};

	public:
		ScreenManager();

		void registerScreen(const lambdacommon::ResourceName &name, Screen *screen);

		bool hasScreen(const lambdacommon::ResourceName &name) const;

		Screen *getScreen(const lambdacommon::ResourceName &name) const;

		void registerOverlay(const lambdacommon::ResourceName &name, Overlay *overlay);

		bool hasOverlay(const lambdacommon::ResourceName &name) const;

		Overlay *getOverlay(const lambdacommon::ResourceName &name) const;

		Screen *getActiveScreen() const;

		void setActiveScreen(const lambdacommon::ResourceName &name);

		std::vector<lambdacommon::ResourceName> getActiveOverlays() const;

		void addActiveOverlay(const lambdacommon::ResourceName &name);

		bool isOverlayActive(const lambdacommon::ResourceName &name);

		void removeActiveOverlay(const lambdacommon::ResourceName &name);

		void attachWindow(const Window &window);

		int getFPS() const;

		int getUpdates() const;

		void render();

		void update();

		void startLoop();
	};
}

#endif //IONICENGINE_SCREEN_H
