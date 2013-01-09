//related discussion,see http://wooce.iteye.com/blog/528583
#ifndef THASHMAP_H
#define THASHMAP_H

#include "tabstractmap.h"
#include "ilist.h"

//#define THashNode<TKey, TNode> THashNode<TKey, TNode>

//TNode should be overided operator =
template <class TKey, class TNode>
class THashNode
{
public:
    THashNode (const TKey Key, const TNode &Object)
	{ 
		m_Key = Key;
		m_Object = Object; 
		m_pHashNext = NULL;
		m_pHashPrev = NULL;
	};

    ~THashNode () {};	

	TNode GetObject() { return m_Object; };

	TKey GetKey() { return m_Key; };

    THashNode *m_pHashNext;

    THashNode *m_pHashPrev;

private:
	TKey m_Key;

    TNode m_Object;	

	THashNode( THashNode &);

	THashNode &operator = ( THashNode & );
};

//An map container class that implemented 'TAbstractMap' interface.
//It provided the operation of a hash storage strategy which has double
//direction list(bucket). 
//This class make no guarantees as to the order of objects in this map.
//For more efficency, the parameter of block count should be set
//to a proper value after considering about the time and space costs.
//The value of this parameter is the buckets that this hash container has.
//Higher value will increase the space but decrease the lookup costs, and 
//lower value is opposite. 
//Be noted that this map class didn't support multi-values, the meaning of
//this multi-values is that objects has the same key value. The class can 
//store them but can't fetch each except the first one.
//@SeeAlso:
//	'TAbstractMap'
template<class TKey, class TNode>
class THashMap :public TAbstractMap<TKey, TNode>
{
public:
	//@Description:
	//	Construct a map container which has 'nBlockCount' buckets.
	//@Params:
	//	%nBlockCount	The buckets count in this hash container.
	THashMap(int nBlockCount);

	~THashMap();

	//@Description:
	//	Store an Object assicated with '%Key' to the container.
	//It will append the object even it has previously a mapping key 
	//in the map.
	//@Params:
	//	%Key	The key (identity) of the object in the container.
	//	%Object	The context of the node being stored.
	//@Return:
	//	Return true	if the object can be stored successfully, 
	//otherwise falsw will be returned.
	bool Store(const TKey &Key, const TNode &Object);

	//@Description:
	//	Fetch an object whose key is '%Key', it will fetch the first
	//object that mapping the key.
	//@Params:
	//	%Key	The key of the object that is going to be fetched.
	//	%Node	The place to store the context of the object which is fetched.
	//@Return:
	//	Return true if the object can be fetched, false if the object
	//does not exist.	
	bool Fetch(const TKey &Key, TNode &Node);

	//@Description:
	//	Delete the object associated the '%Key' value, it will delete
	//the first object that mapping the key.
	//@Params:
	//	%Key	The key of the object going to be deleted.
	//@Return:
	//	Return true if the object is deleted successfully, false
	//if the object can be deleted.
	bool Delete (const TKey &Key);

	//Return the object count in the container.
    int GetCount ();

	//Clear all context in the container.
    void Clear ();

	//Get all keys in the container, it can be use to iterate the context
	//of container.
	void GetKeySet( IList<TKey> &KeySet );

	//The hash function that should be override in different hash container.
	//A good algorithm should be provided to decrease the time and space
	//costs.
	virtual unsigned Hash( const TKey &Key ) = 0;


protected:
	int m_nCount;

	int m_nBlockCount;

	int m_nSize;

	THashNode<TKey, TNode> **m_pHead;

	THashNode<TKey, TNode> **m_pTail;

	THashNode<TKey, TNode> *SearchNode( const TKey &Key );

private:
	THashMap (const TAbstractMap &right);

    THashMap & operator = (const TAbstractMap &right);
};

template <class TKey, class TNode>
THashMap<TKey, TNode>::THashMap(int nBlockCount)
{
	try
	{
		m_pHead = m_pTail = NULL;
		m_nCount = 0;
		m_nBlockCount = nBlockCount;
		m_pHead = new THashNode<TKey, TNode> *[nBlockCount];
		m_pTail = new THashNode<TKey, TNode> *[nBlockCount];
		for ( int i=0; i<nBlockCount; i++ )
			m_pHead[i] = m_pTail[i] = NULL;
	}
	catch (...)
	{
		if ( m_pHead != NULL )
			delete[] m_pHead;
		if ( m_pTail != NULL )
			delete[] m_pTail;
		throw;
	}
};

