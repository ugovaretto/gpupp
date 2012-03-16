#ifndef TIMER_H_
#define TIMER_H_
//
// Copyright (c) 2010 - Ugo Varetto
//
// This source code is free; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// This source code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this source code; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#ifdef _WIN32
#include <windows.h>
#include <WinNT.h>
#else
#include <sys/time.h>
#endif

/// Computes elapsed time in milliseconds returned as a double precision floating point number.
/// Works on Linux, Windows and Mac OS.
class Timer
{
public:
    ///Default constructor.
    Timer();
    ///Start timer.
    void Start();
    ///Stop timer.
    ///\return elapsed time in milliseconds since call to Start()
    double Stop();
    ///Computes elapsed time in milliseconds between a Start() and Stop() calls
	///\return time elapesed between subsequent Start() and Stop() function calls
    double ElapsedTime() const;
    ///Returns the time elapsed from the call to the Start() function;
    ///does not stop the timer.
    ///\return time in milliseconds elapsed from previous call to Start()
    double DTime() const;
private:
    ///Reset timer.
    void Reset();
#ifdef WIN32
    LARGE_INTEGER freq_;
    LARGE_INTEGER tstart_;
    LARGE_INTEGER tend_;
    mutable LARGE_INTEGER tendTmp_;
#else
    timeval tstart_;
    timeval tend_;
    mutable timeval tendTmp_;
#endif
private:
    ///Do not allow construction from other instance.
    Timer( const Timer& );
    ///Forbid assignment.
    Timer operator=( const Timer& );
};

///Scoped timer: starts when declared, stops and invokes callback upon destruction.
template < class CBackT, class TimerT = Timer >
class ScopedCBackTimer
{
public:
    ///Default constructor: Starts timer.
    ScopedCBackTimer()
    {
        timer_.Start();
    }
    ///Destrcuctor: stops timer and invokes callback.
    ~ScopedCBackTimer()
    {  cback_( timer_.Stop() ); }
private:
    CBackT cback_;
    TimerT timer_;
private:
    ///Forbid construction from other instance.
    ScopedCBackTimer( const ScopedCBackTimer& );
    ///Forbid assignment.
    ScopedCBackTimer operator=( const ScopedCBackTimer& );
};


#ifdef WIN32 //WINDOWS

inline Timer::Timer()
{
    ::QueryPerformanceFrequency( &freq_ );
    tstart_.QuadPart = 0;
    tend_.QuadPart = 0;
}

inline void Timer::Start()
{
    ::QueryPerformanceCounter( &tstart_ );
}

inline double Timer::Stop()
{
    ::QueryPerformanceCounter( &tend_ );
    return DTime();
}

inline double Timer::ElapsedTime() const
{
    return 1000. * ( ( double ) tend_.QuadPart -
           ( double ) tstart_.QuadPart) /( ( double ) freq_.QuadPart );
}

inline double Timer::DTime() const
{
    ::QueryPerformanceCounter( &tendTmp_ );
    return 1000. * ( ( double ) tendTmp_.QuadPart -
           ( double ) tstart_.QuadPart) /( ( double ) freq_.QuadPart );
}

inline void Timer::Reset()
{
    tstart_.QuadPart = 0;
    tend_.QuadPart = 0;
}

#else //UNIX

inline Timer::Timer()
{
    Reset();
}

inline void Timer::Start()
{
    ::gettimeofday( &tstart_, 0  );
}

inline double Timer::Stop()
{
    ::gettimeofday( &tend_, 0 );
    return DTime();
}

inline double Timer::ElapsedTime() const
{
    const double t1 = ( double ) tstart_.tv_sec + ( double ) tstart_.tv_usec / 1E6;
    const double t2 = ( double ) tend_.tv_sec   + ( double ) tend_.tv_usec / 1E6;
    return 1000. * ( t2 - t1 );
}

inline double Timer::DTime() const
{
    ::gettimeofday( &tendTmp_, 0 );
    const double t1 = ( double ) tstart_.tv_sec + ( double ) tstart_.tv_usec / 1E6;
    const double t2 = ( double ) tendTmp_.tv_sec   + ( double ) tendTmp_.tv_usec / 1E6;
    return 1000. * ( t2 - t1 );
}


inline void Timer::Reset()
{
    tstart_.tv_sec = 0;
    tstart_.tv_usec = 0;
    tend_.tv_sec = 0;
    tend_.tv_usec = 0;

}
#endif



#endif //TIMER_H_
