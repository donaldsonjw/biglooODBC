 /**************************************************************************/
 /* Copyright(c) 2010 Joseph Donaldson(donaldsonjw@yahoo.com) 		   */
 /* This file is part of biglooODBC.					   */
 /* 									   */
 /*     biglooODBC is free software: you can redistribute it and/or modify */
 /*     it under the terms of the GNU Lesser General Public License as	   */
 /*     published by the Free Software Foundation, either version 3 of the */
 /*     License, or (at your option) any later version.			   */
 /* 									   */
 /*     biglooODBC is distributed in the hope that it will be useful, but  */
 /*     WITHOUT ANY WARRANTY; without even the implied warranty of	   */
 /*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  */
 /*     Lesser General Public License for more details.			   */
 /* 									   */
 /*     You should have received a copy of the GNU Lesser General Public   */
 /*     License along with biglooODBC.  If not, see			   */
 /*     <http://www.gnu.org/licenses/>.					   */
 /**************************************************************************/

#include "codbc.h"
#include <string.h>

static void
odbc_error( char *who, char *message, obj_t object ) 
{
  C_SYSTEM_FAILURE( BGL_ERROR, who, message, object );
}

static void report_odbc_error(char* who,
			      SQLSMALLINT handle_type,
			      SQLHANDLE handle)
{
  SQLTCHAR SQLErrorMessage[512];
  SQLTCHAR SQLState[6];
  SQLINTEGER NativeError = 0;
  SQLSMALLINT ErrMsgLength = 0;
  
  
  SQLGetDiagRec(handle_type, handle, 1, SQLState, &NativeError,
		SQLErrorMessage, sizeof(SQLErrorMessage), &ErrMsgLength);

  odbc_error(who,SQLErrorMessage,string_to_bstring(SQLState));
}

SQLHENV bgl_make_odbc_environment(void)
{			 
  SQLHENV res = SQL_NULL_HANDLE;
  
  SQLRETURN v = SQLAllocHandle(SQL_HANDLE_ENV,
			       SQL_NULL_HANDLE,
			       &res);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_make_odbc_environment",
			SQL_HANDLE_ENV, 
			res);
    }

  return res;
}

void bgl_free_odbc_environment(SQLHANDLE env)
{
  SQLRETURN v = SQLFreeHandle(SQL_HANDLE_ENV, env);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_free_odbc_environment",
			SQL_HANDLE_ENV, 
			env);
    }
}

SQLHDBC bgl_make_odbc_connection(SQLHANDLE env)
{
  SQLHDBC res;
  
  SQLRETURN v = SQLAllocHandle(SQL_HANDLE_DBC,
			       env,
			       &res);

  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_make_odbc_connection",
			SQL_HANDLE_ENV, 
			env);
    }

  return res;
}

void bgl_free_odbc_connection(SQLHANDLE dbc)
{
  SQLRETURN v = SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_free_odbc_connection",
			SQL_HANDLE_DBC, 
			dbc);
    }
}

SQLHSTMT bgl_make_odbc_statement(SQLHANDLE dbc)
{
  SQLHSTMT res = SQL_NULL_HANDLE;
  SQLRETURN v;
  v = SQLAllocHandle(SQL_HANDLE_STMT,
		     dbc,
		     &res);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_make_odbc_statement",
			SQL_HANDLE_DBC, 
			dbc);
    }
  return res;
}

void bgl_free_odbc_statement(SQLHANDLE stmt)
{
  SQLRETURN v = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_free_odbc_statement",
			SQL_HANDLE_STMT, 
			stmt);
    }
}



void bgl_odbc_driver_connect(SQLHANDLE dbc, char* constr)
{
  SQLRETURN v;
  v = SQLDriverConnect(dbc,NULL,constr,SQL_NTS,NULL,
		       0,NULL,SQL_DRIVER_NOPROMPT);

  if(!SQL_SUCCEEDED(v))
    {
	
      report_odbc_error("bgl_odbc_driver_connect",
			SQL_HANDLE_DBC, 
			dbc);
    }
}


void bgl_odbc_sql_exec_direct(SQLHANDLE stmt, char* sqlstring)
{
  SQLRETURN v;
  
  v = SQLExecDirect(stmt, sqlstring, strlen(sqlstring));
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_sql_exec_direct",
			SQL_HANDLE_STMT, 
			stmt);
    }
}

int bgl_odbc_num_result_cols(SQLHANDLE stmt)
{
  int res = 0;
  SQLSMALLINT cols = 0;
  SQLRETURN v = SQLNumResultCols(stmt,&cols);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_num_result_cols",
			SQL_HANDLE_STMT, 
			stmt);
    }

  return cols;
}

