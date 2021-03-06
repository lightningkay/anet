
#include "RecMutex.h"
#include "Exception.h"

namespace util
{
RecMutex::RecMutex() :
_count(0)
{
pthread_mutexattr_t attr;
int rt = pthread_mutexattr_init(&attr);
#ifdef _NO_EXCEPTION
assert( 0 == rt );
if ( rt != 0 )
{
SYS_LOG(ERROR,"%s","ThreadSyscallException");
}
#else
if ( 0 != rt )
{
throw ThreadSyscallException(__FILE__, __LINE__, rt);
}
#endif
rt = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#ifdef _NO_EXCEPTION
assert( 0 == rt );
if ( rt != 0 )
{
SYS_LOG(ERROR,"%s","ThreadSyscallException");
}
#else
if ( 0 != rt )
{
throw ThreadSyscallException(__FILE__, __LINE__, rt);
}
#endif
rt = pthread_mutex_init(&_mutex, &attr);
#ifdef _NO_EXCEPTION
assert( 0 == rt );
if ( rt != 0 )
{
SYS_LOG(ERROR,"%s","ThreadSyscallException");
}
#else
if ( 0 != rt )
{
throw ThreadSyscallException(__FILE__, __LINE__, rt);
}
#endif

rt = pthread_mutexattr_destroy(&attr);
#ifdef _NO_EXCEPTION
assert( 0 == rt );
if ( rt != 0 )
{
SYS_LOG(ERROR,"%s","ThreadSyscallException");
}
#else
if ( 0 != rt )
{
throw ThreadSyscallException(__FILE__, __LINE__, rt);
}
#endif
}

RecMutex::~RecMutex()
{
assert(_count == 0);
const int rc = pthread_mutex_destroy(&_mutex);
assert(rc == 0);
if ( rc != 0 )
{
SYS_LOG(ERROR,"%s","ThreadSyscallException");
}
}

void RecMutex::lock() const
{
const int rt = pthread_mutex_lock(&_mutex);
#ifdef _NO_EXCEPTION
assert( 0 == rt );
if ( rt != 0 )
{
SYS_LOG(ERROR,"%s","ThreadSyscallException");
}
#else
if(0 != rt)
{
throw ThreadSyscallException(__FILE__, __LINE__, rt);
}
#endif
if(++_count > 1)
{
assert(0  == pthread_mutex_unlock(&_mutex));
}
}

bool RecMutex::tryLock() const
{
const int rc = pthread_mutex_trylock(&_mutex);
const bool result = (rc == 0);
if(!result)
{
#ifdef _NO_EXCEPTION
assert( EBUSY == rc );
#else
if(rc != EBUSY)
{
throw ThreadSyscallException(__FILE__, __LINE__, rc);
}
#endif
}
else if(++_count > 1)
{
const int rt = pthread_mutex_unlock(&_mutex);
#ifdef _NO_EXCEPTION
assert( 0 == rt );
if ( rt != 0 )
{
SYS_LOG(ERROR,"%s","ThreadSyscallException");
}
#else
if( 0 != rt)
{
throw ThreadSyscallException(__FILE__, __LINE__, rc);
}
#endif
}
return result;
}

void RecMutex::unlock() const
{
if(--_count == 0)
{
assert( 0 == pthread_mutex_unlock(&_mutex));
}
}

void RecMutex::unlock(LockState& state) const
{
state.mutex = &_mutex;
state.count = _count;
_count = 0;
}

void RecMutex::lock(LockState& state) const
{
_count = state.count;
}


bool RecMutex::willUnlock() const
{
return _count == 1;
}
}//end namespace