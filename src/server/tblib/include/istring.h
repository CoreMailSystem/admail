//This IString class is derived from the TStr class in : http://wooce.iteye.com/blog/517021
#ifndef __ISTRING_H
#define __ISTRING_H

// will throw a message of char* if memory allocation fails

#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "iexception.h"
#include "port.h"
const int DEFAULT_ISTRING_LENG = 16;
class IString
{
public:
// Constructors
    IString(void);							// constructs empty IString
    IString(const IString& ts );			// copy constructor
	IString(char ch,int nRepeat=1);			// from a single character
    IString(const char *s );				// from an ANSI string
	IString(const char *pc,int nLength);	// from a ANSI string or binary array
// Attributes & Operations
	size_t size() const { return GetLen(); }
	size_t length() const { return GetLen(); }
	IString& append(const char* aBuf, size_t aLen) { return AddBuffer(aBuf,aLen); }

    int     GetLen(void) const;			// get data length
	bool	IsEmpty(void) const;		// true if zero length
	void	Empty(void) ;				// clear contents to empty
    void	Release(void);				// clear contents to empty and release the memroy
	const char *  ClearStr(int nLen=-1);		// clear contents to empty and reset the buffer size, nLen not include '\0'
	const char * CConstStr() const;		// return pointer to const string
	operator const char *() const;		// return pointer to const string
	char	GetAt(int i)const;			// return single character at zero-based index
    char &  operator []( int i );		// return single character at zero-based index
    char    operator []( int i )const;	// return single character at zero-based index
	void	SetLen(void);				// reset the string's length
	void	SetAt(int pos,char ch);		// set a single character at zero-based index

    char * Clone(void) const;			// copy the string to a buffer (use memory alloc operator : new)

	void	Attach(char * lpNewBuf);
	void	Attach(char * lpNewBuf,int nStringLength);
	void	Attach(char * lpNewBuf,int nStringLength,int nBufLength);
	char *	Detach(void);

// Access to string implementation buffer as "C" character array
    char*   CStr();						// get pointer to modifiable buffer at least as long as nMinBufLength
// Implementation
    ~IString();
// overloaded assignment
	const	IString& operator=(const IString& string);	// ref-counted copy from another IString
	const	IString& operator=(const char ch);			// set string content to single character
	const	IString& operator=(const char * pStr);		// copy string content from ANSI string
// string concatenation
	const	IString& operator+=(const IString& string);	// concatenate from another CString
	const	IString& operator+=(const char ch);			// concatenate a single character
	const	IString& operator+=(const char * pStr);		// concatenate string from ANSI string
	const	IString& operator+=(int num);				// concatenate string from Int value
	const	IString& operator+=(double num);			// concatenate string from double value
	friend IString operator+(const IString& string1,const IString& string2);
	friend IString operator+(const IString& string, char ch);
	friend IString operator+(char ch,const IString& string);
	friend IString operator+(const IString& string,const char * psz);
	friend IString operator+(const char * psz,const IString& string);
// string comparison
    int		Compare(const char * psz) const;			// straight character comparison
	int		ICompare(const char * psz) const;			// compare ignoring case
	int		NCompare(const char * psz,int n) const;		// straight character comparison but looks at a maximum of n bytes
	int		INCompare(const char * psz,int n) const;	// compare ignoring case but looks at a maximum of n bytes
	int		Collate(const char * psz)	const;			// NLS aware comparison, case sensitive

    int		SubCompare(const char * psz,int nTargLen = -1,int nFrom = 0) const;		// straight character comparison
	int		SubNCompare(const char * psz,int nTargLen = -1,int nFrom = 0) const;	// straight character comparison

