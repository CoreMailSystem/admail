//written by Wooce Yang,from SCUT(South China University of Technology)
#include "adtarget.h"

void ADTarget::ShowFormula(const TAttributeSet &asTarget)
{
	for (int i=0; i<asTarget.GetAttribute("OrClauses").GetSize(); i++)
	{	
		TAttributeSet &asOrClause = asTarget.GetAttribute("OrClauses").Get(i).AsAttributeSet();
		if (asOrClause.Get("Reverse").AsInt()==1)
			fprintf(stderr, "not (");
		for (int j=0; j<asOrClause.GetAttribute("AndClauses").GetSize(); j++)
		{
			TAttributeSet &asFormula = asOrClause.GetAttribute("AndClauses").Get(j).AsAttributeSet();
			if (asFormula.Get("Reverse").AsInt()==1)
				fprintf(stderr, "not (");
			fprintf(stderr, "%s", asFormula.Get("OP1").AsString().CConstStr());
			fprintf(stderr, "%s", asFormula.Get("Predication").AsString().CConstStr());
			TVarObject &var = asFormula.Get("OP2");
			switch (var.GetType())
			{
			case VAR_TYPE_INT:
				fprintf(stderr, "%d", var.AsInt());
				break;
			case VAR_TYPE_STR:
				fprintf(stderr, "%s", var.AsString().CConstStr());
				break;
			case VAR_TYPE_ATTRSET:
				IList<IString> targetSet;
				for(int idx=0; idx<var.AsAttributeSet().GetAttribute("values").GetSize(); idx++ )
					targetSet.Append(var.AsAttributeSet().GetAttribute("values").Get(idx).AsString());
				fprintf(stderr, "[");
				for_list(IString, targetSet, pNode)
					fprintf(stderr, " %s ", pNode->GetInfo().CConstStr());
				fprintf(stderr, "]");
				break;
			}
			if (asFormula.Get("Reverse").AsInt()==1)
				fprintf(stderr, ")");
			if (j != asOrClause.GetAttribute("AndClauses").GetSize()-1)
				fprintf(stderr, " and ");
		}
		if (asOrClause.Get("Reverse").AsInt()==1)
			fprintf(stderr, ")");
		if (i != asTarget.GetAttribute("OrClauses").GetSize()-1)
			fprintf(stderr, " or ");
	}
	fprintf(stderr, "\n");
}

bool ADTarget::IsAccorded(const TAttributeSet& asTarget, const TAttributeSet& attrs)
{
//	bool bReverse;
//	bReverse = (asTarget.Get("Reverse").Get().AsInt()==1);
	IDEBUG( ShowFormula(asTarget) );
	for (int i=0; i<asTarget.GetAttribute("OrClauses").GetSize(); i++)
		if (IsClauseAccorded(asTarget.GetAttribute("OrClauses").Get(i).AsAttributeSet(), attrs))
			return true;
	return false;
};

bool ADTarget::IsClauseAccorded(const TAttributeSet& asSubClause, const TAttributeSet& attrs)
{
    bool bReverse;
	bReverse = (asSubClause.Get("Reverse").AsInt()==1);
	for (int i=0; i<asSubClause.GetAttribute("AndClauses").GetSize(); i++)
		if (!IsConditionAccorded(asSubClause.GetAttribute("AndClauses").Get(i).AsAttributeSet(), attrs))			
			return (false^bReverse);	
	return (true^bReverse);
};

bool ADTarget::IsConditionAccorded(const TAttributeSet& asCondClause, const TAttributeSet& attrs)
{	
    bool bReverse;
	bReverse = (asCondClause.Get("Reverse").AsInt()==1);
    
    IString strOP1 = asCondClause.Get("OP1").AsString();
	IString strPredication = asCondClause.Get("Predication").AsString();		
    TVarObject &varOP2 = asCondClause.Get("OP2");    
    switch(varOP2.GetType())
    {
    case VAR_TYPE_INT:        
        return IsNumberMatch(attrs.Get(strOP1).AsInt(), strPredication, varOP2.AsInt())^bReverse;
    case VAR_TYPE_STR:        
        return IsStringMatch(attrs.Get(strOP1).AsString(),strPredication, varOP2.AsString())^bReverse;
		/*    case TYPE_ENUM:
        tok.GetToken(Value);
        return (kvList.Get(KeyName).Get().GetString()==Value)^Reverse;*/
    case VAR_TYPE_ATTRSET:
		IList<IString> targetSet,userSet;
		int i;
		if( !attrs.HasAttribute(strOP1) )   // to avoid an exception in attrs.GetAttribute(strOP1) below
			return bReverse;
        for(i=0; i<attrs.GetAttribute(strOP1).GetSize(); i++ )
            userSet.Append(attrs.GetAttribute(strOP1).Get(i).AsString());
		for(i=0; i<varOP2.AsAttributeSet().GetAttribute("values").GetSize(); i++ )
            targetSet.Append(varOP2.AsAttributeSet().GetAttribute("values").Get(i).AsString());		
        return IsSetMatch(userSet, strPredication, targetSet)^bReverse;
    }
	throw IException(-1, "ADTarget.IsConditionAccord: Type is unknown.");
};

