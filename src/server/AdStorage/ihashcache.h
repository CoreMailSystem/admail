/*
    Author	: Kaman Chan
    CreationDate: 2000-09-14
    Upgraded by Wooce Yang, 2001/7/17
*/

#ifndef _IHASHCACHE_H
#define _IHASHCACHE_H

#include "icache.h"
#include "imutexlock.h"
#include "ierror.h"
#include "iexception.h"

template <class IObject, class IKey>
class IHashCache;

// A class derived from 'ICacheNode', it cooperates with 'IHashCache'. 
// @Description:
//		'IHashCacheNode'是'IHashCache'的结点类型，继承了'ICacheNode'与'IHashCache'配对。
//	可以储存结点有效期和Hash,LRU链等信息。
// @MT-Safe:
//		Yes / Required.
// @Notes:
//		Equality operator and relation equality operator should be overloaded.
// @SeeAlso:
//		'IHashCache', 'ICacheNode'
template <
class IObject, 
//The key type of this cache.
class IKey
>
class IHashCacheNode : public ICacheNode<IObject, IKey>
{
public:
    // Constructor.
    // @Parameters:
    //		%Object			The Object to be stored.
    //		%Key			The key assoicated the object.
    //		%nSize			The size of the object.
    //		%nTimeToLive	The time the object lives in the cache. -1 is permanence.
    IHashCacheNode (IObject Object, const IKey &Key, long nSize = 0, time_t nTimeToLive = -1);

    // virtual destructor.
    virtual  ~IHashCacheNode();

    // Make a new copy of the object in this cachenode and export it.
    virtual IObject GetObject();

    // Return the key in this cachenode .
    const IKey &GetKey();

private:
    // Do the initial work for IHashCache.
    void Init (const IKey &Key);    

    // Prevent the cachenode from implicit conversion.
    IHashCacheNode(const IHashCacheNode< IObject,IKey > &right) {};

    // Prevent the cachenode from doing equality operation.
    IHashCacheNode< IObject,IKey > & operator=(const IHashCacheNode< IObject,IKey > &right) {};

protected:     
    // The next cachenode in hash table.
    IHashCacheNode *m_pHashNextNode;

    // The previous cachenode in hash table.
    IHashCacheNode *m_pHashPrevNode;

    // The next cachenode in LRU list(access list).
    IHashCacheNode *m_pAccessListNextNode;

    // The previous cachenode in LRU list(access list).
    IHashCacheNode *m_pAccessListPrevNode;

    // The accessed time of this cachenode.
    // @Description:
    //		Increment whenever this cachenode was exported.
    //	When this value equals the adjust rate which user defined, the LRU 
    //	list will be adjusted, and this value will be set to zero.
    int m_nHits;

    // The context of the object which is stored.
    IObject m_Object;

    // The key associated with this cachenode.
    IKey m_Key;

    friend class IHashCache<IObject, IKey>;
};

template <class IObject, class IKey>
IHashCacheNode<IObject,IKey>::IHashCacheNode (IObject Object, const IKey &Key, long nSize, time_t nTimeToLive):ICacheNode<IObject, IKey>(nSize,nTimeToLive)
{
    m_Object = Object;
    Init( Key );
}


template <class IObject, class IKey>
IHashCacheNode<IObject,IKey>::~IHashCacheNode()
{
}

template <class IObject, class IKey>
void IHashCacheNode<IObject,IKey>::Init (const IKey &Key)
{
    m_Key = Key;

    m_pHashNextNode = NULL;
    m_pHashPrevNode = NULL;
    m_pAccessListNextNode = NULL;
    m_pAccessListPrevNode = NULL;

    m_nHits = 0;
}

template <class IObject, class IKey>
const IKey &IHashCacheNode<IObject, IKey>::GetKey()
{
    return m_Key;
}

template <class IObject, class IKey>
IObject IHashCacheNode<IObject, IKey>::GetObject()
{
    return m_Object;
}


