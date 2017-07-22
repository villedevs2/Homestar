// Android specific includes and definitions

#include <GLES2/gl2.h>

typedef long long timeunit;

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)		if ((x) != NULL) { delete (x); (x) = NULL; }
#endif