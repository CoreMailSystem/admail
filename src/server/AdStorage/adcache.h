/**
 * Title:adcache.h
 * Description:   Inherited from IHashCache, each node in the cache will store a letter( include all of the possible channels)
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author:Wooce Yang,from SCUT(South China University of Technology)
 * @version 1.0
 */

#ifndef _ADCACHE_H
#define _ADCACHE_H

#include "ifile.h"
#include "ihashcache.h"
#include "iinifile.h"
#include "icountingpointer.h"
#include "incoredb.h"
#include "parsedtemplate.h"
#include "cacheadtemplate.h"


// The cache class for Ad Storage Server.
// @Description:
// 'ADCache' is a class derived from 'IHashCache''s instantiated class 
// 'IHashCache<ICountingPointer<ADHtml>, IKey>  (IKey should be overloaded
//  the relation equality operator). Some additional functions
//	for AD Storage is added. 
// @MT-Safe
//		Yes
// @SeeAlso:
//		'IHashCache', 'IKey', 'CacheAdTemplate', 'ParsedTemplate'
class ADCache:public IHashCache<ICountingPointer<CacheAdTemplate>, IString>
{
public:
	// Default constructor.
	ADCache();

	// Constructor with config file.
	// @Parameters:
	//		%IniFile		The ini file which has the ad cache parameters section.
	//		%ADCtrlSection	The section name of ad cache parameters.
	ADCache( IIniFile IniFile, const IString &ADCtrlSection,const IString &CacheSection );

	// Constructor with parameters specified.
	// @Parameters:
	//		%nMaxMem			The max memory (bytes) the cache can use.
	//		%nMaxCacheBlock		The number of blocks in the hash table.
	//		%nMaxCacheSize		The max number of nodes can be stored.
	//		%nNumToReap			The number of node being deleted every time 
	//							invoking '.Reap()'
	//		%nAjustRate			The time of hiting that will change LRU list.
	ADCache(long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate );

	// Destructor.
	~ADCache();

	// call back function for pthread_create
    static void* ThreadProxy(void* arg);

    // executed in the call back function ThreadProxy(), can be overloaded in subclass
    virtual void Execute(void);
    
	// The hash function that used for ad cache. Overiding from 'IHashCache'.
	// @Parameters:
	//		%Key		The key offers to the hash function to compute.
	// @Return:
	//		A number represents the block number in the hash table.
    unsigned Hash( const IString &Key );
    
    // Destory the old internal data and set the new parameters.
	// @parameters:
	//		%nMaxMem			The max memory (bytes) the cache can use.
	//		%nMaxCacheBlock		The number of blocks in the hash table.
	//		%nMaxCacheSize		The max number of nodes can be stored.
	//		%nNumToReap			The number of node being deleted every time 
	//							invoking '.Reap()'
	//		%nAjustRate			The time of hiting that will change LRU list.
	void Set( long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate );

	
    // Load the parameters from ini file.
	// @Parameters:
	//		%IniFile		The ini file which has the ad cache parameters section.
	//		%strSection		The section name of ad cache parameters.
	void Config( IIniFile IniFile, const IString &ADCtrlSection, const IString &strSection );

	
	// Traverse the hash table.
	void OnTraverseContainer( IHashCacheNode<ICountingPointer<CacheAdTemplate>, IString> *pNode );

	// Flush the reference count of each node which the letters stored in into the database
    void FlushToDB();

	// Get the running status string
    IString GetStatus();

    // the main interface function of ADCache
	//    been called in ADStorageSvr::ServProc
	// @parameters:
	//		%mid			the mid of the letter that the mail system want to fetch
	//		%loginType		the user login type, see 'enum ADLoginType' in "adconst.h"
	ICountingPointer<CacheAdTemplate> Get( const IString mid,int loginType,int &flag );

    void WriteToFSCache( const IString& mid,const IString& mailContent,bool OnlyWhenExisted=false );

    // provide the db connection to the refresh thread
    void SetThreadDBConn( ISTDDBConnection* connection ){ DBConn=connection; }

	// Get the storage path of the mail when the mail is extracted to the file system
    IString GetMailPath( const IString& mid );
protected:
    //  Overload the function defined in class IHashCache
	virtual void GetCacheObject( const IString &Key,ICountingPointer<CacheAdTemplate>& CountingTemplate );

	virtual void OnRefreshNode( const IString &Key,ICountingPointer<CacheAdTemplate>& CountingTemplate );

private:
    IIncoreDBPool DBPool;
    ISTDDBConnection *DBConn;
	bool m_bAutoRefresh;
	int  m_nRefreshInterval;
    pthread_t m_TID;
    //   The base path which the mail extracted into,  a level of hash subdirectory will be created under this base path
	IString m_strMailPath;
    //   max count of the hash subdirectories
	int     m_nMailStorBlock;
};
#endif