char* bgl_odbc_get_data_as_string(SQLHANDLE stmt, unsigned int colnum)
{
  char* res = NULL;
  SQLLEN length = 0;
  SQLLEN resLength = 0;
  SQLRETURN v;
  // obtain maximum column width in characters
  v = SQLColAttribute(stmt, 
		      (SQLUSMALLINT)colnum, 
		      SQL_DESC_DISPLAY_SIZE,
		      NULL,
		      0,
		      NULL,
		      &length);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_get_data_as_string",
			SQL_HANDLE_STMT, 
			stmt);
    }
  
  res = (char*)GC_MALLOC_ATOMIC(sizeof(char)*length+1);
  
  v = SQLGetData(stmt,
		 (SQLUSMALLINT)colnum,
		 SQL_C_CHAR,
		 res, 
		 length+1, 
		 &resLength);
  
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_get_data_as_string",
			SQL_HANDLE_STMT, 
			stmt);
    }
  if(resLength <= 0)
    {
      res[0] = '\0';
    }
  
  return res;
}

obj_t bgl_odbc_sql_fetch(SQLHANDLE stmt)
{
  obj_t res = BTRUE;
  SQLRETURN v = SQLFetch(stmt);
  if(!SQL_SUCCEEDED(v) && SQL_NO_DATA != v)
    {
      report_odbc_error("bgl_odbc_sql_fetch",
			SQL_HANDLE_STMT, 
			stmt);
    }

  if(SQL_NO_DATA == v)
    {
      res = BFALSE;
    }

  return res;
}

void bgl_odbc_disconnect(SQLHANDLE dbc)
{
  SQLRETURN v = SQLDisconnect(dbc);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_disconnect",
			SQL_HANDLE_DBC, 
			dbc);
    }
}


void bgl_odbc_free_statement(SQLHANDLE stmt, 
			     obj_t option)
{
  SQLRETURN v;
  SQLSMALLINT opt;

  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(option)), "close"))
    {
      opt = SQL_CLOSE;
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(option)), "unbind"))
    {
      opt = SQL_UNBIND;
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(option)), "reset-params"))
    {
      opt = SQL_RESET_PARAMS;
    }
  else
    {
      odbc_error("bgl_odbc_free_statement", "invalid option", option);
    }
  
  v = SQLFreeStmt(stmt,opt);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_free_statement",
			SQL_HANDLE_STMT,
			stmt);
    }
}

void bgl_odbc_sql_cancel(SQLHANDLE stmt)
{
  SQLRETURN v;
  v = SQLCancel(stmt);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_sql_cancel",
			SQL_HANDLE_STMT,
			stmt);
    }
}


void bgl_odbc_sql_set_connect_attr(SQLHANDLE dbc,
				   obj_t attribute,
				   obj_t value)
{
  SQLRETURN v;
  SQLUINTEGER uintval = 0;
  SQLPOINTER valueptr = 0;
  SQLINTEGER stringlength = 0;
  SQLINTEGER attr = 0;
  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "access-mode"))
    {
      attr = SQL_ATTR_ACCESS_MODE;
      if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "read-only"))
	{
	  uintval = SQL_MODE_READ_ONLY;
	  valueptr = (SQLPOINTER)uintval;
	}
      else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "read-write"))
	{
	  uintval = SQL_MODE_READ_WRITE;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_connect_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "asynch-enable"))
    {
      attr = SQL_ATTR_ASYNC_ENABLE;
      if(TRUEP( value ))
	{
	  uintval = SQL_ASYNC_ENABLE_ON;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  
	  uintval = SQL_ASYNC_ENABLE_OFF;
	  valueptr = (SQLPOINTER)uintval;
	}
	  
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "auto-ipd"))
    {
      attr = SQL_ATTR_AUTO_IPD;
      if(TRUEP( value ))
	{
	  uintval = SQL_TRUE;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  
	  uintval = SQL_FALSE;
	  valueptr = (SQLPOINTER)uintval;
	}
	  
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "autocommit"))
    {
      attr = SQL_ATTR_AUTOCOMMIT;
      if(TRUEP( value ))
	{
	  uintval = SQL_AUTOCOMMIT_OFF;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  uintval = SQL_AUTOCOMMIT_ON;
	  valueptr = (SQLPOINTER)uintval;
	}
	  
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "connection-timeout"))
    {
      attr = SQL_ATTR_CONNECTION_TIMEOUT;
      if(INTEGERP( value ))
	{
	  uintval = CINT(value);
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_connect_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "login-timeout"))
    {
      attr = SQL_ATTR_LOGIN_TIMEOUT;
      if(INTEGERP( value ))
	{
	  uintval = CINT(value);
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_connect_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "current-catalog"))
    {
      attr = SQL_ATTR_CURRENT_CATALOG;
      if(STRINGP( value ))
	{
	  
	  valueptr = (SQLPOINTER)BSTRING_TO_STRING(value);
	  stringlength = strlen(BSTRING_TO_STRING(value));
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_connect_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "metadata-id"))
    {
      attr = SQL_ATTR_METADATA_ID;
      if(TRUEP( value ))
	{
	  uintval = SQL_TRUE;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  uintval = SQL_FALSE;
	  valueptr = (SQLPOINTER)uintval;
	}
	  
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "odbc-cursor"))
    {
      attr = SQL_ATTR_ODBC_CURSORS;
      if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "use-if-needed"))
	{
	  uintval = SQL_CUR_USE_IF_NEEDED;
	  valueptr = (SQLPOINTER)uintval;
	}
      else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "use-odbc"))
	{
	  uintval = SQL_CUR_USE_ODBC;
	  valueptr = (SQLPOINTER)uintval;
	}
      else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "use-driver"))
	{
	  uintval = SQL_CUR_USE_DRIVER;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_connect_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
    }
  else
    {
      odbc_error("bgl_odbc_sql_set_connect_attr", 
		 "Invalid Or Unsupported attribute",
		 MAKE_PAIR(attribute, value));
    }

  v = SQLSetConnectAttr(dbc,
			attr,
			valueptr,
			stringlength);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_sql_set_connect_attr",
			SQL_HANDLE_DBC,
			dbc);
					  
    }
						
}

