/**
 * Title:parsedtemplate.h
 * Description:  
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author:<a href="mailto:wooce@21cn.com">Wooce</a>
 * @version 1.0
 */

#ifndef _PARSEDTEMPLATE_H
#define _PARSEDTEMPLATE_H

#include "parsedmail.h"
#include "adtemplate.h"

class ParsedTemplate: public ParsedMail
{
public:
    ParsedTemplate(const IString &m): ParsedMail(m){ }
    IString Parse( const TAttributeSet &attrlist ) const;
	virtual ~ParsedTemplate(){}
};

#endif

