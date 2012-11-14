/**
 * Title:adtemplate.h
 * Description:  
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author:<a href="mailto:wooce@21cn.com">Wooce Yang</a>
 * @version 1.0
 */

#ifndef _ADTEMPLATE_H
#define _ADTEMPLATE_H

#include "istring.h"
#include "idatetime.h"
#include "tattributeset.h"

class ADTemplate
{
public:
    // Default constructor
    // @Parameters:
    //		%mail		The mail template which contain macros like "%TONAME" that can be replaced in Get function.
    ADTemplate();

    // Return ad letter from the mail template, including a replacing macros process
    // @Parameters:
    //		%attrlist	.
    IString Parse(const IString &mail, const TAttributeSet &attrlist );

        // Create System related value 
    IString Time();
};

/*
class ADTemplate
{
public:
    // Default constructor
    // @Parameters:
    //		%mail		The mail template which contain macros like "%TONAME" that can be replaced in Get function.
    ADTemplate( const IString &mail );

    // Return ad letter from the mail template, including a replacing macros process
    // @Parameters:
    //		%attrlist	.
    IString Get( const TAttributeSet &attrlist );

        // Create System related value 
    IString Time();

private:
    IString adtemplate;
};*/

#endif
