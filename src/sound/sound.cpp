/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/sound/sound.h"
#include "../../include/ionicengine/ionicengine.h"
#include <alc.h>

namespace ionicengine
{
	namespace sound
	{
		bool running = false;

		std::map<lambdacommon::ResourceName, int> index_buffer;

		static ALCdevice *device;
		static ALCcontext *context;

		static int next_free_buffer = 0;
		static ALuint buffers[IONIC_SOUND_MAX_BUFFERS];

		static int next_free_source = 0;
		static ALuint sources[IONIC_SOUND_MAX_SOURCES];

		int is_index_valid(int source_index)
		{
			return source_index >= 0 && source_index < IONIC_SOUND_MAX_SOURCES;
		}

		/*!
		 * Gets a free index of an audio source.
		 * @return Either -1 or the index of a source that can be used.
		 */
		int get_free_source()
		{
			int i = next_free_source;
			int state;
			int result = -1;

			// Start at an index that is an educated guess for a sound that might be free.
			do
			{
				// Get the state of the next source that might be available.
				alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
				if (state == AL_STOPPED || state == AL_INITIAL)
				{
					result = i;
					next_free_source = (i + 1) % IONIC_SOUND_MAX_SOURCES;
				}
				else
					i = (i + 1) % IONIC_SOUND_MAX_SOURCES;
			}
			while (result == -1 && i != next_free_source);

			return result;
		}

		bool IONICENGINE_API init()
		{
			if (running)
				return false;

			// Open audio device.
			device = alcOpenDevice(nullptr);
			if (!device)
				return false;

			// Create an OpenAL context.
			context = alcCreateContext(device, nullptr);
			if (!context || !alcMakeContextCurrent(context))
				return false;

			// Enable support for up to IONIC_SOUND_MAX_BUFFERS different sounds.
			alGenBuffers(IONIC_SOUND_MAX_BUFFERS, buffers);
			alGenSources(IONIC_SOUND_MAX_SOURCES, sources);

			running = true;
			return running;
		}

		void IONICENGINE_API shutdown()
		{
			if (!running)
				return;

			// Delete our buffers and sources.
			alDeleteSources(IONIC_SOUND_MAX_SOURCES, sources);
			alDeleteBuffers(IONIC_SOUND_MAX_BUFFERS, buffers);

			// Shutdown our context and close the audio device.
			alcMakeContextCurrent(nullptr);
			alcDestroyContext(context);
			alcCloseDevice(device);

			running = false;
		}

		void IONICENGINE_API add_sound_index(const lambdacommon::ResourceName &sound, int index)
		{
			if (has_sound(sound) && get_sound_index(sound) == index)
				throw std::runtime_error("Cannot add sound that was already indexed.");
			index_buffer.insert({sound, index});
		}

		int IONICENGINE_API get_sound_index(const lambdacommon::ResourceName &sound)
		{
			if (!has_sound(sound))
				return -1;
			return index_buffer.at(sound);
		}

		bool IONICENGINE_API has_sound(const lambdacommon::ResourceName &sound)
		{
			return index_buffer.count(sound);
		}

		int IONICENGINE_API play(const lambdacommon::ResourceName &sound, bool loop)
		{
			if (has_sound(sound))
				return play(get_sound_index(sound), loop);
			return -1;
		}

		int IONICENGINE_API play(int buffer, bool loop)
		{
			int free_source_index = -1;

			if (buffer >= 0 && buffer < IONIC_SOUND_MAX_BUFFERS)
			{
				// Find a source that is not being used.
				free_source_index = get_free_source();
				if (free_source_index >= 0)
				{
					alSourcei(sources[free_source_index], AL_BUFFER, buffers[buffer]);
					alSourcei(sources[free_source_index], AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
					alSourcePlay(sources[free_source_index]);
				}
			}
			else
				print_error("ionicengine::sound::play() => Sound buffer index " + std::to_string(buffer) +
							" is out of range [0," + std::to_string(IONIC_SOUND_MAX_BUFFERS - 1) + "]");

			return free_source_index;
		}

		bool IONICENGINE_API is_playing(int sound)
		{
			if (is_index_valid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);
				return state == AL_PLAYING;
			}
			return false;
		}

		bool IONICENGINE_API is_looping(int sound)
		{
			if (is_index_valid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_LOOPING, &state);
				return state == AL_TRUE;
			}
			return false;
		}

		void IONICENGINE_API pause(int sound)
		{
			if (is_playing(sound))
				alSourcePause(sources[sound]);
		}

		bool IONICENGINE_API is_paused(int sound)
		{
			if (is_index_valid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);
				return state == AL_PAUSED;
			}
			return false;
		}

		void IONICENGINE_API pause_all()
		{
			int state;

			for(ALuint source : sources)
			{
				alGetSourcei(source, AL_SOURCE_STATE, &state);
				if(state == AL_PLAYING)
					alSourcePause(source);
			}
		}

		void IONICENGINE_API resume(int sound)
		{
			if (is_paused(sound))
				alSourcePlay(sources[sound]);
		}

		void IONICENGINE_API resume_all()
		{
			int state;
			for (ALuint source : sources)
			{
				alGetSourcei(source, AL_SOURCE_STATE, &state);
				if (state == AL_PAUSED)
					alSourcePlay(source);
			}

		}

		void IONICENGINE_API stop(int sound)
		{
			if (!is_stopped(sound))
				alSourceStop(sources[sound]);
		}

		bool IONICENGINE_API is_stopped(int sound)
		{
			if (is_index_valid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);
				return !(state == AL_PLAYING || state == AL_PAUSED);
			}
			return false;
		}

		void IONICENGINE_API stop_all()
		{
			for (int sound = 0; sound < IONIC_SOUND_MAX_SOURCES; sound++)
				stop(sound);
		}

		int IONICENGINE_API get_next_free_buffer(bool increment)
		{
			if (increment)
				return next_free_buffer++;
			return next_free_buffer;
		}

		ALuint IONICENGINE_API get_buffer(int index)
		{
			return buffers[index];
		}
	}
}