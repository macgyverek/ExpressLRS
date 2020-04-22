#include "HwTimer.h"
#include <Arduino.h>

/* HW specific code can be found from <mcu type>/ folder */

static void nullCallback(uint32_t){};

HwTimer::HwTimer()
{
    callbackTick = nullCallback; // function is called whenever there is new RC data.
    callbackTick = nullCallback; // function is called whenever there is new RC data.

    HWtimerInterval = TimerIntervalUSDefault;
    TickTock = 0;
    PhaseShift = 0;
    ResetNextLoop = false;

    LastCallbackMicrosTick = 0;
    LastCallbackMicrosTock = 0;
}

void HwTimer::updateInterval(uint32_t newTimerInterval)
{
    TickTock = 0;
    PhaseShift = 0;
    HWtimerInterval = newTimerInterval;
    setTime(newTimerInterval >> 1);
}

void ICACHE_RAM_ATTR HwTimer::phaseShift(int32_t newPhaseShift)
{
    int32_t const MaxPhaseShift = HWtimerInterval >> 1;

    if (newPhaseShift > MaxPhaseShift)
    {
        PhaseShift = MaxPhaseShift;
    }
    else if (newPhaseShift < -MaxPhaseShift)
    {
        PhaseShift = -MaxPhaseShift;
    }
    else
    {
        PhaseShift = newPhaseShift;
    }
    // TODO: modify TMR counter directly
}

void ICACHE_RAM_ATTR HwTimer::callback()
{
    uint32_t us = micros();
    if (TickTock)
    {
        if (PhaseShift != 0 || ResetNextLoop)
        {
            setTime((HWtimerInterval + PhaseShift) >> 1);
            ResetNextLoop = (PhaseShift != 0);
            PhaseShift = 0;
        }

        LastCallbackMicrosTick = us;
        callbackTick(us);
    }
    else
    {
        LastCallbackMicrosTock = us;
        callbackTock(us);
    }
    TickTock ^= 1;
}