/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/sound/wav.h"
#include "../../include/ionicengine/ionicengine.h"
#include <al.h>
#include <sndfile.h>

namespace ionicengine
{
	namespace sound
	{
		namespace wav
		{
			int IONICENGINE_API load(const lambdacommon::ResourceName &name)
			{
				return load(name, getResourcesManager().getResourcePath(name, "wav"));
			}

			int IONICENGINE_API load(const lambdacommon::ResourceName &name, const lambdacommon::fs::FilePath &path)
			{
				if (getNextFreeBuffer(false)  < IONIC_SOUND_MAX_BUFFERS)
				{
					if (!path.exists())
					{
						printError("[IonicEngine] Cannot load sound (WAV) '" + name.toString() + "': path " +
								   path.toString() + " cannot be found!");
						return -404;
					}

					SF_INFO fileInfos;
					SNDFILE *file = sf_open(path.toString().c_str(), SFM_READ, &fileInfos);
					if (!file)
					{
						printError("[IonicEngine] Cannot load sound (WAV) '" + name.toString() +
								   "': libsndfile cannot open the file.");
						return -2;
					}

					auto nbSamples = static_cast<ALsizei>(fileInfos.channels * fileInfos.frames);
					auto sampleRate = static_cast<ALsizei>(fileInfos.samplerate);

					std::vector<ALshort> samples(nbSamples);
					if (sf_read_short(file, &samples[0], nbSamples) < nbSamples)
					{
						printError("[IonicEngine] Cannot load sound (WAV) '" + name.toString() + "': invalid data.");
						return -3;
					}

					// Thanks libsndfile.
					sf_close(file);

					ALenum format;
					switch (fileInfos.channels)
					{
						case 1:
							format = AL_FORMAT_MONO16;
							break;
						case 2:
							format = AL_FORMAT_STEREO16;
							break;
						case 4 :
							format = alGetEnumValue("AL_FORMAT_QUAD16");
							break;
						case 6 :
							format = alGetEnumValue("AL_FORMAT_51CHN16");
							break;
						case 7 :
							format = alGetEnumValue("AL_FORMAT_61CHN16");
							break;
						case 8 :
							format = alGetEnumValue("AL_FORMAT_71CHN16");
							break;
						default:
							printError("[IonicEngine] Cannot load sound (WAV) '" + name.toString() +
									   "': cannot determine format.");
							return -4;
					}

					alBufferData(getBuffer(getNextFreeBuffer(true)), format, (void *) &samples[0], nbSamples * sizeof(ALushort),
								 sampleRate);

					ALenum error = alGetError();
					if (error != AL_NO_ERROR)
					{
						printError("[IonicEngine] Cannot load sound (WAV) '" + name.toString() +
								   "': OpenAL error occurred (" + std::to_string(error) + ").");
						return -5;
					}

					printDebug("[IonicEngine] Sound (WAV) '" + name.toString() + "' loaded successfully with ID '" +
							   std::to_string(getNextFreeBuffer(false) - 1) + "'!");
				}
				else
				{
					printError("Cannot load new sound, maximum buffers reached (" +
							   std::to_string(IONIC_SOUND_MAX_BUFFERS) + ")");
					return -1;
				}

				addSoundIndex(name, getNextFreeBuffer(false)  - 1);
				return getNextFreeBuffer(false)  - 1;
			}
		}
	}
}