#include "stdafx.h"
#include"CriticalSection.h"

__CCriticalSection::__CCriticalSection()
{
	InitializeCriticalSection(&m_CriticalSection);
	m_lTick=0;
}

__CCriticalSection::~__CCriticalSection()
{
	DeleteCriticalSection(&m_CriticalSection);
}

void __CCriticalSection::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
	m_lTick++;
}

void __CCriticalSection::UnLock()
{
	m_lTick--;
	LeaveCriticalSection(&m_CriticalSection);
}

long __CCriticalSection::GetTick()
{
	return m_lTick;
}