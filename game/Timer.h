	#pragma once

#include "Platform.h"
#include <assert.h>
#include <vector>

class TimerSystem
{
public:
	enum EventType
	{
		EVENT_NONE = 0,
		EVENT_EXPIRE,
		EVENT_PERIODIC,
		EVENT_ANIMATION_UPDATE,
	};

	class TimerEventCallback
	{
	public:
		virtual void onTimerEvent(int id, timeunit count, timeunit duration) = 0;
	};

	class Timer
	{
	public:
		Timer(int id, int duration, int period, EventType event, TimerEventCallback* callback);
		~Timer();

		void update(int time);
		bool isActive();
		int getID();

	private:
		int m_id;
		timeunit m_count;
		timeunit m_period_count;
		timeunit m_duration;
		timeunit m_period;
		EventType m_event;
		TimerEventCallback* m_callback;
		bool m_active;
	};

	TimerSystem(TimerEventCallback* callback);
	~TimerSystem();

	void add(int id, int duration, int period, EventType event);
	void update(int time);
	void reset();
	timeunit getBaseTime();
	float getHzTime();

private:
	std::vector<Timer> m_timers;

	TimerEventCallback* m_callback;

	timeunit m_base_timer;
};
