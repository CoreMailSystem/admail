//written by Wooce Yang,from SCUT(South China University of Technology)
#include "adservsvr.h"
#include "tprotocol.h"
#include "attrconvert.h"

extern IError g_Log;

int g_ChannelID[CHANNEL_COUNT] = 
{
	ADSERV_CHNID_SYSTEM,
	ADSERV_CHNID_ADVER
};

ADServSvr::ADServSvr(IIniFile ADIniFile, const IString &ADCtrlSection)
{	
    IString lpConnect;
    lpConnect = "UID=";
    lpConnect += ADIniFile.GetDefStrValue( ADCtrlSection, "DBUID", "AD", "root" );
    lpConnect += ";pwd=";
    lpConnect += ADIniFile.GetDefStrValue( ADCtrlSection, "DBPwd", "AD", "" );
    IDEBUG( g_Error.Out(L_DEBUG, "pwd=%s",ADIniFile.GetDefStrValue( ADCtrlSection, "DBPwd", "AD", "" ).CConstStr()) );
    const IString& dbDriver = ADIniFile.GetDefStrValue( ADCtrlSection, "DBDriver", "AD", "oracle" );
    if( dbDriver.ICompare("oracle")!=0 )
	{
        lpConnect += ";database=";
        lpConnect += ADIniFile.GetDefStrValue( ADCtrlSection, "DBName", "AD", "AdMail" );
	}
    lpConnect += ";server=";
    lpConnect += ADIniFile.GetDefStrValue( ADCtrlSection, "DBServer", "AD", "localhost" );
    lpConnect += ";driver=";
    lpConnect += ADIniFile.GetDefStrValue( ADCtrlSection, "DBDriver", "AD", "mysql" );

    int poolSize = ADIniFile.GetDefIntValue( ADCtrlSection, "PoolSize", "AD", 30 );

	try
    {
        IDEBUG( g_Error.Out(L_DEBUG, "begin to init pool of %s",lpConnect.CConstStr()) );
        if (DBPool.Init(lpConnect,poolSize)!=NORMAL)
            throw(ISTDDBException(ERRORS,"Err to create connect!"));
    } catch (IException &e)
    {
        g_Error.Out(L_ALARM,"Init database pool connection Err:%d %s\n",e.GetErrorCode(),e.GetErrorInfo());
    }
	try
	{
		int nTimeout = ADIniFile.GetDefIntValue(ADCtrlSection, "Timeout", "AD", 30);
		m_pFlight[0] = new SystemFlight();
		m_pFlight[0]->Init(&DBPool, true, nTimeout);
		m_pFlight[1] = new ADFlight();
		m_pFlight[1]->Init(&DBPool, true, nTimeout);
		IDEBUG( g_Error.Out(L_DEBUG, "ADServSvr.ADServSvr: construct success!\n" ) );
	}
	catch (IException &e)
	{
		g_Error.Out(L_ALARM, e.GetErrorInfo());
	}
}

// Destructor.
ADServSvr::~ADServSvr()
{
	for( int i=0;i<CHANNEL_COUNT;i++ )
	  delete m_pFlight[i];
}

int ADServSvr::OnRequest( ISocket& Socket )
{
    if (Socket.GetSocket() == -1)
    {
        g_Error.Out(L_ALARM,"socket in SockArg is -1\n");
        return ERRORINNET;
    } else
    {
        g_Error.Out(L_INFO,"socket %d get request!\n",Socket.GetSocket());
    }

    IAComm comm(Socket);
    TProtocol *pProtocol;
    TAttributeSet asRequest, asResponse;
    XNew(pProtocol, TProtocol(0x0101,0x0001));
    IAuto_Ptr<TProtocol> autoProtocol(pProtocol);
    try
    {
		IDEBUG( g_Error.Out(L_DEBUG, "ADServSvr.OnRequest: Get request from client!!\n") );
        pProtocol->SvrReceive(&comm, asRequest);
		IDEBUG( g_Error.Out(L_DEBUG, "ADServSvr.OnRequest: Got request from client successfully!!\n") );
        ServProc(asRequest,asResponse);
		IDEBUG( g_Error.Out(L_DEBUG, "ADServSvr.OnRequest: Send response to client!!\n") );
        pProtocol->SvrSend(&comm, asResponse);
		IDEBUG( g_Error.Out(L_DEBUG, "ADServSvr.OnRequest: Sent response to client successfully!!\n") );
        return NORMAL;
    } catch (IException &e)
    {
        g_Error.Out(L_WARNING, e.GetErrorInfo());
        return ERRORINNET;
    }
	catch (...)
	{
		g_Error.Out(L_WARNING, "Unknown error\n");
		return ERRORINNET;
	}
}

