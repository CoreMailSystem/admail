/**
 * Title:adstoragesvr.h
 * Description:  
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author:<a href="mailto:wooce@21cn.com">Wooce Yang</a>,from 
 *        SCUT(South China University of Technology)
 * @version 1.0
 */

#ifndef _ADSTORAGESVR_H
#define _ADSTORAGESVR_H

#include "iappsvr.h"
#include "tmboxprotocol.h"
#include "adcache.h"

class ADStorageSvr:public IAppSvr
{
public:
	ADStorageSvr(IIniFile ADIniFile, const IString &ADCtrlSection, const IString &CacheSection);

    // Destructor.
	~ADStorageSvr();
	
    // Callback function for handling the flow of ad.
	// @Return:
	//		NORMAL			The flow of ad has no error.
	//		ERRORINET		Error occurs during getting request, sending request 
	//						or getting the page.
	int OnRequest( ISocket& Comm );

	// The main procedure dealing with the request by the mail system
	void ServProc( TAttributeSet& asRequest,TAttributeSet& asResponse );

	// ---------------------    The old compatible protocol           ----------------
	void DelProc( TAttributeSet& asRequest,TAttributeSet& asResponse );

	void GetProc( TAttributeSet& asRequest,TAttributeSet& asResponse );

	void PutProc( TAttributeSet& asRequest,TAttributeSet& asResponse );

	void SearchProc( TAttributeSet& asRequest,TAttributeSet& asResponse );

	void InfoProc( TAttributeSet& asRequest,TAttributeSet& asResponse );

	void CheckProc( TAttributeSet& asRequest,TAttributeSet& asResponse );
	// ---------------------    End of The old compatible protocol    ----------------

	// Reload one specific letter stored in the cache
	void ReloadCache( const IString& mid ){ p_Cache->RefreshNode(mid); }

	// Reload all the letters stored in the cache
	void ReloadCache(){ p_Cache->RefreshNodes(); }

	// Flush reference counts of all the letters in the cache into the database
	void FlushCache(){ p_Cache->FlushToDB(); }

	// Get the running status string of this AD Storage Server
	IString GetStatus();

private:
    ADCache* p_Cache;
};

#endif
