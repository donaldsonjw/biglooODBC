;;;; Copyright(c) 2010 Joseph Donaldson(donaldsonjw@yahoo.com) 
;;;; This file is part of biglooODBC.
;;;;
;;;;     biglooODBC is free software: you can redistribute it and/or modify
;;;;     it under the terms of the GNU Lesser General Public License as
;;;;     published by the Free Software Foundation, either version 3 of the
;;;;     License, or (at your option) any later version.
;;;;
;;;;     biglooODBC is distributed in the hope that it will be useful, but
;;;;     WITHOUT ANY WARRANTY; without even the implied warranty of
;;;;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;;;;     Lesser General Public License for more details.
;;;;
;;;;     You should have received a copy of the GNU Lesser General Public
;;;;     License along with biglooODBC.  If not, see
;;;;     <http://www.gnu.org/licenses/>.
(directives
   (extern
	(include "codbc.h")
	(type $sqlhandle void* "SQLHANDLE")
	(type $sqlhandle* (pointer $sqlhandle) "SQLHANDLE*")
	(type $sqlhenv $sqlhandle "SQLHENV")
	(type $sqlhdbc $sqlhandle "SQLHDBC")
	(type $sqlhstmt $sqlhandle "SQLHSTMT")
	(type $sqlhdesc $sqlhandle "SQLHDESC")
	(type $sqlsmallint short "SQLSMALLINT")
    (type $sqlreturn $sqlsmallint "SQLRETURN")
	(macro $sql-null-handle::$sqlhandle "SQL_NULL_HANDLE")
	;; handle types
	(macro $sql-handle-desc::$sqlsmallint "SQL_HANDLE_DESC")
	(macro $sql-handle-dbc::$sqlsmallint "SQL_HANDLE_DBC")
	(macro $sql-handle-env::$sqlsmallint "SQL_HANDLE_ENV")
	(macro $sql-handle-stmt::$sqlsmallint "SQL_HANDLE_STMT")

	(macro $make-odbc-environment::$sqlhenv ()
		   "bgl_make_odbc_environment")
	(macro $free-odbc-environment::void (env::$sqlhenv)
		   "bgl_free_odbc_environment")
	(macro $make-odbc-connection::$sqlhdbc (env::$sqlhenv)
		   "bgl_make_odbc_connection")
	(macro $make-odbc-statement::$sqlhstmt (env::$sqlhdbc)
		   "bgl_make_odbc_statement")
	(macro $free-odbc-connection::void (con::$sqlhdbc)
		   "bgl_free_odbc_connection")
	(macro $free-odbc-statement::void (stmt::$sqlhstmt)
		   "bgl_free_odbc_statement")
	(macro $odbc-driver-connect::obj (con::$sqlhdbc constr::string)
		   "bgl_odbc_driver_connect")
	(macro $odbc-disconnect::void (con::$sqlhdbc)
		   "bgl_odbc_disconnect")
	(macro $odbc-sql-exec-direct::obj (stmt::$sqlhstmt sqlstr::string)
		   "bgl_odbc_sql_exec_direct")
	(macro $odbc-num-result-cols::int (stmt::$sqlhstmt)
		   "bgl_odbc_num_result_cols")

	(macro $odbc-get-data-as-string::string (stmt::$sqlhstmt col::uint)
		   "bgl_odbc_get_data_as_string")
	(macro $odbc-sql-fetch::obj (stmt::$sqlhstmt)
		   "bgl_odbc_sql_fetch")
	(macro $odbc-free-statement::void (stmt::$sqlhstmt option::obj)
		   "bgl_odbc_free_statement")
	(macro $odbc-sql-cancel::void (stmt::$sqlhstmt)
		   "bgl_odbc_sql_cancel")
	(macro $odbc-sql-set-connect-attr::void (dbc::$sqlhdbc
											 attribute::obj
											 value::obj)
		   "bgl_odbc_sql_set_connect_attr")
	(macro $odbc-sql-end-tran::void (handletype::obj
									 handle::$sqlhandle
									 completiontype::obj)
		   "bgl_odbc_sql_end_tran")
	(macro $odbc-sql-set-env-attr::void (env::$sqlhenv
										 attribute::obj
										 value::obj)
		   "bgl_odbc_sql_set_env_attr")
	
   ))
