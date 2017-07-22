// Android specific implementation of Debug class

#include "Debug.h"

#define ENABLE_DEBUG 0

void Debug::start()
{
	// Android version does nothing
#if ENABLE_DEBUG
	ofs.open("/sdcard/output.txt", std::ios_base::out);
#endif
}

void Debug::end()
{
	// Android version does nothing
#if ENABLE_DEBUG
	ofs.close();
#endif
}

void Debug::log(std::string string)
{
	__android_log_print(ANDROID_LOG_INFO, "NDKTest3", "%s", string.c_str());

#if ENABLE_DEBUG
	ofs << string << "\n";
#endif
}

void Debug::error(std::string string)
{
	__android_log_print(ANDROID_LOG_ERROR, "NDKTest3", "%s", string.c_str());

#if ENABLE_DEBUG
	ofs << "ERROR: " << string << "\n";
#endif
}
