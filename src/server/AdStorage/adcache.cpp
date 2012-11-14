//written by Wooce Yang,from SCUT(South China University of Technology)
#include <typeinfo>
#include "adcache.h"
#include "utils.h"
#include "adconst.h"


ADCache::ADCache( IIniFile IniFile, const IString &ADCtrlSection,const IString &CacheSection ):IHashCache<ICountingPointer<CacheAdTemplate>, IString>()
{
	Config( IniFile, ADCtrlSection, CacheSection );
}

ADCache::~ADCache()
{
	if( m_bAutoRefresh )
		pthread_cancel(m_TID);
	FlushToDB();
};

void* ADCache::ThreadProxy(void* arg)
{
	ADCache* p = (ADCache *)arg;
	p->Execute();
	return NULL; 
}

void ADCache::Execute(void)
{
	g_Error.Out(L_INFO, "Refresh thread for cache template was started, interval=%ds\n", m_nRefreshInterval);
	while(true)
	{
		Delay(m_nRefreshInterval,0);
		g_Error.Out(L_INFO, "Refresh cache template...\n", m_nRefreshInterval);
		int i = DBPool.Apply();
		try
		{
			ISTDDBConnection *connect = DBPool.GetConnection(i);
			SetThreadDBConn(connect);
			TraverseContainer();
		} catch( ISTDDBException &e )
		{
			DBPool.Release( i,true );
			continue;
		} catch(...)
		{
		}
		DBPool.Release( i );
		g_Error.Out(L_INFO, "Refresh complete!!!\n", m_nRefreshInterval);
	}
};

void ADCache::Set( long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate )
{
	IHashCache<ICountingPointer<CacheAdTemplate>, IString>::Set( nMaxMem, nMaxCacheBlock, nMaxCacheSize, nNumToReap, nAdjustRate );
};

void ADCache::Config( IIniFile IniFile, const IString &ADCtrlSection, const IString &strSection )
{
	IString lpConnect;
	lpConnect = "UID=";
	lpConnect += IniFile.GetDefStrValue( ADCtrlSection, "DBUID", "AD", "root" );
	lpConnect += ";pwd=";
	lpConnect += IniFile.GetDefStrValue( ADCtrlSection, "DBPwd", "AD", "" );
	const IString& dbDriver = IniFile.GetDefStrValue( ADCtrlSection, "DBDriver", "AD", "oracle" );
	if( dbDriver.ICompare("oracle")!=0 )
	{
		lpConnect += ";database=";
		lpConnect += IniFile.GetDefStrValue( ADCtrlSection, "DBName", "AD", "AdMail" );
	}
	lpConnect += ";server=";
	lpConnect += IniFile.GetDefStrValue( ADCtrlSection, "DBServer", "AD", "localhost" );
	lpConnect += ";driver=";
	lpConnect += IniFile.GetDefStrValue( ADCtrlSection, "DBDriver", "AD", "mysql" );

	int poolSize = IniFile.GetDefIntValue( ADCtrlSection, "PoolSize", "AD", 30 );

	try
	{
		if(DBPool.Init(lpConnect,poolSize)!=NORMAL)
			throw(ISTDDBException(ERRORS,"Err to create connect!"));
	} catch(IException &e)
	{
		g_Error.Out(L_ALARM,"Err:%d %s\n",e.GetErrorCode(),e.GetErrorInfo());
	}

	int nRefreshInterval = IniFile.GetDefIntValue(ADCtrlSection, "Timeout", "AD", 30);
	bool bAutoRefresh = true;
	m_bAutoRefresh = bAutoRefresh;
	if(m_bAutoRefresh)
	{
		m_nRefreshInterval = nRefreshInterval;
		pthread_create(&m_TID,NULL, ThreadProxy, this);
	}

	long MaxCacheMemSize = IniFile.GetDefIntValue( strSection, "MaxCacheMemSize", "Cache", 500000) * 100;
	if( MaxCacheMemSize <= 0 )
		g_Error.Out( L_FATAL, "Cache: Cache size should be greater than zero." );

	int MaxCacheBlock = IniFile.GetDefIntValue( strSection, "MaxCacheBlock", "Cache", 100);
	if( MaxCacheBlock <= 0 )
		g_Error.Out( L_FATAL, "Cache: The block number of cache should be greater than zero." );

	int MaxCacheSize = IniFile.GetDefIntValue( strSection, "MaxCacheSize", "Cache", 100);
	if( MaxCacheSize <= 0 )
		g_Error.Out( L_FATAL, "Cache: The size of the cache should be greater than zero." );

	int NumToReap = IniFile.GetDefIntValue( strSection, "NumToReap", "Cache", 1);
	if( NumToReap <= 0 )
		g_Error.Out( L_FATAL, "Cache: The reap number of the cache should be greater than zero." );

	int AdjustRate = IniFile.GetDefIntValue( strSection, "AdjustRate", "Cache", 1);
	if( AdjustRate <= 0 )
		g_Error.Out( L_FATAL, "Cache: The adjust rate of the cache should be greater than zero." );

	Set( MaxCacheMemSize, MaxCacheBlock, MaxCacheSize, NumToReap, AdjustRate ); 
	m_strMailPath    = IniFile.GetDefStrValue(ADCtrlSection, "MailStoragePath", "AD", "/home/ad/mail/");
	m_nMailStorBlock = IniFile.GetDefIntValue(ADCtrlSection, "MailStorageBlockCount", "AD", 50);
	if( m_strMailPath.GetLen()>=18 )
		throw IException(-1,"the mail storage directory path name too long!");
	struct stat buf;
	IString tempPath = m_strMailPath;
	if(stat(m_strMailPath,&buf)==-1)
	{
		if(MakeDir(m_strMailPath)!=0)
			throw(IException(ERRORS,"Can't create the path : %s",m_strMailPath.CConstStr()));
	} else
	{
		tempPath += "/adstorage.test";
		IFile test;
		if(test.Open(tempPath.CConstStr(),O_RDWR | O_CREAT | O_TRUNC) <0)
			throw(IException(ERRORS,"Can't Access the path : %s",m_strMailPath.CConstStr()));
		remove(tempPath.CConstStr());
		test.Close();
	}
};

