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
		bool repeat = true, forward = true, invert_on_repeat = false;
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
		uint32_t get_x() const;

		/*!
		 * Sets the X position of the animation.
		 * @param x The X position of the animation.
		 */
		void set_x(uint32_t x);

		/*!
		 * Gets the Y position of the animation.
		 * @return The Y position of the animation.
		 */
		uint32_t get_y() const;

		/*!
		 * Sets the Y position of the animation.
		 * @param y The Y position of the animation.
		 */
		void set_y(uint32_t y);

		/*!
		 * Checks whether the animation is running or not.
		 * @return True if the animation is running else false.
		 */
		bool is_running() const;

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
		uint32_t get_frame_time() const;

		/*!
		 * Sets the frame time of the animation.
		 * The animation is updated after n number of updates, n is frame time.
		 * @param rate The frame time of the animation.
		 */
		void set_frame_time(uint32_t rate);

		/*!
		 * Checks whether the animation is repeated or not.
		 * @return True if the animation is repeated else false.
		 */
		bool is_repeated() const;

		/*!
		 * Sets whether the animation is repeated or not.
		 * @param repeat True if the animation is repeated else false.
		 */
		void set_repeat(bool repeat);

		/*!
		 * Checks whether the animation indexes is inverted when repeated.
		 * @return True if the animation indexes is inverted when repeated else false.
		 */
		bool is_inverted_on_repeat() const;

		/*!
		 * Sets whether the animation indexes is inverted when repeated.
		 * @param invert_on_repeat True if the animation indexes is inverted when repeated else false.
		 */
		void set_invert_on_repeat(bool invert_on_repeat);

		/*!
		 * Resets the state of the animation.
		 */
		void reset();

		/*!
		 * Gets the current state of the animation.
		 * @return The current state of the animation.
		 */
		size_t get_state() const;

		/*!
		 * Sets the current state of the animation.
		 * @param state The current state of the animation.
		 */
		virtual void set_state(size_t state);

		virtual size_t get_max_state() = 0;

		virtual void render(Graphics *graphics) = 0;

		virtual void update();
	};

	class TexturesAnimation : public Animation
	{
	private:
		std::vector<Texture> textures;

	public:
		TexturesAnimation(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const std::vector<Texture> &textures);

		const std::vector<Texture> &get_textures() const;

		void set_state(size_t state) override;

		virtual size_t get_max_state();

		void render(Graphics *graphics) override;
	};

	class BitmapAnimation : public Animation
	{
	private:
		Texture texture;
		std::vector<TextureRegion> regions;

	public:
		BitmapAnimation(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Texture &texture, uint32_t regionHeight);

		void set_state(size_t state) override;

		virtual size_t get_max_state() override;

		void render(Graphics *graphics) override;
	};

	namespace animation
	{
		// Unused for now. Possibly adds are: GIF Reader and BitmapAnimation generator for GIF? It can be kinda cool!
	}
}

#endif //IONICENGINE_ANIMATION_H
