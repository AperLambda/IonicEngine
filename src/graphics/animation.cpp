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

	uint32_t Animation::getX() const
	{
		return x;
	}

	void Animation::setX(uint32_t x)
	{
		Animation::x = x;
	}

	uint32_t Animation::getY() const
	{
		return y;
	}

	void Animation::setY(uint32_t y)
	{
		Animation::y = y;
	}

	bool Animation::isRunning() const
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

	uint32_t Animation::getFrameTime() const
	{
		return frametime;
	}

	void Animation::setFrameTime(uint32_t rate)
	{
		Animation::frametime = rate;
	}

	bool Animation::isRepeated() const
	{
		return repeat;
	}

	void Animation::setRepeat(bool repeat)
	{
		this->repeat = repeat;
	}

	bool Animation::isInvertOnRepeat() const
	{
		return invertOnRepeat;
	}

	void Animation::setInvertOnRepeat(bool invertOnRepeat)
	{
		this->invertOnRepeat = invertOnRepeat;
	}

	void Animation::reset()
	{
		setState(0);
		updates = 0;
	}

	size_t Animation::getState() const
	{
		return state;
	}

	void Animation::setState(size_t state)
	{
		Animation::state = state;
	}

	void Animation::update()
	{
		if (!this->isRunning())
			return;
		updates++;
		if (updates == frametime)
		{
			updateState();
			if (state > this->getMaxState() - 1 || state < 0)
			{
				if (repeat)
				{
					if (invertOnRepeat)
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

	const std::vector<Texture> &TexturesAnimation::getTextures() const
	{
		return textures;
	}

	void TexturesAnimation::setState(size_t state)
	{
		Animation::setState(
				lambdacommon::maths::clamp(state, static_cast<size_t>(0), textures.size() - 1));
	}

	size_t TexturesAnimation::getMaxState()
	{
		return textures.size();
	}

	void TexturesAnimation::render(Graphics *graphics)
	{
		graphics->drawImage(textures[lambdacommon::maths::clamp(state, static_cast<size_t>(0),
																textures.size())], x, y, width,
							height);
	}

	BitmapAnimation::BitmapAnimation(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const Texture &texture,
									 uint32_t regionHeight) : Animation(x, y, width, height), texture(texture)
	{
		TextureRegion base{0.0f, 0.0f, 1.0f, 1.0f};
		float regionSize = static_cast<float>(regionHeight) / texture.getHeight();
		for (size_t i = 0; i < (texture.getHeight() / regionHeight); i++)
		{
			base.minY(i * regionSize);
			base.maxY(base.minY() + regionSize);
			regions.push_back(base);
		}
	}

	void BitmapAnimation::setState(size_t state)
	{
		Animation::setState(lambdacommon::maths::clamp(state, (size_t) 0, regions.size() - 1));
	}

	size_t BitmapAnimation::getMaxState()
	{
		return regions.size();
	}

	void BitmapAnimation::render(Graphics *graphics)
	{
		graphics->drawImage(texture, x, y, width, height,
							regions[lambdacommon::maths::clamp(state, static_cast<size_t>(0),
															   regions.size())]);
	}
}