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
				return load(name, get_resources_manager().get_resource_path(name, "wav"));
			}

			int IONICENGINE_API load(const lambdacommon::ResourceName &name, const lambdacommon::fs::FilePath &path)
			{
				if (get_next_free_buffer(false)  < IONIC_SOUND_MAX_BUFFERS)
				{
					if (!path.exists())
					{
						print_error("[IonicEngine] Cannot load sound (WAV) '" + name.to_string() + "': path " +
									path.to_string() + " cannot be found!");
						return -404;
					}

					SF_INFO file_info;
					SNDFILE *file = sf_open(path.to_string().c_str(), SFM_READ, &file_info);
					if (!file)
					{
						print_error("[IonicEngine] Cannot load sound (WAV) '" + name.to_string() +
									"': libsndfile cannot open the file.");
						return -2;
					}

					auto nb_samples = static_cast<ALsizei>(file_info.channels * file_info.frames);
					auto sample_rate = static_cast<ALsizei>(file_info.samplerate);

					std::vector<ALshort> samples(nb_samples);
					if (sf_read_short(file, &samples[0], nb_samples) < nb_samples)
					{
						print_error("[IonicEngine] Cannot load sound (WAV) '" + name.to_string() + "': invalid data.");
						return -3;
					}

					// Thanks libsndfile.
					sf_close(file);

					ALenum format;
					switch (file_info.channels)
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
							print_error("[IonicEngine] Cannot load sound (WAV) '" + name.to_string() +
										"': cannot determine format.");
							return -4;
					}

					alBufferData(get_buffer(get_next_free_buffer(true)), format, (void *) &samples[0], nb_samples * sizeof(ALushort),
								 sample_rate);

					ALenum error = alGetError();
					if (error != AL_NO_ERROR)
					{
						print_error("[IonicEngine] Cannot load sound (WAV) '" + name.to_string() +
									"': OpenAL error occurred (" + std::to_string(error) + ").");
						return -5;
					}

					print_debug("[IonicEngine] Sound (WAV) '" + name.to_string() + "' loaded successfully with ID '" +
								std::to_string(get_next_free_buffer(false) - 1) + "'!");
				}
				else
				{
					print_error("Cannot load new sound, maximum buffers reached (" +
								std::to_string(IONIC_SOUND_MAX_BUFFERS) + ")");
					return -1;
				}

				add_sound_index(name, get_next_free_buffer(false) - 1);
				return get_next_free_buffer(false)  - 1;
			}
		}
	}
}