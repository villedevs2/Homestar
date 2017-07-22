#include "Timer.h"

TimerSystem::Timer::Timer(int id, int duration, int period, EventType event, TimerEventCallback* callback)
{
	// don't allow infinite timer with expire event...
	assert(!(event == EVENT_EXPIRE && duration <= 0));

	m_count = 0;
	m_duration = (timeunit)(duration) * 1000000;
	m_id = id;
	m_period_count = 0;
	m_period = (timeunit)(period) * 1000000;
	m_event = event;
	m_callback = callback;

	m_active = true;
}

TimerSystem::Timer::~Timer()
{
}

void TimerSystem::Timer::update(int time)
{
	if (m_active)
	{
		// update period
		m_period_count += time;
		if (m_period_count >= m_period)
		{
			timeunit frac = m_period_count - m_period;
				
			// reset to 0 + any fractional leftover
			m_period_count = 0 + frac;

			// send periodic event if needed
			if (m_event == EVENT_PERIODIC)
			{
				m_callback->onTimerEvent(m_id, m_period_count, m_period);
			}
		}
			

		// update counter
		m_count += time;

		// send event if this is a animation timer
		if (m_event == EVENT_ANIMATION_UPDATE)
		{
			timeunit count = m_count;
			if (count > m_duration)
				count = m_duration;

			m_callback->onTimerEvent(m_id, count, m_duration);
		}

		// check for expire unless infinite
		if (m_duration > 0)
		{
			if (m_count >= m_duration)
			{
				Debug::log(str(fmt::Format("Timer {:d} expired") << m_id));

				// send event if we have a expire event
				if (m_event == EVENT_EXPIRE)
				{
					m_callback->onTimerEvent(m_id, 0, 0); 
				}

				m_active = false;
			}
		}
	}
}

bool TimerSystem::Timer::isActive()
{
	return m_active;
}

int TimerSystem::Timer::getID()
{
	return m_id;
}


TimerSystem::TimerSystem(TimerEventCallback* callback)
{
	m_timers.clear();

	reset();

	m_callback = callback;
}

TimerSystem::~TimerSystem()
{
}


timeunit TimerSystem::getBaseTime()
{
	return m_base_timer;
}

float TimerSystem::getHzTime()
{
	return (float)(m_base_timer) / 1000000000.0f;
}


void TimerSystem::add(int id, int duration, int period, EventType event)
{
	Timer timer(id, duration, period, event, m_callback);

	m_timers.push_back(timer);

	Debug::log(str(fmt::Format("Adding timer {:d}") << id));
}

void TimerSystem::reset()
{
	m_timers.clear();

	m_base_timer = 0;
}

void TimerSystem::update(int time)
{
	m_base_timer += time;
	/*
	std::vector<Timer>::iterator iterator = m_timers.begin();
	while (iterator != m_timers.end())
	{
		Timer& t = *iterator;
		t.update(time);
		
		if (!t.isActive())
		{
			Debug::log(str(fmt::Format("Deleting timer {:d}") << t.getID()));
			iterator = m_timers.erase(iterator);
		}
		else
		{
			++iterator;
		}
	}*/

	int i=0;
	while (i < m_timers.size())
	{
		Timer& t = m_timers.at(i);
		t.update(time);

		if (!t.isActive())
		{
			m_timers.erase(m_timers.begin()+i);
		}
		else
		{
			++i;
		}
	}
}