/* 
	author: Kaman 
	for more info,contact <a href="mailto:wooce@21cn.com">Wooce Yang</a>,SCUT(South China University of Technology)
*/
#include "irefcounter.h"

IRefCounter::IRefCounter()
{
	m_nRefCount = 0;
}


IRefCounter::~IRefCounter()
{
	
}

int IRefCounter::IncReference()
{
	int nTempCount;
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	try
	{
#endif
		m_RefLock.Lock();
		m_nRefCount++;
		nTempCount = m_nRefCount;
	//	IDEBUG( g_Error.Out( L_DEBUG, "RefCounter: Inc Object, refcount is "+IntToStr(m_nRefCount)+"\n") );
		m_RefLock.ReleaseLock();
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	}
	catch( const IException &e )
	{
		g_Error.Out( L_FATAL, "%s, %s\n",e.GetErrorInfo(), "the refcounter object may be deleted.");
	}
	catch (...)
	{
		g_Error.Out( L_FATAL, "RefCounter: Increase reference failed. The refcounter object may be deleted.\n");
	}
#endif
	return nTempCount;
}

int IRefCounter::DecReference()
{
	int nTempCount;
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	try
	{
#endif
		m_RefLock.Lock();
		m_nRefCount--;
		nTempCount = m_nRefCount;
	//	IDEBUG( g_Error.Out( L_DEBUG, "RefCounter: Dec Object, refcount is "+IntToStr(m_nRefCount)+".\n") );
		m_RefLock.ReleaseLock();
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	}	
	catch( IException &e )		
	{
		g_Error.Out( L_FATAL, "%s, %s\n", e.GetErrorInfo(), "the refcounter object may be deleted." );
	}
	catch(...)
	{
		g_Error.Out( L_FATAL, "RefCounter: Decline reference failed. The refcounter object may be deleted.\n" );
	}	
#endif
	return nTempCount;
}

int IRefCounter::AddReference( int count )
{
	int nTempCount;
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	try
	{
#endif
		m_RefLock.Lock();
		m_nRefCount += count;
		nTempCount = m_nRefCount;
	//	IDEBUG( g_Error.Out( L_DEBUG, "RefCounter: Dec Object, refcount is "+IntToStr(m_nRefCount)+".\n") );
		m_RefLock.ReleaseLock();
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	}	
	catch( IException &e )		
	{
		g_Error.Out( L_FATAL, "%s, %s\n", e.GetErrorInfo(), "the refcounter object may be deleted." );
	}
	catch(...)
	{
		g_Error.Out( L_FATAL, "RefCounter: Decline reference failed. The refcounter object may be deleted.\n" );
	}	
#endif
	return nTempCount;
}

int IRefCounter::SubReference( int count )
{
	int nTempCount;
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	try
	{
#endif
		m_RefLock.Lock();
		m_nRefCount -= count;
		nTempCount = m_nRefCount;
	//	IDEBUG( g_Error.Out( L_DEBUG, "RefCounter: Dec Object, refcount is "+IntToStr(m_nRefCount)+".\n") );
		m_RefLock.ReleaseLock();
	// The exception will be throwed only when the user uses this object incorrectly.
	// This is needed only during debuging.
#ifndef NDEBUG
	}	
	catch( IException &e )		
	{
		g_Error.Out( L_FATAL, "%s, %s\n", e.GetErrorInfo(), "the refcounter object may be deleted." );
	}
	catch(...)
	{
		g_Error.Out( L_FATAL, "RefCounter: Decline reference failed. The refcounter object may be deleted.\n" );
	}	
#endif
	return nTempCount;
}
