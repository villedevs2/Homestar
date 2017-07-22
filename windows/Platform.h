// Windows specific includes and definitions

#pragma once

#include <GL/glew.h>
#include "Debug.h"

typedef long long timeunit;

#define SAFE_DELETE(x)		if ((x) != NULL) { delete (x); (x) = NULL; }