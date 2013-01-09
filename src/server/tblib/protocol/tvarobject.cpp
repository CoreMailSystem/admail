//related discussion,see http://wooce.iteye.com/blog/528583
//mailto:wooce@21cn.com
#include "tvarobject.h"
#include "iauto.h"
#include "tattributeset.h"

//-------------------------------------------------------------------------

TVarObject::TVarObject()
{
	m_Data.m_nType = VAR_TYPE_NONE;
//	m_Data.m_nSize = 0;
	m_Data.m_nValue = 0;
};


TVarObject::TVarObject( int nValue )
{
	m_Data.m_nType = VAR_TYPE_NONE;
	SetIntValue(nValue);
};

TVarObject::TVarObject( const IString &strValue )
{
	m_Data.m_nType = VAR_TYPE_NONE;
	SetStrValue(strValue);
};

TVarObject::TVarObject( const char *strValue )
{
	m_Data.m_nType = VAR_TYPE_NONE;
	SetStrValue(strValue);
};

TVarObject::TVarObject( const TBinary &binValue )
{
	m_Data.m_nType = VAR_TYPE_NONE;
	SetBinValue(binValue);
};

TVarObject::TVarObject (const TVarArray &arrayValue)
{
	m_Data.m_nType = VAR_TYPE_NONE;
	SetArrayValue(arrayValue);
};

TVarObject::TVarObject (const TAttributeSet &attrSetValue)
{
	m_Data.m_nType = VAR_TYPE_NONE;
	SetAttributeSetValue(attrSetValue);
};

TVarObject::TVarObject( const TVarObject &right )
{
	m_Data.m_nType = VAR_TYPE_NONE;
	Clone( right );
};

TVarObject::~TVarObject()
{
	SetNull();
};

void TVarObject::SetIntValue (int nValue)
{
	if (m_Data.m_nType != VAR_TYPE_NONE && m_Data.m_nType != VAR_TYPE_INT )
		throw IException( -1, "The variant type doesn't match." );
	else
	{
		m_Data.m_nValue = nValue;
		m_Data.m_nType = VAR_TYPE_INT;
//		m_Data.m_nSize = sizeof(int);
	}
};

void TVarObject::SetStrValue(const IString &strValue)
{
	IString* pStr = NULL;
	XNew(pStr, IString(strValue));
	try
	{
		SetPStrValue(pStr);
	}
	catch (IException &e)
	{
		delete pStr;
		throw e;
	}
};

void TVarObject::SetBinValue(const TBinary &binValue)
{
	TBinary *pBin = NULL;
	XNew(pBin, TBinary(binValue));
	try
	{		
		SetPBinValue(pBin);
	}
	catch (IException &e)
	{	
		delete pBin;
		throw e;
	}
};

void TVarObject::SetArrayValue(const TVarArray &arrValue)
{
	int *pBounds = NULL;	
	TVarArray *pArray = NULL;	
	try
	{
		/*int i;
		XNew(pBounds, int[arrValue.GetArrayDim()]);
		for (i=0; i<arrValue.GetArrayDim(); i++)
			pBounds[i] = arrValue.GetArrayUBound(i);*/
		XNew(pArray, TVarArray(arrValue));
		SetPArrayValue(pArray);
	}
	catch (IException &e)
	{
		if (pArray != NULL)
			delete pArray;
		else
			if (pBounds != NULL)
				delete pBounds;
		throw e;
	}
};

void TVarObject::SetAttributeSetValue(const TAttributeSet &attrSetValue)
{
	TAttributeSet *pAttrSet = NULL;
	XNew(pAttrSet, TAttributeSet);
	try
	{
		*pAttrSet = attrSetValue;
		SetPAttributeSetValue(pAttrSet);
	}
	catch (IException &e)
	{	
		delete pAttrSet;
		throw e;
	}	
};

