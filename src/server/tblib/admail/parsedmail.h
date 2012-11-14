/**
 * Title:parsedmail.h
 * Description:  
 * Copyright:    Copyright (c) 2001
 * Company:
 * @author:<a href="mailto:wooce@21cn.com">Wooce Yang</a>
 * @version 1.0
 */

#ifndef _PARSEDMAIL_H
#define _PARSEDMAIL_H

#include "istring.h"
#include "idatetime.h"
#include "tattributeset.h"
#include <mimepp/mimepp.h>
#include <mimepp/message.h>

#ifndef  DW_USE_ANSI_IOSTREAM
#include <iostream>
#include <fstream>
#if ! (defined(_WIN32) || defined(__WIN32__))
    using std::ifstream;
    using std::ofstream;
    using std::cerr;
#endif
#else 
#include <iostream.h>
#include <fstream.h>
#endif

#include "basicmsg.h"
#include "multipar.h"

class ParsedMail
{
public:
    // Return ad letter from the mail template, including a replacing macros process
    // @Parameters:
    //		%attrlist	.
    ParsedMail(const IString &mail);
    
	IString Get() const;

    ~ParsedMail();
protected:
    DwMessage *msg;
    bool bIsMultipar;
};


#endif

