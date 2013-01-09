//related discussion,see http://wooce.iteye.com/blog/528583
#ifndef _TVAROBJECT_H_
#define _TVAROBJECT_H_

//#include "tglobal.h"
#include "ibuffer.h"
#include "istring.h"
#include "tvararray.h"
#include "tbinary.h"

class TAttributeSet;

enum TVarType {
				VAR_TYPE_NONE = 0,
				VAR_TYPE_INT = 1,
				VAR_TYPE_STR = 2,
				VAR_TYPE_BIN = 3,
				VAR_TYPE_ARRAY = 4,
				VAR_TYPE_ATTRSET = 5
			};

struct TVarData
{
	int m_nType;

//	int m_nSize;		//maybe no use...

	union
	{
		int m_nValue;

		IString *m_pStrValue;

		TBinary *m_pBinValue;
		
		TVarArray *m_pArrayValue;
		
		TAttributeSet *m_pAttrSetValue;
	};	
};

class TVarObject 
{
  public:	  
      TVarObject ();

	  //Array Constructor
	  //TVarObject(int nDims, int *nBounds);

	  //TVarObject for common type
      TVarObject (int nValue);
      TVarObject (const IString &strValue);
      TVarObject (const char *strValue);
      TVarObject (const TBinary &binValue);
	  TVarObject (const TVarArray &arrayValue);
	  TVarObject (const TAttributeSet &attrSetValue);

	  TVarObject (const TVarObject &right);

      ~TVarObject();
  
//      void Get (int &nValue) const;
//      void Get (IString &strValue) const;

	  void SetNull();

	  bool IsNull() const; 

//	  int GetSize() const;

//	  int GetBufSize() const;

	  int GetType() const;

/*
      const TVarObject& operator = (int nValue);
      const TVarObject& operator = (const IString &strValue);
      const TVarObject& operator = (const TVarObject &var);	  
*/

	  //value
	  void SetIntValue(int nValue);
	  void SetStrValue(const IString &strValue);	  
	  void SetBinValue (const TBinary &binValue);
	  void SetArrayValue(const TVarArray &arrValue);	 
	  void SetAttributeSetValue(const TAttributeSet &attrSetValue);

	  //reference
	  void SetPStrValue(IString *pstrValue);
	  void SetPBinValue(TBinary *pBinValue);
	  void SetPArrayValue(TVarArray *pArrValue);
	  void SetPAttributeSetValue(TAttributeSet *pAttrSet);
	
	  //functions for write
	  int &AsInt();
	  IString &AsString();
	  TVarArray &AsArray();
	  TAttributeSet &AsAttributeSet();
	  TBinary &AsBinary ();

	  //fucntions for read
	  const int &AsInt() const;
	  const IString &AsString() const;
	  const TVarArray &AsArray() const;
	  const TAttributeSet &AsAttributeSet() const;
	  const TBinary &AsBinary() const;

	  IString ToString() const;

	  const TVarObject &operator = (const TVarObject &right);
    
  private: 

	  TVarData m_Data;

	  void Clone( const TVarObject &var );
};



#endif