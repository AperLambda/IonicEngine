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
	/*
	 * SCREEN
	 */

	lambdacommon::Color Screen::getBackgroundColor() const
	{
		return backgroundColor;
	}

	void Screen::setBackgroundColor(const lambdacommon::Color &color)
	{
		backgroundColor = color;
	}

	/*
	 * NULLSCREEN
	 */

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

	/*
	 * OVERLAY
	 */

	lambdacommon::Color Overlay::getBackgroundColor() const
	{
		return lambdacommon::Color(.0f, .0f, .0f, .0f);
	}

	/*
	 * OVERLAY FPS
	 */

	OverlayFPS::OverlayFPS(const Font &font) : Overlay(), _font(font)
	{}

	void OverlayFPS::draw(Graphics *graphics)
	{
		graphics->setColor(lambdacommon::Color::COLOR_WHITE);
		graphics->drawText(_font, 2, 3, "FPS: " + std::to_string(_fps));
	}

	void OverlayFPS::update()
	{}

	void OverlayFPS::updateFPS(int fps)
	{
		_fps = fps;
	}

	/*
	 * SCREENMANAGER
	 */

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

	int ScreenManager::getFPS() const
	{
		return fps;
	}

	int ScreenManager::getUpdates() const
	{
		return updates;
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
			auto backgroundColor = lambdacommon::Color::COLOR_BLACK;
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

	void ScreenManager::startLoop()
	{
		if (!_window)
			throw std::runtime_error("Cannot start loop without a Window");

		double lastTime = glfwGetTime(), timer = lastTime;
		double deltaTime = 0, nowTime = 0;
		int frames = 0, updates = 0;

		while (!_window->shouldClose())
		{
			this->render();

			nowTime = glfwGetTime();
			deltaTime += (nowTime - lastTime);
			lastTime = nowTime;

			if (deltaTime >= 0.020)
			{
				this->update();
				updates++;
				deltaTime = 0;
			}
			frames++;

			glfwSwapBuffers(_window->getHandle());
			glfwPollEvents();

			// - Reset after one second
			if (glfwGetTime() - timer > 1.0)
			{
				timer++;
				this->fps = frames;
				this->updates = updates;
				if (hasOverlay(IONICENGINE_OVERLAYS_FPS))
					((OverlayFPS*) getOverlay(IONICENGINE_OVERLAYS_FPS))->updateFPS(frames);
				updates = 0, frames = 0;
			}
		}

		_window->destroy();
	}
}