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

#ifndef CODBC_H
#define CODBC_H
#include "bigloo.h"
#include "sql.h"
#include "sqlext.h"
#include "sqltypes.h"

SQLHENV bgl_make_odbc_environment(void);
void bgl_free_odbc_environment(SQLHANDLE env);
SQLHDBC bgl_make_odbc_connection(SQLHANDLE env);
void bgl_free_odbc_connection(SQLHANDLE dbc);
SQLHSTMT bgl_make_odbc_statement(SQLHANDLE dbc);
void bgl_free_odbc_statement(SQLHANDLE stmt);
void bgl_odbc_driver_connect(SQLHANDLE dbc, char* constr);
void bgl_odbc_sql_exec_direct(SQLHANDLE stmt, char* sqlstring);
int bgl_odbc_num_result_cols(SQLHANDLE stmt);
char* bgl_odbc_get_data_as_string(SQLHANDLE stmt, unsigned int colnum);
obj_t bgl_odbc_sql_fetch(SQLHANDLE stmt);
void bgl_odbc_disconnect(SQLHANDLE dbc);
void bgl_odbc_free_statement(SQLHANDLE stmt, 
			     obj_t option);
void bgl_odbc_sql_cancel(SQLHANDLE stmt);
void bgl_odbc_sql_set_connect_attr(SQLHANDLE dbc,
				   obj_t attribute,
				   obj_t value);
void bgl_odbc_sql_end_tran(obj_t handletype,
			   SQLHANDLE handle,
			   obj_t completiontype);
void bgl_odbc_sql_set_env_attr(SQLHENV env,
			       obj_t attribute,
			       obj_t value);


#endif /*CODBC_H*/
