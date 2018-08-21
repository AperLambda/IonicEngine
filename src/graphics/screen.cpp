/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/screen.h"

namespace ionicengine
{
	class NullScreen : public Screen
	{
	public:
		void draw(Graphics *graphics) override
		{
			// Draws nothing it's a null screen.
		}

		void update() override
		{}
	};

	ScreenManager::ScreenManager()
	{
		registerScreen({"ionicengine", "null"}, new NullScreen());
	}

	void ScreenManager::registerScreen(lambdacommon::ResourceName resourceName, Screen *screen)
	{

	}

	void ScreenManager::registerOverlay(lambdacommon::ResourceName resourceName, Overlay *overlay)
	{

	}

	Screen *ScreenManager::getActiveScreen() const
	{
		return _screens.at(_activeScreen);
	}

	void ScreenManager::render()
	{
		auto screen = getActiveScreen();
		if (screen != nullptr)
			screen->draw(nullptr); // Temporary
	}

	void ScreenManager::update()
	{
		auto screen = getActiveScreen();
		if (screen != nullptr)
			screen->update();
		for (const auto &activeOverlay : _activeOverlays)
		{
			if (_overlays.count(activeOverlay))
			{
				Overlay *overlay = _overlays.at(activeOverlay);
				overlay->update();
			}
		}
	}
}