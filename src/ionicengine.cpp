/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../include/ionicengine/ionicengine.h"

#include <iostream>

using namespace std;

namespace ionicengine
{
	std::string IONICENGINE_API getVersion()
	{
		return to_string(IONICENGINE_VERSION_MAJOR) + '.' + to_string(IONICENGINE_VERSION_MINOR) + '.' +
		       to_string(IONICENGINE_VERSION_PATCH);
	}
}