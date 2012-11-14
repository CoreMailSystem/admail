#include "parsedmail.h"

ParsedMail::ParsedMail(const IString &mail)
{
    DwInitialize();
    DwString messageStr = mail.CConstStr();
    msg = DwMessage::NewMessage(messageStr,0);
    msg->Parse();
    if( msg->Headers().ContentType().Type()==DwMime::kTypeMultipart )
    {
        bIsMultipar = true;
    }
    else
    {
        bIsMultipar = false;
    }
}

IString ParsedMail::Get() const
{
	IString mail_msg = msg->AsString().c_str();
	return mail_msg;
}

ParsedMail::~ParsedMail()
{
    if( msg!=NULL )
        delete msg;
}
