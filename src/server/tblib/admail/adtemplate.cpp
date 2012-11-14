#include "adtemplate.h"
#include "port.h"

typedef IString (ADTemplate::* TemplateCmdFuncType)();

typedef struct
{
    const char* lpszAttrName;           //  system related attribute key that appear after "SYSTEM." in the mail template
    TemplateCmdFuncType pCmdFunc;
}TemplateCmdDefine;

TemplateCmdDefine g_pTemplateCmd[]=
{
    { "TIME", &ADTemplate::Time},
    { NULL, NULL}
};

ADTemplate::ADTemplate()
{
}

IString ADTemplate::Time()
{
    IDateTime t = GetCurTime();
    return t.FormatTime( "%yÄê%mÔÂ%dÈÕ" );
}

IString ADTemplate::Parse(const IString &mail, const TAttributeSet &attrlist )
{
    int len = mail.GetLen();
    if ( len*0.5>10*1024 )
        len += 10*1024;
    else
        len *= 1.5;
    IString Res('\0',len );
    const char * ch;
    for (ch=mail.CConstStr();*ch!='\0';ch++)
    {
        if ( *ch=='$' )
        {
            if ( strncasecmp(ch+1,"USER.",5)==0 )
            {
                int i;
                for ( i=0;
					i<attrlist.GetSize() && 
					!( (strncasecmp(ch+6,attrlist.GetAttribute(i).GetID().CConstStr(),	
								attrlist.GetAttribute(i).GetID().GetLen())==0) && 
					*(ch+6+attrlist.GetAttribute(i).GetID().GetLen())=='$'); i++ );
                if ( i<attrlist.GetSize() )
                {
                    Res += attrlist.Get(i).AsString();
                    ch  += attrlist.GetAttribute(i).GetID().GetLen()+6;
                } else
                    Res += *ch;
            } else if ( strncasecmp(ch+1,"SYSTEM.",7)==0 )
            {
                int i=0;
                for ( i=0; g_pTemplateCmd[i].lpszAttrName!=NULL && 
					!(  (strncasecmp( ch+8,g_pTemplateCmd[i].lpszAttrName,strlen(g_pTemplateCmd[i].lpszAttrName) )==0) &&
					*(ch+8+strlen(g_pTemplateCmd[i].lpszAttrName))=='$'); i++ );
                if ( g_pTemplateCmd[i].lpszAttrName!=NULL )
                {
                    IString rValue = (this->*g_pTemplateCmd[i].pCmdFunc)();
                    Res += rValue;
                    ch += strlen(g_pTemplateCmd[i].lpszAttrName)+8;
                } else
                    Res += *ch;
            } else
                Res += *ch;
        } else
            Res += *ch;
    }
    return Res;
}