#define IHashCacheNodeType IHashCacheNode<IObject, IKey>

// 'IHashCache'类继承了'ICache'类，并实现了相应的存取和筛选算法。
// @Description:
//		对于Cache的存取，'IHashCache'采用了散列算法，根据用户所提供的散列函数
//	对新储存的结点进行散列。至于筛选算法，则采用最近最少使用LRU算法，存
//	入CACHE的结点组成一条LRU链，用者可以指定改变LRU结点顺序的结点调用次
//	数。每当CACHE装满或结点过期时，'IHashCache'就会从LRU链头开始删除，以
//	滕出空间放入新结点。 用户还可指定每次删除CACHE时一次性删除的结点数。
// @Usage:
//		Overide '.Hash()' function.
// @Notes:
//		Equality operator and relation equality operator should be overloaded.
// @MT-Safe:
//		Yes / Required.
// @SeeAlso:
//		'ICache', IHashCacheNode'
template <
class IObject, 
//The key type of this cache.
class IKey
>
class IHashCache: public ICache<IObject, IKey>
{
public:
    // Default constructor.
    IHashCache();

    // Constructor with configuring parameters.
    // @Parameters:
    //		%nMaxMem			The max memory (bytes) the cache can use.
    //		%nMaxCacheBlock		The number of blocks in the hash table.
    //		%nMaxCacheSize		The max number of nodes can be stored.
    //		%nNumToReap			The number of node being deleted every time 
    //							invoking '.Reap()'
    //		%nAjustRate			The time of hiting that will change LRU list.
    IHashCache( long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate );

    // Destructor.
    virtual ~IHashCache();

    // Reset IHashCache's parameters.
    virtual void Set(long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate );

    // Store an object to the cache with the specified key.
    bool Store (IObject Object, const IKey &Key, long nUsedMem = 0, time_t nTimeToLive = -1);

    // Fetch an object according to the key specified. ICache 
    // will return a copy of the object.
    IObject Fetch (const IKey &Key);

    // Added by Wooce
	// bit0 = 0:   existed node in cache   bit0 = 1:   newly stored node
    IObject FetchAndCache(const IKey &Key,int &flag);

    void RefreshNode(const IKey &Key);

    void RefreshNodes();

    // Delete an object from the cache according to the key specified.
    // @Description:
    //		If the key didn't exist, it wouldn't delete anything and return 
    //	true. If the key matched the item in the cache, it would delete 
    //	it from the cache and return true. Return false if something wrong 
    //	happens.
    bool Delete (const IKey &Key);

    // Pure abstract function.
    // The hash function that the derived class should overload for the 
    // hash algorthim use.
    virtual unsigned Hash( const IKey &Key ) = 0;

    // 对Cache中的内容进行遍历。
    void TraverseContainer();

    // 对LRU链进行遍历。
    void TraverseXX();

    // 回调函数，在运行TraverseContainer时对每个结点回调，可被继承类重载。
    virtual void OnTraverseContainer(IHashCacheNodeType *pNode) {};

    // 回调函数，在运行TraverseXX时对每个结点回调，可被继承类重载。
    virtual void OnTraverseXX(IHashCacheNodeType *pNode) {};

    // Get the count of objects stored in the cache.
    int GetObjectCount ();

protected:
    //Added by Wooce
    //   If an object not exists in the cache, we can use this function to get the object from other place such as database,etc
    virtual void GetCacheObject( const IKey &Key,IObject& object )=0;

    virtual void OnRefreshNode( const IKey &Key,IObject& object ){}

	// 跟据指定的KEY在CACHE中找寻相应的结点。
    IHashCacheNodeType * SearchNode (const IKey &Key);

    // 删除CACHE中结点。
    int Reap ();

    // CACHE被装满时返回TRUE。
    bool IsFull ();

    // 对HASH表中KEY对应的行进行锁定。
    void ReadLockNodeByKey (const IKey &Key);

