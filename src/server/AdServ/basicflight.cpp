/*
	<a href="mailto:wooce@21cn.com">Wooce Yang</a>,SCUT(South China University of Technology)
*/
#include "basicflight.h"
#include "utils.h"

BasicFlight::BasicFlight()
{	
};

void BasicFlight::Init(IIncoreDBPool *pDBPool, bool bAutoRefresh, int nRefreshInterval)
{
	if (pDBPool == NULL)
		throw IException(-1, "BasicFlight.BasicFlight: DB Pool can not be null.");
	m_pDBPool = pDBPool;
	m_bAutoRefresh = bAutoRefresh;
	m_bRunRefresh  = bAutoRefresh;
	OnInit();
	if (m_bAutoRefresh)
	{
		m_nRefreshInterval = nRefreshInterval;
		IDEBUG( g_Error.Out(L_DEBUG, "BasicFlight.Init: Try to create thread\n") );
 		pthread_create(&m_TID,NULL, ThreadProxy, this);
		IDEBUG( g_Error.Out(L_DEBUG, "BasicFlight.Init: create thread success!\n") );
		//	pthread_join(tid, NULL);
	}
};

void* BasicFlight::ThreadProxy(void* arg)
{
    IDEBUG( g_Error.Out(L_DEBUG, "BasicFlight.ThreadProxy: Try to execute\n") );
    BasicFlight* p = (BasicFlight *)arg;
    p->Execute();
    return NULL; 
}

void BasicFlight::Execute(void)
{
	g_Error.Out(L_INFO, "Refresh thread for flight was started, interval=%ds\n", m_nRefreshInterval);
	while (true)
	{
        Delay(m_nRefreshInterval,0);
		g_Error.Out(L_INFO, "Refresh thread active...\n");
//		RefreshArg->m_pFlight->UpdateImpression();	//should be change later
		try
		{
			if( m_bRunRefresh )
				Refresh();
		}
		catch (IException &e)
		{
			g_Error.Out(L_WARNING, e.GetErrorInfo());
		}
		g_Error.Out(L_INFO, "Refresh complete!!!\n");
	}
};

BasicFlight::~BasicFlight()
{
	if (m_bAutoRefresh)
		pthread_cancel(m_TID);
};