void bgl_odbc_sql_end_tran(obj_t handletype,
			   SQLHANDLE handle,
			   obj_t completiontype)
{
  SQLRETURN v;
  SQLSMALLINT ht;
  SQLSMALLINT ct;
  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(handletype)), "environment"))
    {
      ht = SQL_HANDLE_ENV;
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(handletype)), "connection"))
    {
      ht = SQL_HANDLE_DBC;
    }
  else
    {
      odbc_error("bgl_odbc_sql_end_tran", 
		 "Invalid handle type",
		 handletype);
    }

  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(completiontype)), "commit"))
    {
      ct = SQL_COMMIT;
    }
  else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(handletype)), "rollback"))
    {
      ct = SQL_ROLLBACK;
    }
  else
    {
      odbc_error("bgl_odbc_sql_end_tran", 
		 "Invalid completion type",
		 completiontype);
    }

  v = SQLEndTran(ht,
		 handle,
		 ct);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_sql_end_tran",
			ht,
			handle);
    }
  
}

void bgl_odbc_sql_set_env_attr(SQLHENV env,
			       obj_t attribute,
			       obj_t value)
{
  SQLRETURN v;
  SQLUINTEGER uintval = 0;
  SQLPOINTER valueptr = 0;
  SQLINTEGER stringlength = 0;
  SQLINTEGER attr = 0;

  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "connection-pooling"))
    {
      attr = SQL_ATTR_CONNECTION_POOLING;
      if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(value)), "off"))
	{
	  uintval = SQL_CP_OFF;
	  valueptr = (SQLPOINTER)uintval;
	}
      else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(value)), "one-per-driver"))
	{
	  uintval = SQL_CP_ONE_PER_DRIVER;
	  valueptr = (SQLPOINTER)uintval;
	}
      else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(value)), "one-per-environment"))
	{
	  uintval = SQL_CP_ONE_PER_HENV;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_env_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
	  
    }
  else  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "match"))
    {
      attr = SQL_ATTR_CP_MATCH;
      if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(value)), "strict"))
	{
	  uintval = SQL_CP_STRICT_MATCH;
	  valueptr = (SQLPOINTER)uintval;
	}
      else if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(value)), "relaxed"))
	{
	  uintval = SQL_CP_ONE_PER_DRIVER;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_env_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
	
    }
  else  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "odbc-version"))
    {
      attr = SQL_ATTR_ODBC_VERSION;
      if(INTEGERP(value))
	{
	  uintval = (SQLUINTEGER)CINT(value);
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  odbc_error("bgl_odbc_sql_set_env_attr", 
		     "Invalid attribute value",
		     MAKE_PAIR(attribute, value));
	}
    }
  else  if(0 == strcmp(BSTRING_TO_STRING(SYMBOL_TO_STRING(attribute)), "output-nts"))
    {
      attr = SQL_ATTR_OUTPUT_NTS;
      if(TRUEP(value))
	{
	  uintval = SQL_TRUE;
	  valueptr = (SQLPOINTER)uintval;
	}
      else
	{
	  uintval = SQL_FALSE;
	  valueptr = (SQLPOINTER)uintval;
	}
    }
  else
    {
      odbc_error("bgl_odbc_sql_set_env_attr", 
		 "Invalid or Unsupported attribute ",
		 attribute);
    }

  v = SQLSetEnvAttr(env,
		    attr,
		    valueptr,
		    stringlength);
  if(!SQL_SUCCEEDED(v))
    {
      report_odbc_error("bgl_odbc_sql_set_env_attr",
			SQL_HANDLE_ENV,
			env);
    }
}