    void WriteLockNodeByKey (const IKey &Key);

    int TryLockWhenCache(const IKey &Key);

    // 对HASH表中KEY对应的行进行解锁。
    void UnLockNodeByKey (const IKey &Key);

    // 把结点连到HASH表中。
    bool AddNodeToContainer (IHashCacheNodeType *pNode);

    // 把结点从HASH表中移除。
    bool DelNodeFromContainer (IHashCacheNodeType *pNode);

    // 把结点连入LRU链。
    bool AddNodeToXX (IHashCacheNodeType *pNode);

    // 把结点从HASH表中移除。
    bool DelNodeFromXX (IHashCacheNodeType *pNode);

    // 对整条LRU链进行封锁。
    void LockXX ();

    // 对LRU链进行解锁。
    void UnLockXX ();

    // 调整LRU链。
    bool AdjustNodeFromXX (IHashCacheNodeType *pNode);

    // 结点过期时的处理函数。
    bool TimeOutHandler (IHashCacheNodeType *pNode);

    // 把指定的结点链入IHashCache中.
    bool StoreNode (IHashCacheNodeType *pNode);

    // 删除指定结点与IHashCache的链结。
    void DeleteNode( IHashCacheNodeType* pNode );

    // 增加结点时的回调函数，在结点插入前调用。
    virtual void OnInsertNode (IHashCacheNodeType *pNode) {};

    // 删除结点时的回调函数，在结点删除前调用。
    virtual void OnDeleteNode (IHashCacheNodeType *pNode) {};

    // 提取结点时的回调函数，在找到结点且返回之前调用。
    virtual void OnFetchNode (ICacheNodeType *pNode) {};

    // 增加结点时封锁整个Cache,目的是为了保持Cache中结点数和所用内存空间
    // 与实际存入的结点相符合，不会出现结点数虚增，即结点数已增加而结点还在
    // 等符插入。
    void LockStore();

    // LockStore()的解锁操作。
    void UnLockStore();

private:
    // Initialize IHashCache's parameters.
    void Init( long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate );

    // 把'IHashCache'中占用的内存去除。
    void Destory();

protected:
    // The maximum memory can be used in the cache.
    long    m_nMaxMem;

    // The used memory used by the cachenode already in the cache.
    long    m_nUsedMem;

    // The max cachenode count can be stored in the cache.
    int     m_nCacheSize;

    // The cachenode count already stored in the cache.
    int     m_nCacheCount;

    // Max blocks in hash
    int     m_nCacheBlocks; 

    // How many objects should be cleared in one reap action
    int     m_nNumToReap;

    // Only move the object to the end of access list when it was 
    //hit m_nAdjustRate times
    int     m_nAdjustRate;      

    // The head pointers pointed to the first element in each hash table row.
    IHashCacheNodeType**        m_apCacheHead;

    // The tail pointers pointed to the last element in each hash table row.
    IHashCacheNodeType**        m_apCacheTail;

    // The head pointer pointed to the first element of LRU list(access list).
    IHashCacheNodeType*     m_pAccessListHead;

    // The tail pointer pointed to the last element of LRU list(access list).
    IHashCacheNodeType*     m_pAccessListTail;

    // An array of locks used for locking a specified row in hash table.
    // It will be used at the time adjusting the hash table which is no need to 
    // lock the whole cache.
    IRWLock*    m_LockHash;

    // A lock which locks the whole cache when adjusting LRU list.
    ILock   m_LockAccessList;

    // A lock used when invoking store function.
    ILock   m_LockStore;

};

template <class IObject, class IKey>
IHashCache<IObject, IKey>::IHashCache()
{
    m_nMaxMem = 0;
    m_nUsedMem = 0;
    m_nCacheSize = 0;
    m_nCacheCount = 0;
    m_nCacheBlocks = 0;
    m_nNumToReap = 0;
    m_nAdjustRate = 0;

    m_apCacheHead = NULL;
    m_apCacheTail = NULL;
    m_pAccessListHead = NULL;
    m_pAccessListTail = NULL;
    m_LockHash = NULL;
}

