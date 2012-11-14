#include "parsedtemplate.h"

IString ParsedTemplate::Parse( const TAttributeSet &attrlist ) const
{
    ADTemplate ad;
	IString parsedResult;
    if( bIsMultipar )
    {
        DwMessage* nMsg = new DwMessage( *msg );    // Deep copy, operate with nMsg won't affect msg itself
        nMsg->Parse();
        MultipartMessage mail_msg(nMsg);
        IString newSubject = ad.Parse( mail_msg.Subject().c_str(),attrlist );
        mail_msg.SetSubject( newSubject.CConstStr() );
        MultipartBodyPart part;
        for( int i=0;i<mail_msg.NumberOfParts();i++ )
        {
            mail_msg.BodyPart(i,part);
            if( part.TypeStr()=="text" )
            {
                IString newBody = ad.Parse( part.Body().c_str(),attrlist );
                part.SetBody( newBody.CConstStr() );
                mail_msg.SetBodyPart(i,part);
            }
        }
        parsedResult = mail_msg.AsString().c_str();
    }
    else
    {
        DwMessage* nMsg = new DwMessage( *msg );    // Deep copy, operate with nMsg won't affect msg itself
        nMsg->Parse();
        BasicMessage mail_msg(nMsg);
        IString newSubject = ad.Parse( mail_msg.Subject().c_str(),attrlist );
        mail_msg.SetSubject( newSubject.CConstStr() );
        IString newBody = ad.Parse( mail_msg.Body().c_str(),attrlist );
        mail_msg.SetBody( newBody.CConstStr() );
        parsedResult = mail_msg.AsString().c_str();
    }
	return parsedResult;
}
