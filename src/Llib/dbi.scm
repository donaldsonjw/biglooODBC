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
(module dbi
   (import odbc)
   (export
	(abstract-class database)
	(class odbc-database::database (%odbc-database-init)
	   connection-string
	   (env (default #unspecified))
	   (conn (default #unspecified))
	   (stmt (default #unspecified)))
	(generic database-close db::database)
	(generic database-eval db::database proc sql)
	(generic database-map db::database proc sql)
	(generic database-for-each db::database proc sql)))

(define (%odbc-database-init db::odbc-database)
   (with-access::odbc-database db (connection-string env conn stmt)
	  (set! env (instantiate::odbc-environment))
	  (odbc-set-env-attr env 'odbc-version 3)
	  (set! conn (instantiate::odbc-connection (env env)))
	  (odbc-connect conn connection-string)
	  (set! stmt (instantiate::odbc-statement (conn conn)))))


(define-generic (database-close db::database))
(define-method (database-close db::odbc-database)
   (with-access::odbc-database db (env conn stmt)
	  (odbc-handle-free! stmt)  
	  (odbc-disconnect conn)
	  (odbc-handle-free! conn)
	  (odbc-handle-free! env)))

(define-generic (database-exec db::database sql))
(define-method (database-exec db::odbc-database str)
   (with-access::odbc-database db (env conn stmt)
	  (odbc-exec-direct stmt str)
	  (let ((cols (odbc-num-result-cols stmt)))
		 (if (odbc-fetch stmt)
			 (do ((i 1 (+ i 1))
				  (res '() (cons (odbc-get-data-as-string stmt i) res)))
				  ((> i cols) (reverse! res)))
			 '()))
	  (odbc-free-statement stmt 'close)))


(define-generic (database-eval db::database proc sql)
   (apply proc (database-exec db sql)))


(define-generic (database-map db::database proc sql))
(define-method (database-map db::odbc-database proc sql)
   (with-access::odbc-database db (env conn stmt)
	  (unwind-protect
		 (begin 
			(odbc-exec-direct stmt sql)
			(let ((cols (odbc-num-result-cols stmt)))
			   (let loop ((cont (odbc-fetch stmt))
						  (r '()))
				  (if cont
					  (do ((i 1 (+ i 1))
						   (res '() (cons (odbc-get-data-as-string stmt i)
										  res)))
						  ((> i cols)(loop (odbc-fetch stmt)
										   (cons (proc (reverse! res)) r))))
					  (reverse! r)))))
		 (odbc-free-statement stmt 'close))))


(define-generic (database-for-each db::database proc sql))
(define-method (database-for-each db::odbc-database proc sql)
   (with-access::odbc-database db (env conn stmt)
	  (unwind-protect
		(begin 
		   (odbc-exec-direct stmt sql)
		   (let ((cols (odbc-num-result-cols stmt)))
			  (let loop ((cont (odbc-fetch stmt)))
				 (when cont
					(do ((i 1 (+ i 1))
						 (res '() (cons (odbc-get-data-as-string stmt i) res)))
						((> i cols) (begin (proc (reverse! res))
										   (loop (odbc-fetch stmt)))))))
			  #unspecified))		
		(odbc-free-statement stmt 'close))))









	  
													 

	   