bool ADTarget::IsNumberMatch(int nOP1, const IString &strPredication,int nOP2)
{	
	if (strPredication == "=")
		return nOP1==nOP2;
	else
	if (strPredication == ">")
		return nOP1>nOP2;
	else
	if (strPredication == ">=")
		return nOP1>=nOP2;
	else
	if (strPredication == "<")
		return nOP1<nOP2;
	else
	if (strPredication == "<=")
		return nOP1<=nOP2;
	else
		throw IException(-1, "ADTarget.IsNumberMatch: invalid operator.");
};

bool ADTarget::IsStringMatch( const IString &strOP1,const IString &strPredication, const IString &strOP2)
{
	IString strPredicationNoCase = strPredication;
	strPredicationNoCase.MakeLower();
	if (strPredicationNoCase == "equal")
		return strOP1==strOP2;
	else
	if (strPredicationNoCase == "equalignorecase")
		return const_cast<IString &>(strOP1).MakeLower()==const_cast<IString &>(strOP2).MakeLower();
	else
	if (strPredicationNoCase == "include")
		return strOP1.Find(strOP2.CConstStr())>=0;
	else
	if (strPredicationNoCase == "beginwith")
		return strOP1.Left(strOP2.GetLen())==strOP2;
	else
	if (strPredicationNoCase == "endwith")
		return strOP1.Right(strOP2.GetLen())==strOP2;
	else
	if (strPredicationNoCase == "like")
		return strOP1.MatchWithWildChar(strOP2.CConstStr());
	else
		throw IException(-1, "ADTarget.IsStringMatch: invalid operator.");
};

bool ADTarget::IsSetMatch(const IList<IString>& userSet, const IString &strPredication, const IList<IString> &targetSet )
{
	IString strPredicationNoCase = strPredication;
	strPredicationNoCase.MakeLower();
	if(strPredicationNoCase == "in")
        return IsSetInclude( userSet, targetSet );
	else
    if(strPredicationNoCase == "intersact")
        return IsSetIntersect( userSet, targetSet );
	else
		if( strPredicationNoCase == "=" )
			return IsSetEqual(userSet, targetSet);
		else
			throw IException(-1, "ADTarget.IsSetMatch: invalid operator.");
};

bool ADTarget::IsSetInclude(const IList<IString>& sSet, const IList<IString> &dSet)
{
	if (sSet.GetCount() > dSet.GetCount())
		return false;
	for_list(IString, sSet, pNode)
	{
		if (const_cast<IList<IString> &>(dSet).Find(pNode->GetInfo()) == NULL )
			return false;
	}
	return true;
};

bool ADTarget::IsSetIntersect(const IList<IString>& sSet, const IList<IString> &dSet)
{
	for_list(IString, sSet, pNode)
	{
		if (const_cast<IList<IString> &>(dSet).Find(pNode->GetInfo()) != NULL )
			return true;
	}
	return false;
};

bool ADTarget::IsSetEqual(const IList<IString>& sSet, const IList<IString> &dSet)
{
	if (sSet.GetCount() != dSet.GetCount())
		return false;
	for_list(IString, sSet, pNode)
	{
		if (const_cast<IList<IString> &>(dSet).Find(pNode->GetInfo()) == NULL )	//***suppose no same values in both sets
			return false;
	}
	return true;
};

