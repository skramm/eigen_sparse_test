

#include <chrono>
#include <iostream>

typedef std::chrono::high_resolution_clock                          MyClock;
typedef std::chrono::time_point<MyClock>                            MyTimePoint;
typedef std::chrono::duration<MyTimePoint::rep,MyTimePoint::period> MyDuration;


struct Timing
{
//	MyDuration _duration;
	MyTimePoint _startTime;

	Timing()
	{
		initTimer();
	}

	void initTimer()
	{
		_startTime = MyClock::now();
	}

	void PrintDuration()
	{
//		_duration = MyClock::now() - _startTime;
		std::cout << "Duration = "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(MyClock::now() - _startTime).count()
			<< " ms\n";
		initTimer();
	}
	MyDuration getDuration()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(MyClock::now() - _startTime);
	}
};
