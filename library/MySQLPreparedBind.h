#pragma once
#include <mysql.h>

#define MakeMySQLBind(varName, count)					MYSQL_BIND varName[count]; \
														memset(varName, 0, sizeof(varName));

#define MakeMySQLBind_Pointer(varName, count)			MYSQL_BIND *varName = new MYSQL_BIND[count]; \
														memset(varName, 0, sizeof(MYSQL_BIND) * count);

#define SetMySQLBind(varName, index, type, to)			varName[index].buffer_type	= type; \
														varName[index].buffer		= to;

#define SetMySQLBindU(varName, index, type, to)			SetMySQLBind(varName, index, type, to); \
														varName[index].is_unsigned = true;

#define	SetMySQLBindFloat(varName, index, to)			SetMySQLBind(varName, index, MYSQL_TYPE_SHORT, &to);	

#define	SetMySQLBindDouble(varName, index, to)			SetMySQLBind(varName, index, MYSQL_TYPE_DOUBLE, &to);	


#define SetMySQLBindChar(varName, index, to)			SetMySQLBind(varName, index, MYSQL_TYPE_TINY, &to);

#define SetMySQLBindUChar(varName, index, to)			SetMySQLBindU(varName, index, MYSQL_TYPE_TINY, &to);

#define SetMySQLBindByte(varName, index, to)			SetMySQLBindUChar(varName, index, to);

#define	SetMySQLBindShort(varName, index, to)			SetMySQLBind(varName, index, MYSQL_TYPE_SHORT, &to);	

#define	SetMySQLBindUShort(varName, index, to)			SetMySQLBindU(varName, index, MYSQL_TYPE_SHORT, &to);	


#define	SetMySQLBindInt(varName, index, to)				SetMySQLBind(varName, index, MYSQL_TYPE_LONG, &to);				  

#define	SetMySQLBindUInt(varName, index, to)			SetMySQLBindU(varName, index, MYSQL_TYPE_LONG, &to);		


#define	SetMySQLBindInt64(varName, index, to)			SetMySQLBind(varName, index, MYSQL_TYPE_LONGLONG, &to);

#define	SetMySQLBindUInt64(varName, index, to)			SetMySQLBindU(varName, index, MYSQL_TYPE_LONGLONG, &to);


#define	SetMySQLBindString(varName, index, to, len)		SetMySQLBind(varName, index, MYSQL_TYPE_VAR_STRING, (void *)to); \
														varName[index].buffer_length = (unsigned long)len;	

#define	SetMySQLBindSTDString(varName, index, to)		SetMySQLBindString(varName, index, (void *)to.c_str(), to.length());

#define	SetMySQLBindCharArray(varName, index, to, maxL)	SetMySQLBindString(varName, index, (void *)to.c_str(), (maxL ? to.maxLength() : to.length()));

/* Not added yet
MYSQL_TYPE_ENUM=247, MYSQL_TYPE_SET=248,
MYSQL_TYPE_TINY_BLOB=249, MYSQL_TYPE_MEDIUM_BLOB=250, MYSQL_TYPE_LONG_BLOB=251, MYSQL_TYPE_BLOB=252,
MYSQL_TYPE_GEOMETRY=255
*/