	bool    operator ==( const IString& ts ) const;
    bool    operator ==( const char * ts ) const ;
    bool    operator !=( const IString& ts ) const ;
    bool    operator !=( const char * ts )  const ;
    bool    operator >=( const IString& ts ) const ;
    bool    operator <=( const IString& ts ) const ;
    bool    operator > ( const IString& ts ) const ;
    bool    operator < ( const IString& ts ) const ;
// simple sub-string extraction
	IString SubString ( int start, int len ) const;				// return a substring starting at zero-based start which length is len
	IString Mid(int nFirst) const;								// return all characters starting at zero-based nFirst
	IString Left(int nCount) const;								// return first nCount characters in string
	IString Right(int nCount) const;							// return nCount characters from end of string
	IString SpanIncluding(const char * lpszCharSet) const;		//  characters from beginning that are also in passed string
	IString SpanExcluding(const char * lpszCharSet) const;		// characters from beginning that are not also in passed string
// upper/lower/reverse conversion
	IString& MakeUpper(void);			// NLS aware conversion to uppercase
	IString& MakeLower(void);			// NLS aware conversion to lowercase
	IString& MakeReverse(void);			// NLS aware conver upper to lowercase and lowercase to upper
	IString& ReverseOrder(void);		// reverse string right-to-left
// trimming whitespace (either side)
	IString& LTrim(void);				// remove whitespace starting from left edge
    IString& RTrim(void);				// remove whitespace starting from right side
    IString& LRTrim(void);				// remove whitespace starting from both left and right side
// trimming anything (either side)
	IString &TrimRight(char chTarget);	// remove continuous occcurrences of characters in passed string, starting from right
	IString &TrimLeft(char chTarget);	// remove continuous occcurrences of characters in passed string, starting from left
	IString &TrimRight(const char * lpszTargets);	// remove continuous occcurrences of characters in passed string, starting from right
	IString &TrimLeft(const char * lpszTargets);	// remove continuous occcurrences of characters in passed string, starting from left
//added by elephant--------------------2000.6.8----------
	IString &TrimRight2(char chTarget);	// remove continuous occcurrences of characters in passed string, starting from right
	IString &TrimLeft2(char chTarget);	// remove continuous occcurrences of characters in passed string, starting from left
	IString &TrimRight2(const char * lpszTargets);	// remove continuous occcurrences of characters in passed string, starting from right
	IString &TrimLeft2(const char * lpszTargets);	// remove continuous occcurrences of characters in passed string, starting from left
//<------------------end of add--------------------------
// advanced manipulation
	int      Replace(char chOld, char chNew);						// replace occurrences of chOld with chNew and return how many char be changed
	int		 Replace(const char * lpszOld, const char * lpszNew);	// replace occurrences of substring lpszOld with lpszNew
    IString& Ins( int pos, const char * str);						// insert substring at zero-based pos
    IString& Ins( int pos, char ch);								// insert a char at zero-based pos
    IString& Del( int pos, int len);								// delete len characters starting at zero-based pos
	IString& Del( const char * pstr,int begin=0,int count=1);		// delete a string starting at zero-based begin and repeat do it count times
	IString& AddBuffer(const char * lpBuf,int nLen);				// add nLen characters from a binary buffer
	IString& SetBuffer(const char * lpBuf,int nLen);				// set string content to nLen characters from a binary buffer
	IString& InsBuffer(const char * lpBuf,int nPos,int nLen);		// insert nLen characters from a binary buffer at nPos
// searching
	int		Find(char ch,int nBegin=0) const;							// find character starting at zero-base nBegin, -1 if not found
	int		ReverseFind(char ch) const;									// find character starting at right
    int     Find(const char *pstr,int nBegin=0) const;					// find first instance of substring starting at zero-based index
	int		Findncase(const char *pstr,int nBegin=0) const;				// find first instance of substring starting at zero-based index ignoring case
	int		FindInclude(const char * pscCharSet,int nBegin=0) const;	//find the first char which in the chars
	int		FindExcluing(const char * pszCharSet,int nBegin=0) const;	//find the first char which not inthe chars
// KMP Searching
	int *	MakeNext(void);												// auto malloc memroy using operator : new
	int		Find(const char * pszString,int * pnNext,int nBegin=0) const;
	int		Findncase(const char * pszString,int * pnNext,int nBegin=0) const;
	int		FindWithWildChar(const char * pszStr,char chSglWildChar='?',char chMltWildChar='*') const;
	bool	MatchWithWildChar(const char * pszStr,char chSglWildChar='?',char chMltWildChar='*') const;
// simple formatting
	IString& Format(const char *fmt, ... );							// printf-like formatting using passed string
    IString& FormatVa(const char *fmt, const va_list va );			// printf-like formatting using variable arguments parameter
// simple convert
    int     ToInt(int nBegin=0) const;						// convert to int type value at nBegin
	long	ToLong(int nBegin=0,int base=10) const;			// convert to long type value at nBegin
	LONGLONG ToLongLong(int nBegin=0,int base=10) const;		// convert to long long type value at nBegin
    double  ToDouble(int nBegin=0) const;						// convert to double type value at nBegin
// load string from file
    bool    LoadTextFile( const IString& fname );				// load from a file which name is fname
    int     LoadTextFile( int fid ,int nLength=-1);
    int     LoadTextFile( FILE * fp, int nLength=-1);
// save the string to file
	bool	SaveToFile( const IString& fname ) const;
	int		SaveToFile( int fid,int nLength=-1) const;
	int		SaveToFile( FILE * fp,int nLength=-1) const;
//-------------------------------------------------------------------
protected:
    char    *str;										// pointer to buffer storage of the string characters
    int     m_length, buflen;							// the length of the string and the length of the buffer
// implementation helpers
    void    Alloc( int size );							// alloc memory use memory alloc operator : new, if not enough memory will throw IException
	int		Alloc2( int size );							// the same as Alloc, but when memory not enough will not throw IException and return -1
	void	ReAlloc(int size);							// realloc memory
    friend  IString FmtStr(const char *fmt, ... );
};
//-------------------------------------------------------------------
const int MAXFORMATLEN=32000;
IString FmtStr(const char *fmt, ... );
IString operator+(const IString& string1,const IString& string2);
IString operator+(const IString& string, char ch);
IString operator+(char ch,const IString& string);
IString operator+(const IString& string,const char * psz);
IString operator+(const char * psz,const IString& string);
IString ToUpper( IString ts );
IString ToLower( IString ts );
IString CharToStr( char ch );
IString IntToStr( LONGLONG  i, int radix=10 );
int *get_next(const char * str,int * next,int length);	//not auto alloc memory
char *	wildcharfind(const char * pszSrc,const char * pszStr,char chSglWildChar='?',char chMltWildChar='*',char ** lastpoint=NULL);

int incore_strcasecmp(const char * str1,const char * str2);
int incore_strncasecmp(const char * str1,const char * str2,int n);

#endif
