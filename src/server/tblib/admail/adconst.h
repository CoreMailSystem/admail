#ifndef _ADCONST_H_
#define _ADCONST_H_

#define  AM_TABLE_ADVERTISER       "am_advertiser"
#define  AM_TABLE_INSERTION_ORDER  "am_insertion_order"
#define  AM_TABLE_FLIGHT           "am_flight"
#define  AM_TABLE_ADLETTER         "am_adletter"
#define  AM_TABLE_SYSTEM_FLIGHT    "am_system_flight"
#define  AM_TABLE_DOMAIN           "am_domain"
#define  AM_TABLE_ATTRIBUTESET     "am_attributeset"
#define  AM_TABLE_ADMINISTRATOR    "am_administrator"

enum ADLoginType
{
	AD_LOGTYPE_WEB = 1,
	AD_LOGTYPE_POP3 = 2,
	AD_LOGTYPE_IMAP = 3,
	AD_LOGTYPE_OTHER = 9
};

enum ADServChnID
{
	ADSERV_CHNID_SYSTEM = 0,
	ADSERV_CHNID_ADVER = 1
};

enum 
{
	AD_PROTOCOLTYPE_SERV = 0x0101,
	AD_PROTOCOLTYPE_SERV_ADMIN = 0x0102,
	AD_PROTOCOLTYPE_STORAGE = 0x0201,
	AD_PROTOCOLTYPE_STORAGE_MBOX = 0x0202,
	AD_PROTOCOLTYPE_STORAGE_ADMIN = 0x0203
};


#endif