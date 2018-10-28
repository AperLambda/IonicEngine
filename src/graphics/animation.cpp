/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/animation.h"
#include <lambdacommon/maths.h>

namespace ionicengine
{
	Animation::Animation(uint32_t x, uint32_t y, uint32_t width, uint32_t height) : x(x), y(y), width(width), height(height)
	{}

	void Animation::updateState()
	{
		if (forward) state++;
		else state--;
	}

	uint32_t Animation::get_x() const
	{
		return x;
	}

	void Animation::set_x(uint32_t x)
	{
		Animation::x = x;
	}

	uint32_t Animation::get_y() const
	{
		return y;
	}

	void Animation::set_y(uint32_t y)
	{
		Animation::y = y;
	}

	bool Animation::is_running() const
	{
		return running;
	}

	void Animation::start()
	{
		running = true;
	}

	void Animation::stop()
	{
		running = false;
	}

	uint32_t Animation::get_frame_time() const
	{
		return frametime;
	}

	void Animation::set_frame_time(uint32_t rate)
	{
		Animation::frametime = rate;
	}

	bool Animation::is_repeated() const
	{
		return repeat;
	}

	void Animation::set_repeat(bool repeat)
	{
		this->repeat = repeat;
	}

	bool Animation::is_inverted_on_repeat() const
	{
		return invert_on_repeat;
	}

	void Animation::set_invert_on_repeat(bool invert_on_repeat)
	{
		this->invert_on_repeat = invert_on_repeat;
	}

	void Animation::reset()
	{
		set_state(0);
		updates = 0;
	}

	size_t Animation::get_state() const
	{
		return state;
	}

	void Animation::set_state(size_t state)
	{
		Animation::state = state;
	}

	void Animation::update()
	{
		if (!this->is_running())
			return;
		updates++;
		if (updates == frametime)
		{
			updateState();
			if (state > this->get_max_state() - 1 || state < 0)
			{
				if (repeat)
				{
					if (invert_on_repeat)
					{
						forward = !forward;
						updateState();
						updateState();
					}
					else
						state = 0;
				}
			}

			updates = 0;
		}
	}

	TexturesAnimation::TexturesAnimation(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
										 const std::vector<Texture> &textures) : Animation(x, y, width, height),
																				 textures(textures)
	{}

	const std::vector<Texture> &TexturesAnimation::get_textures() const
	{
		return textures;
	}

	void TexturesAnimation::set_state(size_t state)
	{
		Animation::set_state(
				lambdacommon::maths::clamp(state, static_cast<size_t>(0), textures.size() - 1));
	}

	size_t TexturesAnimation::get_max_state()
	{
		return textures.size();
	}

	void TexturesAnimation::render(Graphics *graphics)
	{
		graphics->draw_image(textures[lambdacommon::maths::clamp(state, static_cast<size_t>(0),
																 textures.size())], x, y, width,
							 height);
	}

	BitmapAnimation::BitmapAnimation(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Texture &texture,
									 uint32_t regionHeight) : Animation(x, y, width, height), texture(texture)
	{
		TextureRegion base{0.0f, 0.0f, 1.0f, 1.0f};
		float regionSize = static_cast<float>(regionHeight) / texture.get_height();
		for (size_t i = 0; i < (texture.get_height() / regionHeight); i++)
		{
			base.min_y(i * regionSize);
			base.max_y(base.min_y() + regionSize);
			regions.push_back(base);
		}
	}

	void BitmapAnimation::set_state(size_t state)
	{
		Animation::set_state(lambdacommon::maths::clamp(state, (size_t) 0, regions.size() - 1));
	}

	size_t BitmapAnimation::get_max_state()
	{
		return regions.size();
	}

	void BitmapAnimation::render(Graphics *graphics)
	{
		graphics->draw_image(texture, x, y, width, height,
							 regions[lambdacommon::maths::clamp(state, static_cast<size_t>(0),
																regions.size())]);
	}
}