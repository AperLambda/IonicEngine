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

		std::map<lambdacommon::ResourceName, int> indexBuffer;

		static ALCdevice *device;
		static ALCcontext *context;

		static int nextFreeBuffer = 0;
		static ALuint buffers[IONIC_SOUND_MAX_BUFFERS];

		static int nextFreeSource = 0;
		static ALuint sources[IONIC_SOUND_MAX_SOURCES];

		int isIndexValid(int sourceIndex)
		{
			return sourceIndex >= 0 && sourceIndex < IONIC_SOUND_MAX_SOURCES;
		}

		/*!
		 * Gets a free index of an audio source.
		 * @return Either -1 or the index of a source that can be used.
		 */
		int getFreeSource()
		{
			int i = nextFreeSource;
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
					nextFreeSource = (i + 1) % IONIC_SOUND_MAX_SOURCES;
				}
				else
					i = (i + 1) % IONIC_SOUND_MAX_SOURCES;
			}
			while (result == -1 && i != nextFreeSource);

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

		void IONICENGINE_API addSoundIndex(const lambdacommon::ResourceName &sound, int index)
		{
			if (hasSound(sound) && getSoundIndex(sound) == index)
				throw std::runtime_error("Cannot add sound that was already indexed.");
			indexBuffer.insert({sound, index});
		}

		int IONICENGINE_API getSoundIndex(const lambdacommon::ResourceName &sound)
		{
			if (!hasSound(sound))
				return -1;
			return indexBuffer.at(sound);
		}

		bool IONICENGINE_API hasSound(const lambdacommon::ResourceName &sound)
		{
			return indexBuffer.count(sound);
		}

		int IONICENGINE_API play(const lambdacommon::ResourceName &sound, bool loop)
		{
			if (hasSound(sound))
				return play(getSoundIndex(sound), loop);
			return -1;
		}

		int IONICENGINE_API play(int buffer, bool loop)
		{
			int freeSourceIndex = -1;

			if (buffer >= 0 && buffer < IONIC_SOUND_MAX_BUFFERS)
			{
				// Find a source that is not being used.
				freeSourceIndex = getFreeSource();
				if (freeSourceIndex >= 0)
				{
					alSourcei(sources[freeSourceIndex], AL_BUFFER, buffers[buffer]);
					alSourcei(sources[freeSourceIndex], AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
					alSourcePlay(sources[freeSourceIndex]);
				}
			}
			else
				printError("ionicengine::sound::play() => Sound buffer index " + std::to_string(buffer) + " is out of range [0," + std::to_string(IONIC_SOUND_MAX_BUFFERS - 1) + "]");

			return freeSourceIndex;
		}

		bool IONICENGINE_API isPlaying(int sound)
		{
			if (isIndexValid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);
				return state == AL_PLAYING;
			}
			return false;
		}

		bool IONICENGINE_API isLooping(int sound)
		{
			if (isIndexValid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_LOOPING, &state);
				return state == AL_TRUE;
			}
			return false;
		}

		void IONICENGINE_API pause(int sound)
		{
			if (isPlaying(sound))
				alSourcePause(sources[sound]);
		}

		bool IONICENGINE_API isPaused(int sound)
		{
			if (isIndexValid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);
				return state == AL_PAUSED;
			}
			return false;
		}

		void IONICENGINE_API pauseAll()
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
			if (isPaused(sound))
				alSourcePlay(sources[sound]);
		}

		void IONICENGINE_API resumeAll()
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
			if (!isStopped(sound))
				alSourceStop(sources[sound]);
		}

		bool IONICENGINE_API isStopped(int sound)
		{
			if (isIndexValid(sound))
			{
				int state;
				alGetSourcei(sources[sound], AL_SOURCE_STATE, &state);
				return !(state == AL_PLAYING || state == AL_PAUSED);
			}
			return false;
		}

		void IONICENGINE_API stopAll()
		{
			for (int sound = 0; sound < IONIC_SOUND_MAX_SOURCES; sound++)
				stop(sound);
		}

		int IONICENGINE_API getNextFreeBuffer(bool increment)
		{
			if (increment)
				return nextFreeBuffer++;
			return nextFreeBuffer;
		}

		ALuint IONICENGINE_API getBuffer(int index)
		{
			return buffers[index];
		}
	}
}