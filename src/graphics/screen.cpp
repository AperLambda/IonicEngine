/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <algorithm>
#include "../../include/ionicengine/graphics/screen.h"

namespace ionicengine
{
	lambdacommon::Color Screen::getBackgroundColor() const
	{
		return backgroundColor;
	}

	void Screen::setBackgroundColor(const lambdacommon::Color &color)
	{
		backgroundColor = color;
	}

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
		lambdacommon::ResourceName nullScreen{"ionicengine", "screens/null"};
		registerScreen(nullScreen, new NullScreen());
		_activeScreen = nullScreen;
	}

	void ScreenManager::registerScreen(const lambdacommon::ResourceName &name, Screen *screen)
	{
		if (_screens.count(name))
			throw std::invalid_argument(
					"ScreenManager has already registered the screen with name '" + name.toString() + "'!");
		_screens[name] = screen;
	}

	bool ScreenManager::hasScreen(const lambdacommon::ResourceName &name) const
	{
		return static_cast<bool>(_screens.count(name));
	}

	Screen *ScreenManager::getScreen(const lambdacommon::ResourceName &name) const
	{
		if (!hasScreen(name))
			return nullptr;
		return _screens.at(name);
	}

	void ScreenManager::registerOverlay(const lambdacommon::ResourceName &name, Overlay *overlay)
	{
		if (_overlays.count(name))
			throw std::invalid_argument(
					"GraphicsManager has already registered the overlay with name '" + name.toString() + "'!");
		_overlays[name] = overlay;
	}

	bool ScreenManager::hasOverlay(const lambdacommon::ResourceName &name) const
	{
		return static_cast<bool>(_overlays.count(name));
	}

	Overlay *ScreenManager::getOverlay(const lambdacommon::ResourceName &name) const
	{
		if (!hasOverlay(name))
			return nullptr;
		return _overlays.at(name);
	}

	Screen *ScreenManager::getActiveScreen() const
	{
		return _screens.at(_activeScreen);
	}

	void ScreenManager::setActiveScreen(const lambdacommon::ResourceName &name)
	{
		_activeScreen = name;
	}

	std::vector<lambdacommon::ResourceName> ScreenManager::getActiveOverlays() const
	{
		return _activeOverlays;
	}

	void ScreenManager::addActiveOverlay(const lambdacommon::ResourceName &name)
	{
		if (!isOverlayActive(name))
			_activeOverlays.emplace_back(name);
	}

	bool ScreenManager::isOverlayActive(const lambdacommon::ResourceName &name)
	{
		return std::find(_activeOverlays.begin(), _activeOverlays.end(), name) != _activeOverlays.end();
	}

	void ScreenManager::removeActiveOverlay(const lambdacommon::ResourceName &name)
	{
		if (isOverlayActive(name))
			_activeOverlays.erase(std::find(_activeOverlays.begin(), _activeOverlays.end(), name));
	}

	void ScreenManager::attachWindow(const Window &window)
	{
		_window = {window};
	}

	void ScreenManager::render()
	{
		if (_window)
		{
			_window->requestContext();
			auto size = _window->getFramebufferSize();
			glViewport(0, 0, size.first, size.second);
			auto graphics = getGraphicsManager()->newGraphics(size);
			auto screen = getActiveScreen();
			auto backgroundColor = lambdacommon::Color::BLACK;
			if (screen != nullptr)
				backgroundColor = screen->getBackgroundColor();
			glClearColor(backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(),
						 backgroundColor.alpha());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (screen != nullptr)
				screen->draw(graphics); // Temporary
			for (const auto &activeOverlay : _activeOverlays)
			{
				if (hasOverlay(activeOverlay))
				{
					Overlay *overlay = getOverlay(activeOverlay);
					overlay->draw(graphics);
				}
			}
			delete graphics;
		}
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