template <class IObject, class IKey>
IHashCache<IObject, IKey>::IHashCache( long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate )
{
    Init( nMaxMem, nMaxCacheBlock, nMaxCacheSize, nNumToReap, nAdjustRate );
}

template <class IObject, class IKey>
IHashCache<IObject,IKey>::~IHashCache()
{
    Destory();
}

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::Init( long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate )
{
    try
    {
        m_apCacheHead = m_apCacheTail = NULL;
        m_LockHash = NULL;
        XNew( m_apCacheHead , IHashCacheNodeType* [ nMaxCacheBlock ] );
        XNew( m_apCacheTail , IHashCacheNodeType* [ nMaxCacheBlock ] );

        m_LockHash = new IRWLock [ nMaxCacheBlock ];
        if ( m_LockHash == NULL )
            throw IException( MEMNOTENOUGH, "Not enough memory" );
    } catch (...)
    {
        if ( m_apCacheHead != NULL )
            delete[] m_apCacheHead;
        if ( m_apCacheTail != NULL )
            delete[] m_apCacheTail;
        if ( m_LockHash != NULL )
            delete[] m_LockHash;
        throw;
    }
    for ( int i=0; i<nMaxCacheBlock; i++ )
        m_apCacheHead[ i ] = m_apCacheTail[ i ] = NULL;

    m_pAccessListHead = NULL;
    m_pAccessListTail = NULL;

    m_nMaxMem = nMaxMem;
    m_nUsedMem = 0;
    m_nCacheSize = nMaxCacheSize;
    m_nCacheCount = 0;
    m_nCacheBlocks = nMaxCacheBlock;
    m_nNumToReap = nNumToReap;
    m_nAdjustRate = nAdjustRate;    
}

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::Destory()
{
    IHashCacheNodeType *p, *p1;
    if ( m_apCacheHead != NULL )
    {
        for ( int i=0; i < m_nCacheBlocks ; i++ )
        {
            p = m_apCacheHead[i];
            while ( p != NULL )
            {
                p1 = p->m_pHashNextNode;
                delete p;
                p = p1;
            }
        }
        delete m_apCacheHead;
    }
    if ( m_apCacheTail != NULL )
        delete m_apCacheTail;
    if ( m_LockHash != NULL )
        delete[] m_LockHash;
}


template <class IObject, class IKey>
void IHashCache<IObject, IKey>::Set(long nMaxMem, int nMaxCacheBlock, int nMaxCacheSize, int nNumToReap, int nAdjustRate )
{
    Destory();
    Init( nMaxMem, nMaxCacheBlock, nMaxCacheSize, nNumToReap, nAdjustRate );
}

template <class IObject, class IKey>
bool IHashCache<IObject,IKey>::Store( IObject Object, const IKey& Key, long nUsedMem, time_t nTimeToLive )
{
    IHashCacheNodeType *pNode;

    try
    {
        XNew( pNode , IHashCacheNodeType( Object, Key, nUsedMem, nTimeToLive ) );
    } catch ( IException& e )
    {
        g_Error.Out( L_ALARM, e.GetErrorInfo() );
    }
    return StoreNode( pNode );
}

template <class IObject, class IKey>
IHashCacheNodeType * IHashCache<IObject, IKey>::SearchNode (const IKey &Key)
{
    int nPos = Hash( Key ) % m_nCacheBlocks;

    IHashCacheNodeType* pNode = m_apCacheTail[ nPos ];
    while ( pNode != NULL )
    {
        if ( pNode->m_Key == Key )
            break;
        else
            pNode = pNode->m_pHashPrevNode;
    }
    if ( pNode == NULL )    //Object not in cache
        return NULL;
    return pNode;
};

