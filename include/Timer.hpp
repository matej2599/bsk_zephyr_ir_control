#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

class Timer
{
public:
  Timer(size_t events)
      : m_events(events)
  {
    m_lastUpdateTime = new unsigned long[m_events];

    for (size_t i = 0; i < m_events; i++)
    {
      m_lastUpdateTime[i] = millis();
    }
  }

  ~Timer()
  {
    delete[] m_lastUpdateTime;
  }

  bool timerCheck(size_t event, const unsigned long& milliseconds)
  {
    if (event > m_events)
    {
      return false;
    }

    unsigned long currentTime = millis();

    if (currentTime >= (m_lastUpdateTime[event] + milliseconds))
    {
      m_lastUpdateTime[event] = currentTime;
      return true;
    }

    return false;
  }

  bool isExpired(size_t event, const unsigned long& milliseconds)
  {
    if (event > m_events)
    {
      return false;
    }

    unsigned long currentTime = millis();

    if (currentTime >= (m_lastUpdateTime[event] + milliseconds))
    {
      return true;
    }

    return false;
  }

  void reset(size_t event)
  {
    if (event > m_events)
    {
      return;
    }

    m_lastUpdateTime[event] = millis();
  }

private:
  size_t m_events;
  unsigned long* m_lastUpdateTime = nullptr;
};

template <typename TargetClassType>
class RepeatitiveTimer
{
public:
  RepeatitiveTimer()
  {
  }

  ~RepeatitiveTimer()
  {
    delete[] m_tasks;
  }

  template <typename T>
  void addFunction(T* obj, void (T::*method)(), unsigned long milliseconds)
  {
    resize(m_taskCounter + 1);

    m_tasks[m_taskCounter - 1].method = reinterpret_cast<void (RepeatitiveTimer::*)()>(method);
    m_tasks[m_taskCounter - 1].obj = obj;
    m_tasks[m_taskCounter - 1].timeout = milliseconds;
    m_tasks[m_taskCounter - 1].lastUpdateTime = millis();
  }

  template <typename T>
  void reset(void (T::*method)())
  {
    for (size_t i = 0; i < m_taskCounter; i++)
    {
      if (m_tasks[i].method == reinterpret_cast<void (RepeatitiveTimer::*)()>(method))
      {
        m_tasks[i].lastUpdateTime = millis();
      }
    }
  }

  template <typename T>
  void setTimeout(void (T::*method)(), unsigned long timeout)
  {
    for (size_t i = 0; i < m_taskCounter; i++)
    {
      if (m_tasks[i].method == reinterpret_cast<void (RepeatitiveTimer::*)()>(method))
      {
        if (timeout)
        {
          m_tasks[i].timeout = timeout;
        }
      }
    }
  }

  void tick()
  {
    if (m_taskCounter < 1)
    {
      return;
    }

    unsigned long currentTime = millis();

    for (size_t i = 0; i < m_taskCounter; i++)
    {
      if (currentTime >= (m_tasks[i].lastUpdateTime + m_tasks[i].timeout))
      {
        m_tasks[i].lastUpdateTime = currentTime;
        m_tasks[i].update();
      }
    }
  }

private:
  void resize(size_t size)
  {
    // TODO implement and use some container instead of array
    Task* tempTasks = new Task[size];

    for (size_t i = 0; i < m_taskCounter; ++i)
    {
      tempTasks[i] = m_tasks[i];
    }

    delete[] m_tasks;

    m_tasks = tempTasks;
    m_taskCounter = size;
  }

private:
  struct Task
  {
    void (RepeatitiveTimer::*method)();
    void* obj;
    unsigned long timeout = 0;
    unsigned long lastUpdateTime = 0;

    void update()
    {
      auto exec = reinterpret_cast<void (TargetClassType::*)()>(method);
      (static_cast<TargetClassType *>(obj)->*exec)();
    }
  };

  Task* m_tasks = nullptr;
  size_t m_taskCounter = 0;
};

#endif