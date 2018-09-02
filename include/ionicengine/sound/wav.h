/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_WAV_H
#define IONICENGINE_WAV_H

#include "sound.h"

namespace ionicengine
{
	namespace sound
	{
		namespace wav
		{
			/*!
			 * Loads a wav sound with the specified resource name.
			 * @param name The resource name.
			 * @return The allocated buffer to the loaded sound.
			 */
			extern int IONICENGINE_API load(const lambdacommon::ResourceName &name);

			/*!
			 * Loads a wav sound with the specified resource name at the specified location.
			 * @param name The resource name.
			 * @param path The path of the wav file.
			 * @return The allocated buffer to the loaded sound.
			 */
			extern int IONICENGINE_API load(const lambdacommon::ResourceName &name, const lambdacommon::fs::FilePath &path);
		}
	}
}

#endif //IONICENGINE_WAV_H