template <class TKey, class TNode>
THashMap<TKey, TNode>::~THashMap()
{
	THashNode<TKey, TNode> *p, *p1;
	if ( m_pHead != NULL )
	{
		for ( int i=0; i<m_nBlockCount; i++ )
		{
			p = m_pHead[i];
			while ( p != NULL )
			{
				p1 = p->m_pHashNext;
				delete p;
				p = p1;
			}
		}
		delete m_pHead;
	}
	if ( m_pTail != NULL ) 
		delete[] m_pTail;
};

template <class TKey, class TNode>
bool THashMap<TKey, TNode>::Store(const TKey &Key, const TNode &Object)
{
	try
	{
		THashNode<TKey, TNode> *pNode = NULL;
		pNode = new THashNode<TKey, TNode>( Key, Object );
		int nPos = Hash( Key ) % m_nBlockCount;
		if ( m_pHead[nPos] == NULL )
		{
			m_pHead[nPos] = pNode;
			m_pTail[nPos] = pNode;
		}
		else
		{
			THashNode<TKey, TNode> *pTailNode = m_pTail[nPos];
			pNode->m_pHashPrev = pTailNode;
			pTailNode->m_pHashNext = pNode;
			m_pTail[nPos] = pNode;
		}
		++m_nCount;
		return true;
	}
	catch (IException &e)
	{
		g_Error.Out( L_ALARM, e.GetErrorInfo() );
		return false;
	}
};

template <class TKey, class TNode>
bool THashMap<TKey, TNode>::Fetch(const TKey &Key, TNode &Node )
{	
	try
	{
		THashNode<TKey, TNode> *pNode = SearchNode( Key );
		if ( pNode != NULL )		
		{
			Node = pNode->GetObject();
			return true;
		}
		else
			return false;
	}
	catch (IException &e)
	{
		g_Error.Out( L_WARNING, e.GetErrorInfo() );
		return false;
	}
};

template <class TKey, class TNode>
THashNode<TKey, TNode> *THashMap<TKey, TNode>::SearchNode (const TKey &Key)
{
	int nPos = Hash( Key ) % m_nBlockCount;
	THashNode<TKey, TNode> *pNode = m_pHead[nPos];
	while ( pNode != NULL )
	{
		if ( pNode->GetKey() == Key )
			break;
		else
			pNode = pNode->m_pHashNext;
	}
	if ( pNode == NULL )
		return NULL;
	else
		return pNode;
};

template <class TKey, class TNode>
bool THashMap<TKey, TNode>::Delete (const TKey &Key)
{
	try
	{
		THashNode<TKey, TNode> *pNode = SearchNode( Key );
		int nPos = Hash( Key ) % m_nBlockCount;
		if ( pNode != NULL )
		{
			THashNode<TKey, TNode> *pPrev, *pNext;
			pPrev = pNode->m_pHashPrev;
			pNext = pNode->m_pHashNext;
			if ( pPrev == NULL )			
				m_pHead[nPos] = pNext;
			else
				pPrev->m_pHashNext = pNext;
			if ( pNext == NULL )
				m_pTail[nPos] = pPrev;
			else 
				pNext->m_pHashPrev = pPrev;
			delete pNode;
			--m_nCount;
		}
		return true;

	}
	catch (IException &e)
	{
		g_Error.Out( L_ALARM, e.GetErrorInfo() );
		return false;
	}
};

template <class TKey, class TNode>
int THashMap<TKey, TNode>::GetCount ()
{
	return m_nCount;
};

template <class TKey, class TNode>
void THashMap<TKey, TNode>::GetKeySet( IList<TKey> &KeySet )
{	
	try
	{
		KeySet.ClearAll();
		THashNode<TKey, TNode> *p, *p1;
		if ( m_pHead != NULL )
		{
			for ( int i=0; i<m_nBlockCount; i++ )
			{
				p = m_pHead[i];
				while ( p != NULL )
				{
					p1 = p->m_pHashNext;
					KeySet.Append( p->GetKey() );
					p = p1;
				}
			}		
		}	
	}
	catch (IException &e)
	{
		g_Error.Out( L_ALARM, e.GetErrorInfo() );
		throw;
	}
};

template <class TKey, class TNode>
void THashMap<TKey, TNode>::Clear ()
{
	THashNode<TKey, TNode> *p, *p1;
	if ( m_pHead != NULL )
	{
		for ( int i=0; i<m_nBlockCount; i++ )
		{
			p = m_pHead[i];
			while ( p != NULL )
			{
				p1 = p->m_pHashNext;
				delete p;
				p = p1;
			}
		}		
	}
};

#endif