/*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/


#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android/sensor.h>

#include <android/log.h>
#include <android/window.h>
#include "android_native_app_glue.h"


#include "BinaryFile.h"
#include "Game.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

#define SENSOR_SAMPLES_PER_SEC 100

/**
* Our saved state data.
*/
struct saved_state {
	int test;
};

/**
* Shared state for our app.
*/
struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;

	bool running;
	bool game_active;

	Game* game;
	float sensor_data[6];

	struct saved_state state;
};

/**
* Initialize an EGL context for the current display.
*/
static int android_init_display(struct engine* engine) {
	// initialize OpenGL ES and EGL

	/*
	* Here specify the attributes of the desired configuration.
	* Below, we select an EGLConfig with at least 8 bits per color
	* component compatible with on-screen windows
	*/
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_DEPTH_SIZE, 16,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};

	const EGLint context_attribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION,
		2,
		EGL_NONE,
	};

	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
	context = eglCreateContext(display, config, NULL, context_attribs);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;

	// Initialize GL state.
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
//	glEnable(GL_CULL_FACE);
//	glShadeModel(GL_SMOOTH);
//	glDisable(GL_DEPTH_TEST);

	return 0;
}

/**
* Tear down the EGL context currently associated with the display.
*/
static void android_term_display(struct engine* engine)
{
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}

	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
* Process the next input event.
*/
static int32_t android_handle_input(struct android_app* app, AInputEvent* event)
{
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		if (engine->display != NULL)
		{
			float x = AMotionEvent_getX(event, 0);
			float y = AMotionEvent_getY(event, 0);
			int action = AMotionEvent_getAction(event);

			UI::UITouchEvent gev = UI::UI_TOUCH_UP;

			if (action == AMOTION_EVENT_ACTION_DOWN)
				gev = UI::UI_TOUCH_DOWN;
			else if (action == AMOTION_EVENT_ACTION_UP)
				gev = UI::UI_TOUCH_UP;
			else if (action == AMOTION_EVENT_ACTION_MOVE)
				gev = UI::UI_TOUCH_MOVE;

			engine->game->onTouch(gev, x, y);
		}
		return 1;
	}
	return 0;
}

/**
* Process the next main command.
*/
static void android_handle_cmd(struct android_app* app, int32_t cmd)
{
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			android_init_display(engine);
			
			engine->game->init(engine->width, engine->height);
			engine->game_active = true;
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		android_term_display(engine);
		break;
	case APP_CMD_DESTROY:
		engine->game->shutdown();
		engine->game_active = false;
		break;
	case APP_CMD_GAINED_FOCUS:
		// When our app gains focus, we start monitoring the accelerometer.
		if (engine->accelerometerSensor != NULL)
		{
			ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
			ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->accelerometerSensor, (1000L / SENSOR_SAMPLES_PER_SEC) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if (engine->accelerometerSensor != NULL)
		{
			ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->accelerometerSensor);
		}
		break;
	}
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {
	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = android_handle_cmd;
	state->onInputEvent = android_handle_input;
	engine.app = state;

	// Prepare to monitor accelerometer
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager, state->looper, LOOPER_ID_USER, NULL, NULL);

	// get AssetManager from VM and init the file system
	AAssetManager* asset_manager = state->activity->assetManager;
	BinaryFile::init(asset_manager);

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}

	// Keep screen on
	ANativeActivity_setWindowFlags(state->activity, AWINDOW_FLAG_KEEP_SCREEN_ON, 0);

	// init game
	engine.game = new Game();

	engine.running = true;
	engine.game_active = false;

	// loop waiting for stuff to do.
	
	struct timespec prevframe_time;
	clock_gettime(CLOCK_MONOTONIC, &prevframe_time);

	while (engine.running)
	{
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
		{
			// Process this event.
			if (source != NULL)
			{
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER)
			{
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1) > 0)
				{
					//LOGI("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
					if (event.type == ASENSOR_TYPE_ACCELEROMETER)
					{
						engine.sensor_data[0] = event.acceleration.x;
						engine.sensor_data[1] = event.acceleration.y;
						engine.sensor_data[2] = event.acceleration.z;
					}
				}
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0)
			{
				engine.running = false;
				android_term_display(&engine);
				return;
			}
		}

		struct timespec before_time, after_time;

		clock_gettime(CLOCK_MONOTONIC, &before_time);

		long frame_bef = prevframe_time.tv_nsec;
		long frame_aft = ((before_time.tv_sec - prevframe_time.tv_sec) * 1000000000) + before_time.tv_nsec;
		long f2ftime = frame_aft - frame_bef;
		prevframe_time = before_time;

		// run logic and update screen
		//android_draw_frame(&app_state);
		if (engine.game_active)
		{			
			engine.game->onSensor(engine.sensor_data);
			engine.game->onUpdate(Game::LOGIC_PERIOD_NS);
			engine.game->onRender();	

			eglSwapBuffers(engine.display, engine.surface);			
		}

		clock_gettime(CLOCK_MONOTONIC, &after_time);		

		long before_nsec = before_time.tv_nsec;
		long after_nsec = ((after_time.tv_sec - before_time.tv_sec) * 1000000000) + after_time.tv_nsec;
		long frametime = after_nsec - before_nsec;

		engine.game->reportMS((int)frametime / 1000000);
		engine.game->reportF2FMS((int)f2ftime / 1000000);

		// sleep for the remainder of the frame
		if (frametime < Game::LOGIC_PERIOD_NS)
		{
			struct timespec sleeptime, rem;

			sleeptime.tv_sec = 0;
			sleeptime.tv_nsec = Game::LOGIC_PERIOD_NS - frametime - 1000000;

			nanosleep(&sleeptime, &rem);
		}
	}

	// clean up and shut down
	delete engine.game;
}