void TVarObject::SetPStrValue(IString *pstrValue)
{
	if (pstrValue == NULL)
		throw IException(-1, "The pointer of string should not be null.");
	if (m_Data.m_nType == VAR_TYPE_STR)
		delete m_Data.m_pStrValue;
	else
		if (m_Data.m_nType != VAR_TYPE_NONE)
			throw IException( -1, "The variant type doesn't match." );
	m_Data.m_pStrValue = pstrValue;
	m_Data.m_nType = VAR_TYPE_STR;
//	m_Data.m_nSize = (*m_Data.m_pStrValue).GetLen();
};

void TVarObject::SetPBinValue(TBinary *pBinValue)
{
	if (pBinValue == NULL)
		throw IException(-1, "The bin value should not be null.");
	if (m_Data.m_nType == VAR_TYPE_BIN)
		delete m_Data.m_pBinValue;
	else
		if (m_Data.m_nType != VAR_TYPE_NONE)
			throw IException( -1, "The variant type doesn't match." );
	m_Data.m_pBinValue = pBinValue;
	m_Data.m_nType = VAR_TYPE_BIN;
//	m_Data.m_nSize = nSize;
};

void TVarObject::SetPArrayValue(TVarArray *pArrValue)
{
	if (pArrValue == NULL)
		throw IException(-1, "The array value should not be null.");
	if (m_Data.m_nType == VAR_TYPE_ARRAY)
		delete m_Data.m_pArrayValue;
	else
		if (m_Data.m_nType != VAR_TYPE_NONE)
			throw IException( -1, "The variant type doesn't match." );
	m_Data.m_pArrayValue = pArrValue;
	m_Data.m_nType = VAR_TYPE_ARRAY;
//	m_Data.m_nSize = 0;		//dynamic calculate
};

void TVarObject::SetPAttributeSetValue(TAttributeSet *pAttrSet)
{
	if (pAttrSet == NULL)
		throw IException(-1, "The attrset value should not be null.");
	if (m_Data.m_nType == VAR_TYPE_ATTRSET)
		delete m_Data.m_pAttrSetValue;
	else
		if (m_Data.m_nType != VAR_TYPE_NONE)
			throw IException( -1, "The variant type doesn't match." );
	m_Data.m_pAttrSetValue = pAttrSet;
	m_Data.m_nType = VAR_TYPE_ATTRSET;
//	m_Data.m_nSize = 0;		//dynamic calculate
};

void TVarObject::SetNull()
{
	switch ( m_Data.m_nType )
	{
	case VAR_TYPE_INT:
		m_Data.m_nValue = 0;
		break;
	case VAR_TYPE_STR:
		if ( m_Data.m_pStrValue != NULL )
			delete m_Data.m_pStrValue;
		break;
	case VAR_TYPE_BIN:
		if ( m_Data.m_pBinValue != NULL )
			delete m_Data.m_pBinValue;
		break;
	case VAR_TYPE_ARRAY:
		if ( m_Data.m_pArrayValue != NULL )
			delete m_Data.m_pArrayValue;
		break;
	case VAR_TYPE_ATTRSET:
		if (m_Data.m_pAttrSetValue != NULL)
			delete m_Data.m_pAttrSetValue;
		break;
	}
	m_Data.m_nType = VAR_TYPE_NONE;
//	m_Data.m_nSize = 0;
};

bool TVarObject::IsNull() const 
{
	return m_Data.m_nType == VAR_TYPE_NONE;
};

int TVarObject::GetType() const
{
	return m_Data.m_nType;
};


void TVarObject::Clone(const TVarObject &var)
{
	switch (var.m_Data.m_nType)
	{
	case VAR_TYPE_INT:
		SetIntValue( var.m_Data.m_nValue );
		break;
	case VAR_TYPE_STR:
		SetStrValue( *var.m_Data.m_pStrValue );
		break;
	case VAR_TYPE_BIN:
		SetBinValue( *var.m_Data.m_pBinValue );
		break;
	case VAR_TYPE_ARRAY:
		//copy the array
		SetArrayValue( *var.m_Data.m_pArrayValue );
		break;
	case VAR_TYPE_ATTRSET:
		SetAttributeSetValue(*var.m_Data.m_pAttrSetValue);
		break;
	case VAR_TYPE_NONE:
		SetNull();
		break;
	default:
		throw IException( -1, "TVarObject.Clone: Unknown type.\n" );
	}
};

