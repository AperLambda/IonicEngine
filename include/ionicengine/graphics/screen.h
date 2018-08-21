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

namespace ionicengine
{
	class Screen
	{
	public:
		virtual void draw(Graphics *graphics) = 0;

		virtual void update() = 0;
	};

	class Overlay : public Screen
	{
	public:
		void draw(Graphics *graphics) override = 0;

		void update() override = 0;
	};

	class ScreenManager
	{
	private:
		std::map<lambdacommon::ResourceName, Screen *> _screens;
		std::map<lambdacommon::ResourceName, Overlay *> _overlays;
		lambdacommon::ResourceName _activeScreen{"ionicengine", "null"};
		std::vector<lambdacommon::ResourceName> _activeOverlays;

	public:
		ScreenManager();

		void registerScreen(lambdacommon::ResourceName resourceName, Screen *screen);

		void registerOverlay(lambdacommon::ResourceName resourceName, Overlay *overlay);

		Screen *getActiveScreen() const;

		void render();

		void update();
	};
}

#endif //IONICENGINE_SCREEN_H
