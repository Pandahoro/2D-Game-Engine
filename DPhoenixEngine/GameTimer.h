#pragma once

#include "D3DUtil.h"

namespace DPhoenix
{
	class GameTimer
	{
	public:
		GameTimer();

		float TotalTime()const;  // in seconds
		float DeltaTime()const; // in seconds

		void Reset(); // Call before message loop.
		void Start(); // Call when unpaused.
		void Stop();  // Call when paused.
		void Tick();  // Call every frame.

	private:
		double mSecondsPerCount;	//how many seconds each count
		double mDeltaTime;			//difference in time

		__int64 mBaseTime;			//the first time snapshot to count from
		__int64 mPausedTime;		//how long paused?
		__int64 mStopTime;			//time stopped
		__int64 mPrevTime;			//previous time
		__int64 mCurrTime;			//curren time

		bool mStopped;				//have we stopped the timer?
	};
}