//Print the key name when the cache traverse all the node 
void ADCache::OnTraverseContainer( IHashCacheNode<ICountingPointer<CacheAdTemplate>, IString> *pNode )
{
	if( pNode->GetObject()->GetChannelID()==ADSERV_CHNID_ADVER )
		pNode->GetObject()->GetAdChannelOP()->DumpToDB( DBConn );
};

//Hash for ad
unsigned ADCache::Hash (const IString &Key)
{
	int h=0;
	int len = Key.GetLen();

	for( int i=0; i<len; i++ )
	{
		h *= 16777619;
		h ^= (unsigned char )Key[i];
	}
	return h;
};

void ADCache::FlushToDB()
{
	int i = DBPool.Apply();
	try
	{
		ISTDDBConnection *connect = DBPool.GetConnection(i);
		SetThreadDBConn(connect);
		TraverseContainer();
	} catch( ISTDDBException &e )
	{
		DBPool.Release( i,true );
		return;
	} catch(...)
	{
	}
	DBPool.Release( i );
}

IString ADCache::GetStatus()
{
	IString statusStr = "";
	statusStr.Format("%d letters stored in the cache.The cache can store %d letters at most",m_nCacheCount,m_nCacheSize);
	return statusStr;
}

ICountingPointer<CacheAdTemplate> ADCache::Get( const IString mid,int loginType,int &flag )
{
	ICountingPointer<CacheAdTemplate> ad = FetchAndCache(mid,flag);
	IDEBUG( g_Error.Out(L_DEBUG, "ADCache.Get: FetchAndCache success!!\n") );
	if( ad->GetChannelID()==ADSERV_CHNID_ADVER )
		ad->GetAdChannelOP()->IncReference( loginType );
	return ad;
}