/*
int TVarObject::GetSize() const
{
	switch ( m_Data.m_nType )
	{
	case VAR_TYPE_INT:		
	case VAR_TYPE_STR:			
	case VAR_TYPE_BIN:
		return m_Data.m_nSize;
	case VAR_TYPE_ARRAY:
		return 0;
//		return m_Data.m_pArrayValue->GetSize();
	default:
		throw IException(-1, "Unknown variant type.");
	}
}
*/


int	&TVarObject::AsInt()
{
	if (m_Data.m_nType == VAR_TYPE_INT)	
		return m_Data.m_nValue;
	else	
		throw IException( -1, "TVarObject.GetInt: The type doesn't match.\n" );
};

const int &TVarObject::AsInt() const
{
	if (m_Data.m_nType == VAR_TYPE_INT)	
		return m_Data.m_nValue;
	else	
		throw IException( -1, "TVarObject.GetInt: The type doesn't match.\n" );
};

IString &TVarObject::AsString()
{
	///return const_cast<IString &>(AsString());
	if (m_Data.m_nType == VAR_TYPE_STR)
		return (*m_Data.m_pStrValue);
	else
		throw IException( -1, "TVarObject.AsString: The type parsing is error.\n" );
};

const IString &TVarObject::AsString() const
{
	if (m_Data.m_nType == VAR_TYPE_STR)
		return (*m_Data.m_pStrValue);
	else
		throw IException( -1, "TVarObject.AsString: The type parsing is error.\n" );
};

TBinary &TVarObject::AsBinary ()
{
	if (m_Data.m_nType == VAR_TYPE_BIN) 
		return (*m_Data.m_pBinValue);
	else	
		throw IException( -1, "The variant type is not buffer.");	
};

const TBinary &TVarObject::AsBinary() const
{
	if (m_Data.m_nType == VAR_TYPE_BIN) 
		return (*m_Data.m_pBinValue);
	else	
		throw IException( -1, "The variant type is not buffer.");	
};

TVarArray &TVarObject::AsArray()
{
	if (m_Data.m_nType == VAR_TYPE_ARRAY)
		return (*m_Data.m_pArrayValue);
	else
		throw IException( -1, "TVarObject.AsArray: The type parsing is error.\n" );
};

const TVarArray &TVarObject::AsArray() const
{
	if (m_Data.m_nType == VAR_TYPE_ARRAY)
		return (*m_Data.m_pArrayValue);
	else
		throw IException( -1, "TVarObject.AsArray: The type parsing is error.\n" );
};

TAttributeSet &TVarObject::AsAttributeSet()
{
	if (m_Data.m_nType == VAR_TYPE_ATTRSET)
		return (*m_Data.m_pAttrSetValue);
	else
		throw IException( -1, "TVarObject.AsAttributeSet: The type parsing is error.\n" );
};

const TAttributeSet &TVarObject::AsAttributeSet() const
{
	if (m_Data.m_nType == VAR_TYPE_ATTRSET)
		return (*m_Data.m_pAttrSetValue);
	else
		throw IException( -1, "TVarObject.AsAttributeSet: The type parsing is error.\n" );
};

IString TVarObject::ToString() const
{
	IString strRes;	
	switch ( m_Data.m_nType )
	{
	case VAR_TYPE_INT:		
		strRes = IntToStr( m_Data.m_nValue, 10 );
		break;
	case VAR_TYPE_STR:		
		strRes = (*m_Data.m_pStrValue);
		break;
	case VAR_TYPE_BIN:		
		strRes = (char *)m_Data.m_pBinValue;
		break;
	case VAR_TYPE_NONE:
		strRes = "NULL";
		break;
	default:		//no array and attributeset
		throw IException( -1, "TVariant.AsString: The type parsing is error.\n" );
	}
	return strRes;	
};

const TVarObject &TVarObject::operator = (const TVarObject &right)
{
	Clone(right);
	return *this;
};

