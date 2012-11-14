/**
 * Title:adtarget.h
 * Description:  
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author:<a href="mailto:wooce@21cn.com">Wooce</a>,SCUT(South China University of Technology)
 * @version 1.0
 */
#ifndef _ADTARGET_H
#define _ADTARGET_H

//#include <string>
//#include <set>
//#include <algorithm>
//#include <utility>
#include "istring.h"
#include "tattributeset.h"
#include "itokens.h"
#include "ilist.h"

// The class's goal is to judge whether a target is met by a user attribute set quickly and efficiently
// @Notes:
//		
// @SeeAlso:
//

//using namespace std;

//using namespace std;

//typedef set<string> TStringSet;
//typedef set< string,less<string> > TStringSet;

//A class for caculating a flight target expression according to the user 
//attributes.
class ADTarget
{
public:
    ADTarget() {};

	//@Description:
	//Caculate a target expression with the user attributes.
	//@Param:
	//	%asTarget	The target expression that is going to be caculated. It 
	//				is stored by a specified format.
	//	%attrs		User attributes that would be placed with the variable 
	//				of the expression. The values of this user attributes
	//				will be compared with the const in the expression and
	//				return the result finally.
	//@Return:
	//	true	if the result of expression is true after caculating.
	//	false	if the result of expression is false after caculating.
    bool IsAccorded(const TAttributeSet& asTarget, const TAttributeSet& attrs);

private:
	//@Description:
	//	Or clause matching. Evaluate all the 'and' clauses and return 
	//their 'or' result.
	//@Param:
	//	%asSubClause	The set of and clauses.
	//	%attrs			User attributes.
	//@Return:
	//	true		The result is true after caculating.
	//	false		The result is false after caculating.
    bool IsClauseAccorded(const TAttributeSet& asSubClause, const TAttributeSet& attrs);

	//@Description:
	//	And clause matching. Evaluate all the 'atom' expression and 
	//return their 'and' result.
	//@Param:
	//	%asCondClause	The set of 'atom' expression.
	//	%attrs			User attributes.
	//@Return:
	//	true		The result is true after caculating.
	//	false		The result is false after caculating.
    bool IsConditionAccorded(const TAttributeSet& asCondClause, const TAttributeSet& attrs);
    
	//@Description:
	//	Evaluate the expression whose operands are number type.
	//@Param:
	//	%nOP1		The left operand of the expression.
	//	%strPredication		The operator of the expression.
	//	%nOP2		The right operand of the expression.
	//@Return:
	//	true		The result is true after caculating.
	//	false		The result is false after caculating.
    bool IsNumberMatch( int nOP1, const IString &strPredication,int nOP2 );
    
	//@Description:
	//	Evaluate the expression whose operands are string type.
    bool IsStringMatch( const IString &strOP1,const IString &strPredication, const IString &strOP2);
    
    //bool IsEnumMatch( IString userValue,IString EnumValue );

	//@Description:
	//	Evaluate the expression whose operands are set.
    bool IsSetMatch(const IList<IString>& userSet,const IString &strPredication, const IList<IString>& targetSet );

	//@Description:
	//	Set function. Return true if all '%sSet''s elements included in 
	//'%dSet', otherwise return false.
	bool IsSetInclude(const IList<IString>& sSet, const IList<IString> &dSet);

	//@Description:
	//	Set function. Return true if any elements in '%sSet' included in 
	//'%dSet', otherwise return false.	
	bool IsSetIntersect(const IList<IString>& sSet, const IList<IString> &dSet);

	//@Description:
	//	Set function. Return true if all '%sSet''s elements included in 
	//'%dSet' and the number of elements is equal, otherwise return false.
	bool IsSetEqual(const IList<IString>& sSet, const IList<IString> &dSet);
	
	void ShowFormula(const TAttributeSet &asTarget);
};




/*class ADTarget
{
public:
    ADTarget( IString& target );

    bool IsAccorded( const TAttributeSet& kvList );

protected:
    bool IsClauseAccorded( const TAttributeSet& kvList,IString& subclause );

    bool IsConditionAccorded( const TAttributeSet& kvList, IString& Condition );
    
    bool IsNumberMatch( int userValue,IString Predication,int Value );
    
    bool IsStringMatch( IString userValue,IString RegExp );
    
    //bool IsEnumMatch( IString userValue,IString EnumValue );

    bool IsSetMatch( TStringSet& userSet,IString Predication,TStringSet& targetSet );

private:
    IList<ITokenItem> clauseList;
    int clausecount;
};*/

#endif