int ADServSvr::ServProc( TAttributeSet &asRequest, TAttributeSet &asResponse )
{
	const TAttributeSet &asUserAttrs = asRequest.Get("UserAttrs").AsAttributeSet();
    try
    {
		asResponse.Clear();
		int nRes=0;
        asResponse.Put( "Result", nRes );
		// Convert the coremail attrubutes to the standard attributes in advertise system
		TAttributeSet userAttrs;
		g_AttrSetConvert.Convert(asUserAttrs,userAttrs);
		int i;
		IDEBUG(
			for( i=0;i<userAttrs.GetSize();i++ )
			{
				g_Error.Out(L_DEBUG, "%s ",userAttrs.GetAttribute(i).GetID().CConstStr() );
				if (userAttrs.GetAttribute(i).Get().GetType() == VAR_TYPE_INT)
					g_Error.Out(L_DEBUG, " = %d\n", userAttrs.GetAttribute(i).Get().AsInt());
				if (userAttrs.GetAttribute(i).Get().GetType() == VAR_TYPE_STR)
					g_Error.Out(L_DEBUG, " = %s\n", userAttrs.GetAttribute(i).Get().AsString().CConstStr());
			}
		);
        TAttributeSet channelsResponse;
		for( i=0;i<asRequest.GetAttribute("Channels").GetSize();i++ )
		{
            const TAttributeSet& asChannel = asRequest.GetAttribute("Channels").Get(i).AsAttributeSet();
			int nChannelID = asChannel.Get("ChannelID").AsInt();
			int j;
			for( j=0;j<CHANNEL_COUNT;j++ )
				if( g_ChannelID[j]==nChannelID )
					break;
		    if( j==CHANNEL_COUNT )
			{
				asResponse.Clear();
				asResponse.Put("Result",1);
				asResponse.Put("ErrNo",1);
				asResponse.Put("ErrMsg","Channel not exist");
                return -1;
			}
			int nMaxNo = asChannel.Get("MaxNo").AsInt();
		    TAttributeSet midSet;
            int nNewMaxNo;
            nNewMaxNo = m_pFlight[nChannelID]->Targeting(userAttrs,
										 nMaxNo,
										MaxAllowed(userAttrs,nChannelID),
										midSet);
            TAttributeSet channelResponse;
			channelResponse.Put("ChannelID",nChannelID);
			channelResponse.Put("MaxNo",nNewMaxNo);
			if( midSet.GetSize()>0 )
			{
				int z;
				for (z=0; z<midSet.GetAttribute("MailIDSet").GetSize(); z++)
					channelResponse.Put("MailIDSet",midSet.GetAttribute("MailIDSet").Get(z).AsAttributeSet());
			}
			asResponse.Put("Channels",channelResponse);
		}
        if ( !asResponse.HasAttribute("Channels") )
            throw IException(1, "No applicable Advertise Channel for this user.\n");
		//IDEBUG( g_Error.Out(L_DEBUG, "ADServSvr.ServProc: Targeting returned, midlist count: %d\n", channelsResponse.GetSize()) );		
		//asResponse.Put( "Channels", channelsResponse );
        IString LogMsg = "";
        /*LogMsg.Format( "userid:%s domainid:%s maxno:%d logintype: %d    Impression-->mailcount:%d \n",
			asRequest.Get("UserID").Get().AsString().CStr(),
			asRequest.Get("DomainName").Get().AsString().CStr(),			
			asRequest.Get("LoginType").Get().AsInt(),
			midList.GetCount(),
			nNewMaxNo );  */
//        g_Log.Out( L_FATAL, LogMsg );
		return 0;
    } 
	catch ( IException &e )
    {
        asResponse.Clear();
        asResponse.Put("Result", 1);
        asResponse.Put("ErrNo", 2);
		IString errMsg = "Server Exception: ";
		errMsg += e.GetErrorInfo();
        asResponse.Put("ErrMsg", errMsg);
        g_Error.Out( L_WARNING,errMsg );
        /*IString LogMsg = "";
        LogMsg.Format( "userid:%s domainid:%s logintype: %d Error:%d %s\n",
                       asRequest.Get("UserID").Get().AsString().CConstStr(),
                       asRequest.Get("DomainName").Get().AsString().CConstStr(),                       
                       asRequest.Get("LoginType").Get().AsInt(),
                       e.GetErrorCode(),
                       e.GetErrorInfo() ); */
//        g_Log.Out( L_FATAL,LogMsg );
		return -1;
    }
	catch ( ... )
	{
		g_Error.Out(L_WARNING, "ServPro: Unknown error.\n");
		return -1;
	}
}

IString ADServSvr::GetStatus()
{
	IString statusStr = "";
	for( int i=0;i<CHANNEL_COUNT;i++ )
	{
		statusStr += m_pFlight[i]->GetStatus();
	}
	return statusStr;
}
