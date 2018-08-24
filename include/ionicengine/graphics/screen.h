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
		lambdacommon::Color backgroundColor = lambdacommon::Color::BLACK;
	public:
		virtual void draw(Graphics *graphics) = 0;

		virtual void update() = 0;

		lambdacommon::Color getBackgroundColor() const;

		void setBackgroundColor(const lambdacommon::Color &color);
	};

	class IONICENGINE_API Overlay : public Screen
	{};

	class IONICENGINE_API ScreenManager
	{
	private:
		std::map<lambdacommon::ResourceName, Screen *> _screens;
		std::map<lambdacommon::ResourceName, Overlay *> _overlays;
		lambdacommon::ResourceName _activeScreen = IONICENGINE_NULL_RESOURCE;
		std::vector<lambdacommon::ResourceName> _activeOverlays;

		std::optional<Window> _window;

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

		void render();

		void update();
	};
}

#endif //IONICENGINE_SCREEN_H
