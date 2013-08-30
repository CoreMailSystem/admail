/*
	<a href="mailto:wooce@21cn.com">Wooce Yang</a>,SCUT(South China University of Technology)
*/
#ifndef	_BASIC_FLIGHT_H_
#define _BASIC_FLIGHT_H_

#include "istring.h"
#include "incoredb.h"
#include "tattributeset.h"
#include "parsedtemplate.h"

class BasicFlight
{
public:
	BasicFlight();

	virtual ~BasicFlight();

	static void* ThreadProxy(void* arg);

    virtual void Execute(void);

	virtual void Init(IIncoreDBPool *pDBPool, bool bAutoRefresh, int nRefreshInterval = 10);

	virtual void OnInit() = 0;

	//Reload the whole cache which the orders store in in this channel
	virtual void Refresh()=0;

	virtual void Refresh(int orderNo)=0;

	//Flush the change in the cache into the database if necessary
	virtual void FlushToDB()=0;

	virtual IString GetStatus()=0;

	virtual int Targeting(const TAttributeSet &attrs, int nMaxNo, int nMaxAllowed, 
		TAttributeSet &midSet)=0;

protected:
	IIncoreDBPool *m_pDBPool;

	pthread_t m_TID;
	
	bool m_bAutoRefresh;

    bool m_bRunRefresh;

	int m_nRefreshInterval;
};


#endif