void ADCache::GetCacheObject( const IString &Key,ICountingPointer<CacheAdTemplate>& CountingTemplate)
{
	//ICountingPointer<CacheAdTemplate> CountingTemplate;
	IString sql;
	IMySQLConnection connection;
	IString strMID = connection.EncodeString(Key);
	sql.Format( "select mail_type,mail_body from %s where mid='%s'",AM_TABLE_ADLETTER, strMID.CConstStr() );
	int i = DBPool.Apply();
	CacheAdTemplate* ad = NULL;
	//int adcount = -1;
	try
	{
		//  Should not use IDBConnection here,  for we shouldn't delete the connection pointer in the pool in this scope myself
		ISTDDBConnection* connect = DBPool.GetConnection(i);
		IDBStatement statement(connect->GetStatement(sql));
		statement->ExecCommit();
		IDEBUG( g_Error.Out(L_DEBUG, "ADCache.GetCacheObject: exec success!!\n") );

		IDBResultSet set(statement->GetResultSet());
		IDEBUG( g_Error.Out(L_DEBUG, "ADCache.GetCacheObject: begin to fetch!!\n") );
		//   We can't use GetRowCount() to get the result count of the select statement on Oracle, so we just try Fetch here and don't care about the query result count
		if( !(set.Fetch()) )
			throw IException(ERRORS,"Can't find the letter of the mid!");
		IDEBUG( g_Error.Out(L_DEBUG, "ADCache.GetCacheObject: fetch success!!\n") );
		//New the specified mail according to the channel
		int nChnID = strMID.SubString(3, 3).ToInt();        
		if(nChnID == ADSERV_CHNID_SYSTEM)
		{
			//System mail.
			XNew( ad, CacheAdTemplate(set["mail_body"].AsString()) );
		} else
			if(nChnID == ADSERV_CHNID_ADVER )
		{
			//Ad mail.
			XNew( ad, CacheAdTemplate(set["mail_body"].AsString(), &DBPool, strMID ));
		}
	} catch( ISTDDBException e )
	{
		IDEBUG( g_Error.Out(L_DEBUG, "ADCache.GetCacheObject: ISTDDBException caught here:%s!!\n",e.GetErrorInfo()) );
		DBPool.Release( i,true );
		throw;
	} catch( IException e )
	{
		IDEBUG( g_Error.Out(L_DEBUG, "ADCache.GetCacheObject: Exception caught here!!\n") );
		DBPool.Release( i );
		throw;
	} catch( ... )
	{
		IDEBUG( g_Error.Out(L_DEBUG, "ADCache.GetCacheObject: Unknown Error!!\n") );
	}
	DBPool.Release( i );
	if( ad==NULL )
	{
		IDEBUG( g_Error.Out(L_DEBUG, "ADCache.GetCacheObject: ad is null!!\n") );
		throw ( IException(2, "Data storage error.\n") );
	}
	CountingTemplate = ad;
}

IString ADCache::GetMailPath( const IString& mid )
{
	IString path('\0',40);
	path = m_strMailPath;
	int h=0;
	int len = mid.GetLen();
	for( int i=0; i<len; i++ )
	{
		h *= 16777619;
		h ^= (unsigned char )mid[i];
	}
	path += IntToStr((unsigned)h%m_nMailStorBlock);
	return path;
}

// will throw IFileException when write to disk error 
void ADCache::WriteToFSCache( const IString& mid,const IString& mailContent, bool OnlyWhenExisted )
{
	IDEBUG( g_Error.Out(L_DEBUG, "Start Write to File System Cache process!\n"); );
	IString path = GetMailPath(mid);
	IFile letter;
	IString letterPath = path;
	letterPath += "/";
	letterPath += mid;      
	struct stat buf;
	IDEBUG( g_Error.Out(L_DEBUG, "Get the mail path : %s\n", letterPath.CStr()); );
	if( OnlyWhenExisted && stat(letterPath,&buf)==-1 )   //只在disk上已有的时侯才写,   for cache refresh/reload process
		return;
	if( !OnlyWhenExisted && stat(letterPath,&buf)!=-1 )  //只有在disk上没有的时侯才写,  for  Info命令
		return;
	if(stat(path,&buf)==-1)
	{
		if(MakeDir(path)!=0)
			throw(IFileException(ERRORS,"Can't create the hash path : %s",path.CConstStr()));
		IDEBUG( g_Error.Out(L_DEBUG, "Created the dir : %s\n", path.CStr()); );
	}
	int tryCount = 3;
	//if ( !(stat(letterPath,&buf)!=-1) )
	//{
	int res=letter.OpenAndLock(F_TRYWLOCK,letterPath.CConstStr(),O_RDWR|O_CREAT|O_TRUNC,tryCount);
	if(res>0)
	{
		letter.WriteEx( mailContent.CConstStr(),mailContent.GetLen() );
		IDEBUG( g_Error.Out(L_DEBUG, "cache the mail to file system!\n"); );
	} else
		throw(IFileException(ERRORS,"Can't open the file %s when dealing INFO command",letterPath.CConstStr()));
	letter.Close();
	//}
}

void ADCache::OnRefreshNode( const IString &Key,ICountingPointer<CacheAdTemplate>& CountingTemplate )
{
	try
	{
		WriteToFSCache( Key,CountingTemplate->Get(), true );
	} catch( IException &e )
	{
		g_Error.Out(L_ALARM,"ADCache.OnRefreshNode:can't write to disk!");
	}

}