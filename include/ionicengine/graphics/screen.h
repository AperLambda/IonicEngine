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

#include "gui.h"
#include "../window/window.h"
#include <thread>

namespace ionicengine
{
	class IONICENGINE_API Screen : public Gui
	{
	protected:
		std::vector<GuiComponent *> components;

	public:
		void draw(Graphics *graphics) override;

		void update() override;

		void refresh(uint32_t width, uint32_t height);

		virtual bool onMouseMove(int x, int y);

		virtual bool onMousePressed(Window &window, int button, int mouseX, int mouseY);

		virtual bool onMouseReleased(Window &window, int button, int mouseX, int mouseY);
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

		void init() override;

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
		std::pair<uint32_t, uint32_t> oldFramebufferSize;
		Graphics *graphics;

		int fps{0}, updates{0};
		float deltaTime{0.f};

	public:
		ScreenManager();

		~ScreenManager();

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

		std::optional<Window> getAttachedWindow() const;

		int getFPS() const;

		int getUpdates() const;

		void setDeltaTime(float deltaTime);

		float getDeltaTime() const;

		bool onMouseMove(int x, int y);

		bool onMouseButton(int button, InputAction action, int mods);

		void render();

		void update();

		void startLoop();
	};

	namespace screen
	{
		/*!
		 * Gets the attached ScreenManager of the specified Window.
		 * @param window The Window.
		 * @return The pointer of the ScreenManager to get.
		 */
		extern ScreenManager * IONICENGINE_API getScreenManager(const Window &window);
	}
}

#endif //IONICENGINE_SCREEN_H