/*
#define TYPE_NUMBER    0
#define TYPE_STRING    1
#define TYPE_ENUM      2
#define TYPE_SET       3

#define OP_SUBSET     0
#define OP_INTERSECT  1
#define OP_EQUAL      2

#define NUMOP_E       0
#define NUMOP_G       1
#define NUMOP_GE      2
#define NUMOP_L       3
#define NUMOP_LE      4

const char ChrClauseDelim = '\n';
const char ChrAtomDelim   = '\r';
const char ChrKVDelim     = '\b';

ADTarget::ADTarget( IString& target )
{
	ITokens tok( target.CStr() );
	char split[]= { ChrClauseDelim, '\0' };
	tok.SetSplit( split );
    tok.GetTokenList(clauseList);
}

bool ADTarget::IsAccorded( const TAttributeSet& kvList )
{
    for_list(ITokenItem,clauseList,pt)
        if( IsClauseAccorded(kvList,pt->GetInfo().m_strToken) )
            return true;
    return false;
}

bool ADTarget::IsClauseAccorded( const TAttributeSet& kvList,IString& subclause )
{
	ITokens tok( subclause.CStr() );
	char split[]= { ChrAtomDelim, '\0' };
	tok.SetSplit( split );
    IString Condition;
    tok.GetToken( Condition );
    bool Reverse;
    if( Condition.ToInt()==1 )
        Reverse = true;
    else
        Reverse = false;
    while( tok.GetToken(Condition)!=NULL )
        if( !IsConditionAccorded(kvList,Condition) )
            return (false^Reverse);
    return (true^Reverse);
}

bool ADTarget::IsConditionAccorded( const TAttributeSet& kvList, IString& Condition )
{
    ITokens tok( Condition.CStr() );
	char split[]= { ChrKVDelim, '\0' };
	tok.SetSplit( split );
    
    tok.GetToken( Condition );
    bool Reverse;
    if( Condition.ToInt()==1 )
        Reverse = true;
    else
        Reverse = false;
    
    IString KeyName;
    tok.GetToken(KeyName);
    IString KeyType;
    tok.GetToken(KeyType);
    IString Predication,Value;
    switch( KeyType.ToInt() )
    {
    case TYPE_NUMBER:
        tok.GetToken(Predication);
        tok.GetToken(Value);
        return IsNumberMatch( kvList.Get(KeyName).Get().GetInt(),Predication,Value.ToInt() )^Reverse;
    case TYPE_STRING:
        tok.GetToken(Value);
        return IsStringMatch( kvList.Get(KeyName).Get().GetString(),Value )^Reverse;
    case TYPE_ENUM:
        tok.GetToken(Value);
        return (kvList.Get(KeyName).Get().GetString()==Value)^Reverse;
    case TYPE_SET:
        tok.GetToken(Predication);
        TStringSet targetSet,userSet;
        IString setValue;
        while( tok.GetToken(setValue)!=NULL )
            targetSet.insert( setValue.CStr() );
        for( int i=0;i<kvList.Get(KeyName).GetSize();i++ )
            userSet.insert( kvList.Get(KeyName).Get(i).GetString().CConstStr() );
        return IsSetMatch( userSet,Predication,targetSet )^Reverse;
    }
}

bool ADTarget::IsNumberMatch( int userValue,IString Predication,int Value )
{
    switch( Predication.ToInt() )
    {
    case NUMOP_E:
        return (userValue==Value);
    case NUMOP_G:
        return (userValue>Value);
    case NUMOP_GE:
        return (userValue>=Value);
    case NUMOP_L:
        return (userValue<Value);
    case NUMOP_LE:
        return (userValue<=Value);
    }
}
    
bool ADTarget::IsStringMatch( IString userValue,IString RegExp )
{
}

bool ADTarget::IsSetMatch( TStringSet& userSet,IString Predication,TStringSet& targetSet )
{
    if( Predication.ToInt()==OP_SUBSET )
        return includes( targetSet.begin(),targetSet.end(),userSet.begin(),userSet.end() );
    if( Predication.ToInt()==OP_INTERSECT )
    {
        TStringSet result;
        insert_iterator<TStringSet> res_ins(result,result.begin());
        set_intersection( targetSet.begin(),targetSet.end(),userSet.begin(),userSet.end(),res_ins );
        if( result.size()==0 )
            return false;
        else
            return true;
    }
    if( Predication.ToInt()==OP_EQUAL )
        return (targetSet==userSet);
}
*/
