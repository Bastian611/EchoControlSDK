
#include "semaphore.h"

ECCS_BEGIN


void Semaphore::notify()
{
	SMART_LOCK(_mtx);
	++_count;
	_condition.notify_one();
}
void Semaphore::wait()
{
	ECCS_C11 unique_lock<decltype(_mtx)> ul(_mtx);

#ifdef SUPPORT_C11
	_condition.wait(ul, [&](){ return _count!=0; });
#else
	while(!_count){  // !! important !!
		_condition.wait(ul);
	}
#endif

	--_count;
}
bool Semaphore::try_wait()
{
	SMART_LOCK(_mtx);
	if(_count) {
		--_count;
		return true;
	}
	return false;
}


ECCS_END
