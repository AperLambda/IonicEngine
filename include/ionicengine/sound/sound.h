/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_SOUND_H
#define IONICENGINE_SOUND_H

#include "../includes.h"
#include <al.h>

#define IONIC_SOUND_MAX_BUFFERS 500    // How many different sounds we can keep in memory at once.
#define IONIC_SOUND_MAX_SOURCES 100    // How many different sounds we can have active at one time.

namespace ionicengine
{
	namespace sound
	{
		/*!
		 * Initializes sound engine.
		 * @return True if successful initialization, else false.
		 */
		extern bool IONICENGINE_API init();

		/*!
		 * Shutdowns sound engine.
		 */
		extern void IONICENGINE_API shutdown();

		extern void IONICENGINE_API add_sound_index(const lambdacommon::ResourceName &sound, int index);

		extern int IONICENGINE_API get_sound_index(const lambdacommon::ResourceName &sound);

		/*!
		 * Checks whether there is a sound registered with the specified resource name.
		 * @param sound The name of the sound to check.
		 * @return True if there is a sound registered, else false.
		 */
		extern bool IONICENGINE_API has_sound(const lambdacommon::ResourceName &sound);

		/*!
		 * Plays the specified sound.
		 * @param sound The sound to play.
		 * @param loop True if the sound is looped, else false.
		 * @return The source index of the sound or -1 if any error happened.
		 */
		extern int IONICENGINE_API play(const lambdacommon::ResourceName &sound, bool loop = false);

		/*!
		 * Plays the specified sound.
		 * @param buffer The buffer index of the sound to play.
		 * @param loop True if the sound is looped, else false.
		 * @return The source index of the sound or -1 if any error happened.
		 */
		extern int IONICENGINE_API play(int buffer, bool loop = false);

		/*!
		 * Checks whether the specified sound is currently playing.
		 * @param sound The sound to check.
		 * @return True if the sound is currently playing, else false.
		 */
		extern bool IONICENGINE_API is_playing(int sound);

		/*!
		 * Checks whether the specified sound is currently looped.
		 * @param sound The sound to check.
		 * @return True if the sound is currently looped, else false.
		 */
		extern bool IONICENGINE_API is_looping(int sound);

		/*!
		 * Pauses the specified sound.
		 * @param sound The sound to pause.
		 */
		extern void IONICENGINE_API pause(int sound);

		/*!
		 * Checks whether the specified sound is paused.
		 * @param sound The sound to check.
		 * @return True if the sound is paused, else false.
		 */
		extern bool IONICENGINE_API is_paused(int sound);

		/*!
		 * Pauses every sounds being currently playing.
		 */
		extern void IONICENGINE_API pause_all();

		/*!
		 * Resumes the specified paused sound.
		 * @param sound The sound to resume.
		 */
		extern void IONICENGINE_API resume(int sound);

		/*!
		 * Resumes every sounds being currently paused.
		 */
		extern void IONICENGINE_API resume_all();

		/*!
		 * Stops the currently played sound.
		 * @param sound The sound to stop.
		 */
		extern void IONICENGINE_API stop(int sound);

		/*!
		 * Checks whether the specified sound is stopped.
		 * @param sound The sound to check.
		 * @return True if the sound is stopped, else false.
		 */
		extern bool IONICENGINE_API is_stopped(int sound);

		/*!
		 * Stops every sounds being currently played.
		 */
		extern void IONICENGINE_API stop_all();

		/*!
		 * Gets the next free buffer.
		 * @param increment Increment the buffer after the call?
		 * @return The next free buffer.
		 */
		extern int IONICENGINE_API get_next_free_buffer(bool increment = false);

		/*!
		 * Gets the OpenAL buffer by his IonicEngine's id.
		 * @param index The IonicEngine's id of the OpenAL buffer.
		 * @return The OpenAL buffer or 0 if errors.
		 */
		extern ALuint IONICENGINE_API get_buffer(int index);
	}
}

#endif //IONICENGINE_SOUND_H
