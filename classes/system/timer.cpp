 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth/system/timer.h>
#include <smooth/system/backends/timerbackend.h>

const S::Int	 S::System::Timer::classID = S::Object::RequestClassID();

S::System::Timer::Timer()
{
	backend = TimerBackend::CreateBackendInstance();

	type = classID;

	status = TIMER_STOPPED;

	onInterval.SetParentObject(this);
}

S::System::Timer::~Timer()
{
	Stop();

	delete backend;
}

S::Int S::System::Timer::Start(Int interval)
{
	if (status == TIMER_RUNNING) return Error();

	status = TIMER_RUNNING;

	return backend->Start(interval);
}

S::Int S::System::Timer::Stop()
{
	if (status != TIMER_RUNNING) return Error();

	status = TIMER_STOPPED;

	return backend->Stop();
}

S::Int S::System::Timer::Restart(Int interval)
{
	if (status != TIMER_RUNNING) return Error();

	Stop();

	return Start(interval);
}

S::Int S::System::Timer::GetID() const
{
	return backend->GetID();
}

S::Int S::System::Timer::GetStatus() const
{
	return status;
}
