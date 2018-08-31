/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_ANIMATION_H
#define IONICENGINE_ANIMATION_H

#include "textures.h"
#include "graphics.h"

namespace ionicengine
{
	class Animation
	{
	private:
		bool running = true;
	protected:
		bool repeat = true, forward = true, invertOnRepeat = false;
		uint32_t x, y, width, height;
		size_t state = 0;
		uint32_t updates = 0, frametime = 1;

		void updateState();

	public:
		Animation(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

		/*!
		 * Gets the X position of the animation.
		 * @return The X position of the animation.
		 */
		uint32_t getX() const;

		/*!
		 * Sets the X position of the animation.
		 * @param x The X position of the animation.
		 */
		void setX(uint32_t x);

		/*!
		 * Gets the Y position of the animation.
		 * @return The Y position of the animation.
		 */
		uint32_t getY() const;

		/*!
		 * Sets the Y position of the animation.
		 * @param y The Y position of the animation.
		 */
		void setY(uint32_t y);

		/*!
		 * Checks whether the animation is running or not.
		 * @return True if the animation is running else false.
		 */
		bool isRunning() const;

		/*!
		 * Starts the animation.
		 */
		void start();

		/*!
		 * Stops the animation.
		 */
		void stop();

		/*!
		 * Gets the frame time of the animation.
		 * The animation is updated after n number of updates, n is frame time.
		 * @return The frame time of the animation.
		 */
		uint32_t getFrameTime() const;

		/*!
		 * Sets the frame time of the animation.
		 * The animation is updated after n number of updates, n is frame time.
		 * @param rate The frame time of the animation.
		 */
		void setFrameTime(uint32_t rate);

		/*!
		 * Checks whether the animation is repeated or not.
		 * @return True if the animation is repeated else false.
		 */
		bool isRepeated() const;

		/*!
		 * Sets whether the animation is repeated or not.
		 * @param repeat True if the animation is repeated else false.
		 */
		void setRepeat(bool repeat);

		/*!
		 * Checks whether the animation indexes is inverted when repeated.
		 * @return True if the animation indexes is inverted when repeated else false.
		 */
		bool isInvertOnRepeat() const;

		/*!
		 * Sets whether the animation indexes is inverted when repeated.
		 * @param invertOnRepeat True if the animation indexes is inverted when repeated else false.
		 */
		void setInvertOnRepeat(bool invertOnRepeat);

		/*!
		 * Resets the state of the animation.
		 */
		void reset();

		/*!
		 * Gets the current state of the animation.
		 * @return The current state of the animation.
		 */
		size_t getState() const;

		/*!
		 * Sets the current state of the animation.
		 * @param state The current state of the animation.
		 */
		virtual void setState(size_t state);

		virtual size_t getMaxState() = 0;

		virtual void render(Graphics *graphics) = 0;

		virtual void update();
	};

	class TexturesAnimation : public Animation
	{
	private:
		std::vector<Texture> textures;

	public:
		TexturesAnimation(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const std::vector<Texture> &textures);

		const std::vector<Texture> &getTextures() const;

		void setState(size_t state) override;

		virtual size_t getMaxState();

		void render(Graphics *graphics) override;
	};

	class BitmapAnimation : public Animation
	{
	private:
		Texture texture;
		std::vector<TextureRegion> regions;

	public:
		BitmapAnimation(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Texture &texture, uint32_t regionHeight);

		void setState(size_t state) override;

		virtual size_t getMaxState() override;

		void render(Graphics *graphics) override;
	};

	namespace animation
	{
		// Unused for now. Possibly adds are: GIF Reader and BitmapAnimation generator for GIF? It can be kinda cool!
	}
}

#endif //IONICENGINE_ANIMATION_H