template <class IObject, class IKey>
int IHashCache<IObject, IKey>::Reap ()
{
    int nDeleted = 0;
    IDEBUG( g_Error.Out( L_DEBUG, "IHashCache: Begin to reap\n") );
    for ( int i=0; i < m_nNumToReap; i++ )
    {
        LockXX();
        if ( m_pAccessListHead == NULL )
        {
            UnLockXX(); //**
            break;
        }
        IKey Key = m_pAccessListHead->m_Key;
        UnLockXX();
        if ( Delete( Key ) )
            nDeleted++;
    }
    IDEBUG( g_Error.Out( L_DEBUG, "ICache: Finished reaping.\n" ) );
    return nDeleted;
}

template <class IObject, class IKey>
bool IHashCache<IObject, IKey>::IsFull ()
{
    return m_nCacheCount == m_nCacheSize || m_nUsedMem >= m_nMaxMem; 
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::ReadLockNodeByKey ( const IKey &Key )
{
    int nPos = Hash( Key ) % m_nCacheBlocks;
    m_LockHash[ nPos ].ReadLock();
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::WriteLockNodeByKey ( const IKey &Key )
{
    int nPos = Hash( Key ) % m_nCacheBlocks;
    m_LockHash[ nPos ].WriteLock();
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::UnLockNodeByKey( const IKey &Key )
{
    int nPos = Hash( Key ) % m_nCacheBlocks;
    m_LockHash[ nPos ].Release();
};

template <class IObject, class IKey>
int IHashCache<IObject, IKey>::TryLockWhenCache ( const IKey &Key )
{
    int nPos = Hash( Key ) % m_nCacheBlocks;
    return m_LockHash[ nPos ].TryWriteLock();
};

template <class IObject, class IKey>
bool IHashCache<IObject, IKey>::AddNodeToContainer (IHashCacheNodeType *pNode)
{
    IHashCacheNodeType *pHashNode = (IHashCacheNodeType *)pNode;

    int nPos = Hash( pHashNode->m_Key )  % m_nCacheBlocks;

    if ( m_apCacheHead[ nPos ] == NULL )
    {
        m_apCacheHead[ nPos ] = pHashNode;
        m_apCacheTail[ nPos ] = pHashNode;
    } else
    {
        IHashCacheNodeType* pTail = m_apCacheTail[ nPos ];
        pHashNode->m_pHashPrevNode = pTail;
        //item->m_pHashNextNode = NULL;	//ommited
        pTail->m_pHashNextNode = pHashNode;
        m_apCacheTail[ nPos ] = pHashNode;
    }
    return true;
};

template <class IObject, class IKey>
bool IHashCache<IObject, IKey>::DelNodeFromContainer (IHashCacheNodeType *pNode)
{
    IHashCacheNodeType *pPrevNode = pNode->m_pHashPrevNode;
    IHashCacheNodeType *pNextNode = pNode->m_pHashNextNode;

    int nPos = Hash( pNode->m_Key )  % m_nCacheBlocks;

    if ( pPrevNode == NULL )
        m_apCacheHead[ nPos ] = pNextNode;
    else
        pPrevNode->m_pHashNextNode = pNextNode;

    if ( pNextNode == NULL )
        m_apCacheTail[ nPos ] = pPrevNode;
    else
        pNextNode->m_pHashPrevNode = pPrevNode;
    IDEBUG( g_Error.Out( L_DEBUG, "IHashCache: delete node from container.\n") );
    return true;
};

template <class IObject, class IKey>
bool IHashCache<IObject, IKey>::AddNodeToXX (IHashCacheNodeType *pNode)
{
    if ( m_pAccessListHead == NULL )
    {
        m_pAccessListHead = pNode;
        m_pAccessListTail = pNode;
    } else
    {
        pNode->m_pAccessListPrevNode = m_pAccessListTail;
        //item->m_pAccessListNextNode = NULL;	//ommited
        m_pAccessListTail->m_pAccessListNextNode = pNode;
        m_pAccessListTail = pNode;
    }
    return true;
};

template <class IObject, class IKey>
bool IHashCache<IObject, IKey>::DelNodeFromXX (IHashCacheNodeType *pNode)
{   
    IHashCacheNodeType* pPrevNode = pNode->m_pAccessListPrevNode;
    IHashCacheNodeType* pNextNode = pNode->m_pAccessListNextNode;

    if ( pPrevNode == NULL )
        m_pAccessListHead = pNextNode;
    else
        pPrevNode->m_pAccessListNextNode = pNextNode;

    if ( pNextNode == NULL )
        m_pAccessListTail = pPrevNode;
    else
        pNextNode->m_pAccessListPrevNode = pPrevNode;
    IDEBUG( g_Error.Out( L_DEBUG, "IHashCache: delete node from XX\n") ); 
    return true;
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::LockXX ()
{
    m_LockAccessList.Lock();
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::UnLockXX ()
{
    m_LockAccessList.ReleaseLock();
};

template <class IObject, class IKey>
bool IHashCache<IObject, IKey>::AdjustNodeFromXX (IHashCacheNodeType *pNode)
{
    pNode->m_nHits++;
    //Change the LRU list?
    if ( (pNode->m_nHits % m_nAdjustRate) == 0 )
    {
        //Move this node to the end of the list
        LockXX();
        if ( m_pAccessListTail != pNode )
        {
            // Remove node
            IHashCacheNodeType* pPrevNode = pNode->m_pAccessListPrevNode;
            IHashCacheNodeType* pNextNode = pNode->m_pAccessListNextNode;
            if ( pPrevNode == NULL )
                m_pAccessListHead = pNextNode;
            else
                pPrevNode->m_pAccessListNextNode = pNextNode;
            pNextNode->m_pAccessListPrevNode = pPrevNode;

            //Append to tail
            m_pAccessListTail->m_pAccessListNextNode = pNode;
            pNode->m_pAccessListNextNode = NULL;
            pNode->m_pAccessListPrevNode = m_pAccessListTail;
            m_pAccessListTail = pNode;
        }
        UnLockXX();
    }
    return true;
};

template <class IObject, class IKey>
bool IHashCache<IObject, IKey>::TimeOutHandler (IHashCacheNodeType *pNode)
{
    time_t now=time(NULL);
    if ( pNode->m_nTimeOut!=-1 && pNode->m_nTimeOut<now ) //This object already timeouted, shouldn't be used
    {
        DeleteNode( pNode );
        IDEBUG( g_Error.Out( L_DEBUG, "ICache: Key Expired, it will be deleted.\n" ) );
        return true;
    }
    return false;
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::LockStore()
{
    m_LockStore.Lock();
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::UnLockStore()
{
    m_LockStore.ReleaseLock();
};

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::TraverseContainer()
{
    int i;
    IHashCacheNodeType *pNode;
    g_Error.Out( L_ALARM, "-----Content of Cache------\n" );
    for (i=0; i<m_nCacheBlocks; i++)
    {
        m_LockHash[i].WriteLock();
        pNode = m_apCacheHead[ i ];
        while ( pNode != NULL )
        {
            OnTraverseContainer( pNode );
            pNode = pNode->m_pHashNextNode;
        }
        m_LockHash[i].Release();
    }   
    g_Error.Out( L_ALARM, "\nCache Count = %d , Max Cache Size = %d\n", m_nCacheCount, m_nCacheSize ) ;
    g_Error.Out( L_ALARM, "----------------------------\n" ) ;
}

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::TraverseXX()
{
    LockXX();
    g_Error.Out( L_ALARM, "---------LRU List-------------\n" ) ;
    IHashCacheNodeType *pNode = m_pAccessListHead;
    while ( pNode != NULL )
    {
        OnTraverseXX( pNode );
        pNode = pNode->m_pAccessListNextNode;
    }
    g_Error.Out( L_ALARM, "------------------------------\n" );
    UnLockXX();
}

template <class IObject, class IKey>
IObject IHashCache<IObject,IKey>::Fetch (const IKey &Key)
{   
    IObject ResObj; 
    ReadLockNodeByKey( Key );   
    IHashCacheNodeType *pNode = SearchNode( Key );  
    if ( pNode == NULL )
    {
        UnLockNodeByKey( Key );     
        return ResObj;
    }
    bool NodeTimeOut = TimeOutHandler( pNode );
    if ( NodeTimeOut )
    {
        UnLockNodeByKey( Key );
        return ResObj;
    }
    OnFetchNode( pNode );
    pNode->SetLastAccessTime( time(NULL) );
    AdjustNodeFromXX( pNode );
    ResObj = pNode->GetObject();
    IDEBUG( g_Error.Out( L_DEBUG, "ICache: Fetched the node from cache.\n" ) );
    UnLockNodeByKey( Key );
    return ResObj;
}

template <class IObject, class IKey>
IObject IHashCache<IObject,IKey>::FetchAndCache(const IKey &Key,int &flag)
{   
    IObject ResObj; 
    ReadLockNodeByKey( Key );  
    IHashCacheNodeType *pNode = SearchNode( Key );  
    if ( pNode==NULL )
    {
        UnLockNodeByKey(Key);
        if ( TryLockWhenCache(Key)!=0 )
        {
            ReadLockNodeByKey(Key);
            pNode = SearchNode(Key);
        }
    }
    if ( pNode==NULL )            //   Not Existed in the Cache
    {
        flag = 1;                 // new stored node by calling GetCacheObject()
		try
        {
            GetCacheObject(Key,ResObj);
        } catch (IException e )
        {
            UnLockNodeByKey( Key );
            throw;
        } catch ( ... )
        {
            UnLockNodeByKey( Key );
            throw;
        }
        XNew( pNode , IHashCacheNodeType( ResObj, Key, 0, -1 ) );
        LockStore();
        if ( IsFull() )
        {
            for ( int i=0; i < m_nNumToReap; i++ )
            {
                LockXX();
                if ( m_pAccessListHead == NULL )
                {
                    UnLockXX(); //**
                    break;
                }
                IHashCacheNodeType *p = m_pAccessListHead;
                OnDeleteNode( p );
                m_nCacheCount--;
                m_nUsedMem -= p->m_nSize;
                DelNodeFromXX( p );
                UnLockXX();
                DelNodeFromContainer( p );
                delete p;
            }

            if ( IsFull( ) ) //it almost couldn't be full.
            {
                //Can't complete the reap operation, can't cache any more object
                UnLockStore();
                UnLockNodeByKey(Key);
                return ResObj;
            }
        }
        OnInsertNode( pNode );
        m_nCacheCount++;
        m_nUsedMem += pNode->GetSize();
        AddNodeToContainer( pNode );
        LockXX();
        AddNodeToXX( pNode );
        UnLockXX();
        //ResObj = pNode->GetObject();
        IDEBUG( g_Error.Out( L_DEBUG, "ICache: Store a new node to cache.\n" ) );
        UnLockStore();
    } else
    {
		flag = 0;
        OnFetchNode( pNode );
        pNode->SetLastAccessTime( time(NULL) );
        AdjustNodeFromXX( pNode );
        ResObj = pNode->GetObject();
        IDEBUG( g_Error.Out( L_DEBUG, "ICache: Fetched the node from cache.\n" ) );
    }
    UnLockNodeByKey( Key );
    return ResObj;
}

template <class IObject, class IKey>
void IHashCache<IObject,IKey>::RefreshNode(const IKey &Key)
{
    IObject ResObj; 
    WriteLockNodeByKey( Key );   
    IHashCacheNodeType *pNode = SearchNode( Key );  
    if ( pNode == NULL )
    {
        UnLockNodeByKey( Key );     
        throw IException(1,"Message not exist");
    }
    pNode->SetLastAccessTime( time(NULL) );
    AdjustNodeFromXX( pNode );
    try
    {
        GetCacheObject(Key,ResObj);
		OnRefreshNode(Key,ResObj);
    } 
    catch ( ... )
    {
        UnLockNodeByKey( Key );
        throw IException(2,"Unable to load Message");
    }
    pNode->m_Object = ResObj;
    IDEBUG( g_Error.Out( L_DEBUG, "IHashCache: Refresh the content of the node in the cache.\n" ) );
    UnLockNodeByKey( Key );
}

template <class IObject, class IKey>
void IHashCache<IObject,IKey>::RefreshNodes()
{
    int i;
    IHashCacheNodeType *pNode;
    for (i=0; i<m_nCacheBlocks; i++)
    {
        m_LockHash[i].WriteLock();
        pNode = m_apCacheHead[ i ];
        while ( pNode != NULL )
        {
			IObject ResObj;
            try
            {
				GetCacheObject(pNode->GetKey(),ResObj);
				OnRefreshNode( pNode->GetKey(),ResObj );
            } catch (IException e )
            {
                m_LockHash[i].Release();
                throw;
            } catch ( ... )
            {
                m_LockHash[i].Release();
                throw;
            }
            pNode->m_Object = ResObj;
            pNode = pNode->m_pHashNextNode;
        }
        m_LockHash[i].Release();
    }   
}

template <class IObject, class IKey>
bool IHashCache<IObject,IKey>::Delete (const IKey &Key)
{
    WriteLockNodeByKey( Key );
    IHashCacheNodeType *pNode = SearchNode( Key );
    if ( pNode == NULL )
    {
        UnLockNodeByKey( Key );
        return false;
    }
    DeleteNode( pNode );
    IDEBUG( g_Error.Out( L_DEBUG, "ICache: Delete a node from cache.\n" ) );
    UnLockNodeByKey( Key );
    return true;
}

template <class IObject, class IKey>
void IHashCache<IObject, IKey>::DeleteNode( IHashCacheNodeType* pNode )
{
    OnDeleteNode( pNode );      
    m_nCacheCount--;
    m_nUsedMem -= pNode->m_nSize;
    LockXX();
    DelNodeFromXX( pNode );
    UnLockXX();
    DelNodeFromContainer( pNode );
    delete pNode;
}


template <class IObject, class IKey>
bool IHashCache<IObject,IKey>::StoreNode (IHashCacheNodeType *pNode)
{
    IObject ResObj;
    LockStore();
    if ( IsFull() )
    {
        if ( Reap()<=0 ) //Only when the objects to be free are all be freed by other thread.
            if ( IsFull( ) ) //if reap()<=0 , it almost couldn't be full.
            {
                //Can't complete the reap operation, can't cache any more object
                Delete( pNode->GetKey() );
                UnLockStore();
                return false;
            }
    }
    WriteLockNodeByKey( pNode->GetKey() );
    IHashCacheNodeType *pOldNode = SearchNode( pNode->GetKey() );
    if ( pOldNode != NULL )
        DeleteNode( pOldNode );
    OnInsertNode( pNode );
    m_nCacheCount++;
    m_nUsedMem += pNode->GetSize();
    AddNodeToContainer( pNode );
    LockXX();
    AddNodeToXX( pNode );
    UnLockXX();
    //ResObj = pNode->GetObject();
    IDEBUG( g_Error.Out( L_DEBUG, "ICache: Store a new node to cache.\n" ) );
    UnLockNodeByKey( pNode->GetKey() );
    UnLockStore();
    return true;
}

template <class IObject, class IKey>
int IHashCache<IObject,IKey>::GetObjectCount ()
{
    return m_nCacheCount;
}

#endif

