/**
 * Title:adservsvr.h
 * Description:  
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author:<a href="mailto:wooce@21cn.com">Wooce</a> ,SCUT(South China University of Technology)
 * @version 1.0
 */
#ifndef _ADSERVSVR_H
#define _ADSERVSVR_H

#include "iappsvr.h"
#include "incoredb.h"
#include "tattributeset.h"
#include "adtarget.h"
#include <list>
#include "adflight.h"
#include "systemflight.h"

#define CHANNEL_COUNT 2

class ADServSvr:public IAppSvr
{
public:
	ADServSvr(IIniFile ADIniFile, const IString &ADCtrlSection);

    // Destructor.
	~ADServSvr();
	
    // Callback function for handling the flow of ad.
	// @Return:
	//		NORMAL			The flow of ad has no error.
	//		ERRORINET		Error occurs during getting request, sending request 
	//						or getting the page.
	int OnRequest( ISocket& Comm );
    
    int ServProc( TAttributeSet &CmdBuf, TAttributeSet &ResBuf );

    //  Return the max limit of the letter count that is sent to the user who logins this time
    // @param
    //       kvList  
	// @Return:
	//		    
    virtual int MaxAllowed( const TAttributeSet& kvList,int m_nChannelID ){ return -1; }

	void ReloadCache( int m_nChannelID ){ m_pFlight[m_nChannelID]->Refresh(); }

	void ReloadCache( int m_nChannelID,int orderNo ){ m_pFlight[m_nChannelID]->Refresh(orderNo); }

	void FlushCache( int m_nChannelID ){ m_pFlight[m_nChannelID]->FlushToDB(); }

	IString GetStatus();

protected:

private:
    IIncoreDBPool DBPool;

	BasicFlight* m_pFlight[CHANNEL_COUNT];
};

#endif

