/* 
	author: Kaman 
	for more info,contact <a href="mailto:wooce@21cn.com">Wooce Yang</a>,SCUT(South China University of Technology)
*/

#ifndef _REFCOUNTER_H
#define _REFCOUNTER_H

#include "istring.h"
#include "iexception.h"
#include "imutexlock.h"
#include "assert.h"

//A simple counter class provided the thread safe reference counting.
//Three basic functions are IncReference(), DecReference() and GetReference().
//It can be inherited or contained by other classes in mutithread programming.
class IRefCounter
{
public:
	// Default constructor.
	IRefCounter();	

	// Destructor.
	virtual ~IRefCounter();

	// The value stored the reference count.
	int		m_nRefCount;

	// Increment the reference count, Mutithread safe.	
	// @Return:
	//		The reference count after increment.
	int		IncReference();

	// Decrement the reference count, mutithread safe.
	// @Return:
	//		The reference count after decrement.
	int		DecReference();

	// Add the reference count
	// @Return:
	//		The reference count after setting.
	int		AddReference( int count );

	// Subtract the reference count for convenience when upgrading it to the database, mutithread safe.
	// @Return:
	//		The reference count after setting.
	int		SubReference( int count );
	
    // Return the reference count.
	// @Return:
	//		The current reference count.
	int		GetReference() 
	{
		 return m_nRefCount; 
	};
		
protected:
	// The lock for mutithread used, it will be lock at the time invoking 
	// IncReference() or DecReference().
	ILock	m_RefLock;

private:
	// Prevent IRefCounter from implicit conversion.
	IRefCounter (const class IRefCounter& ref);

	// Prevent IRefCounter from equality operation.
	IRefCounter& operator = ( const class IRefCounter& ref );
};

#endif //_REFCOUNTER_H
