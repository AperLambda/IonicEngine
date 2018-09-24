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

	void Screen::draw(Graphics *graphics)
	{
		for (GuiComponent *component : components)
			component->draw(graphics);
	}

	void Screen::update()
	{
		for (GuiComponent *component : components)
			component->update();
	}

	void Screen::refresh(uint32_t width, uint32_t height)
	{
		this->width = width;
		this->height = height;
		for (GuiComponent *component : components)
			delete component;
		components.clear();
	}

	bool Screen::onMouseMove(int x, int y)
	{
		for (GuiComponent *component : components)
		{
			if (component->isVisible())
			{
				if (component->isHovered())
				{
					if (graphics::isMouseInBox(x, y, component->getX(), component->getY(), component->width,
											   component->height))
						return true;
					else
						component->setHovered(false);
				}
				else
				{
					if (graphics::isMouseInBox(x, y, component->getX(), component->getY(), component->width,
											   component->height))
					{
						component->setHovered(true);
						component->onHover();
						return true;
					}
				}
			}
		}
		return false;
	}

	bool Screen::onMousePressed(Window &window, int button, int mouseX, int mouseY)
	{
		for (GuiComponent *component : components)
		{
			if (component->isEnabled() && component->isVisible() &&
				graphics::isMouseInBox(mouseX, mouseY, component->getX(), component->getY(), component->width,
									   component->height))
			{
				component->setClicked(true);
				component->onMousePressed(window, button, mouseX, mouseY);
				return true;
			}
		}
		return false;
	}

	bool Screen::onMouseReleased(Window &window, int button, int mouseX, int mouseY)
	{
		for (GuiComponent *component : components)
		{
			if (component->isEnabled() && component->isVisible() && mouseX >= component->getX() &&
				mouseY >= component->getY() && mouseX < component->getX() + static_cast<int>(component->width) &&
				mouseY < component->getY() + static_cast<int>(component->height))
			{
				component->setClicked(false);
				component->onMouseReleased(window, button, mouseX, mouseY);
				return true;
			}
		}
		return false;
	}

	/*
	 * NULLSCREEN
	 */

	class NullScreen : public Screen
	{
	public:
		void init() override
		{}

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

	void OverlayFPS::init()
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

	ScreenManager::~ScreenManager()
	{
		delete graphics;
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
		auto screen = getActiveScreen();
		if (screen != nullptr)
		{
			screen->refresh(oldFramebufferSize.first, oldFramebufferSize.second);
			screen->init();
		}
	}

	std::vector<lambdacommon::ResourceName> ScreenManager::getActiveOverlays() const
	{
		return _activeOverlays;
	}

	void ScreenManager::addActiveOverlay(const lambdacommon::ResourceName &name)
	{
		if (!isOverlayActive(name))
		{
			_activeOverlays.emplace_back(name);
			auto overlay = _overlays.at(name);
			if (overlay != nullptr)
			{
				overlay->refresh(oldFramebufferSize.first, oldFramebufferSize.second);
				overlay->init();
			}
		}
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

	std::optional<Window> ScreenManager::getAttachedWindow() const
	{
		return _window;
	}

	int ScreenManager::getFPS() const
	{
		return fps;
	}

	int ScreenManager::getUpdates() const
	{
		return updates;
	}

	float ScreenManager::getDeltaTime() const
	{
		return deltaTime;
	}

	void ScreenManager::setDeltaTime(float deltaTime)
	{
		ScreenManager::deltaTime = deltaTime;
	}

	bool ScreenManager::onMouseMove(int x, int y)
	{
		if (!_window)
			return false;

		for (const auto &activeOverlay : _activeOverlays)
		{
			if (hasOverlay(activeOverlay))
			{
				Overlay *overlay = getOverlay(activeOverlay);
				if (overlay->onMouseMove(x, y))
					return true;
			}
		}

		auto screen = getActiveScreen();
		return screen->onMouseMove(x, y);
	}

	bool ScreenManager::onMouseButton(int button, InputAction action, int mods)
	{
		if (!_window)
			return false;

		auto cursorPosition = _window->getCursorPosition();

		for (const auto &activeOverlay : _activeOverlays)
		{
			if (hasOverlay(activeOverlay))
			{
				Overlay *overlay = getOverlay(activeOverlay);
				if (action == InputAction::PRESS)
				{
					if (overlay->onMousePressed(_window.value(), button, static_cast<int>(cursorPosition.first),
												static_cast<int>(cursorPosition.second)))
						return true;
				}
				else if (action == InputAction::RELEASE)
				{
					if (overlay->onMouseReleased(_window.value(), button, static_cast<int>(cursorPosition.first),
												 static_cast<int>(cursorPosition.second)))
						return true;
				}
			}
		}

		auto screen = getActiveScreen();
		if (action == InputAction::PRESS)
		{
			if (screen->onMousePressed(_window.value(), button, static_cast<int>(cursorPosition.first),
									   static_cast<int>(cursorPosition.second)))
				return true;
		}
		else if (action == InputAction::RELEASE)
		{
			if (screen->onMouseReleased(_window.value(), button, static_cast<int>(cursorPosition.first),
										static_cast<int>(cursorPosition.second)))
				return true;
		}

		return false;
	}

	void ScreenManager::render()
	{
		if (_window)
		{
			_window->requestContext();
			auto screen = getActiveScreen();
			auto backgroundColor = lambdacommon::Color::COLOR_BLACK;
			if (screen != nullptr)
				backgroundColor = screen->getBackgroundColor();
			glClearColor(backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(),
						 backgroundColor.alpha());
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (screen != nullptr)
			{
				screen->draw(graphics);
				graphics->resetTransform();
			}
			for (const auto &activeOverlay : _activeOverlays)
			{
				if (hasOverlay(activeOverlay))
				{
					Overlay *overlay = getOverlay(activeOverlay);
					overlay->draw(graphics);
					graphics->resetTransform();
				}
			}
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

		auto size = _window->getFramebufferSize();
		graphics = getGraphicsManager()->newGraphics(size);
		oldFramebufferSize = {0, 0};

		double lastTime = glfwGetTime(), timer = lastTime;
		double deltaTime = 0, nowTime = 0;
		int frames = 0, updates = 0;

		glfwSetFramebufferSizeCallback(_window->getHandle(), [](GLFWwindow *window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});

		while (!_window->shouldClose())
		{
			auto currentSize = _window->getSize();
			if (oldFramebufferSize != currentSize)
			{
				graphics->updateFramebufferSize(currentSize.first, currentSize.second);
				oldFramebufferSize = currentSize;
				auto screen = getActiveScreen();
				if (screen != nullptr)
				{
					screen->refresh(currentSize.first, currentSize.second);
					screen->init();
				}
				for (const auto &activeOverlay : _activeOverlays)
				{
					if (hasOverlay(activeOverlay))
					{
						Overlay *overlay = getOverlay(activeOverlay);
						overlay->refresh(currentSize.first, currentSize.second);
						overlay->init();
					}
				}
			}

			this->render();

			nowTime = glfwGetTime();
			deltaTime += (nowTime - lastTime);
			lastTime = nowTime;

			this->deltaTime = static_cast<float>(deltaTime);

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
					((OverlayFPS *) getOverlay(IONICENGINE_OVERLAYS_FPS))->updateFPS(frames);
				updates = 0, frames = 0;
			}
		}

		LCOMMON_DELETE_POINTER(graphics);

		_window->destroy();
	}

	namespace screen
	{
		std::map<Window, ScreenManager *> screenManagers;

		ScreenManager *getScreenManager(const Window &window)
		{
			return screenManagers.at(window);
		}
	}

	void ScreenManager::attachWindow(const Window &window)
	{
		_window = {window};
		screen::screenManagers.insert({window, this});
	}
}