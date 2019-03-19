#ifndef __DEF_CRITICALSECTION__
#define __DEF_CRITICALSECTION__
#include<windows.h>

/*·â×°¹Ø¼ü¶Î*/

class __CCriticalSection
{
public:
	__CCriticalSection();
	~__CCriticalSection();
	void Lock();
	void UnLock();
	long GetTick();
private:
	CRITICAL_SECTION m_CriticalSection;
	long m_lTick;
};

class CMakeCritical
{
public:
	CMakeCritical(__CCriticalSection& critical):m_criticalBuf(critical)
	{
		m_criticalBuf.Lock();
	}
	virtual ~CMakeCritical()
	{
		m_criticalBuf.UnLock();
	}
private:
	__CCriticalSection& m_criticalBuf;
};

#endif