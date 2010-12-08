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
(module odbc
   (include "odbc.sch")
   (export
	(abstract-class odbc-handle (%odbc-handle-init)
	   ($builtin::$sqlhandle (default $sql-null-handle)))
	(class odbc-environment::odbc-handle)
	(class odbc-connection::odbc-handle
	   env::odbc-environment)
	(class odbc-statement::odbc-handle
	   conn::odbc-connection
	   )
	(class odbc-descriptor::odbc-handle)
	(generic odbc-handle-free! handle::odbc-handle)
	(generic odbc-connect conn::odbc-connection conn-string::string)
	(generic odbc-disconnect conn::odbc-connection)
	(generic odbc-exec-direct stmt::odbc-statement sql::string)
	(generic odbc-get-data-as-string stmt::odbc-statement col::uint)
	(generic odbc-num-result-cols stmt::odbc-statement)
	(generic odbc-fetch stmt::odbc-statement)
	(generic odbc-free-statement stmt::odbc-statement
									 option::symbol)
	(generic odbc-cancel stmt::odbc-statement)
	(generic odbc-set-connect-attr conn::odbc-connection
									   attrib::symbol
									   value)
	(generic odbc-end-tran handle::odbc-handle compl::symbol)
	(generic odbc-set-env-attr env::odbc-environment attr::symbol value)))


(define *odbc-handle-mutex* (make-mutex))
(define *odbc-handle-map* (create-hashtable :weak 'keys))

(define (odbc-handle-map-add! obj::odbc-handle)
   (with-lock *odbc-handle-mutex*
	  (lambda ()
		 (with-access::odbc-handle obj ($builtin)
			(hashtable-put! *odbc-handle-map* $builtin obj)))))



;; odbc handle constructor
(define-generic (%odbc-handle-init obj::odbc-handle))


(define-method (%odbc-handle-init obj::odbc-environment)
   (with-access::odbc-environment obj ($builtin)
	  (when (void*-null? $builtin)
		 (set! $builtin ($make-odbc-environment)))))

(define-method (%odbc-handle-init obj::odbc-connection)
   (with-access::odbc-connection obj ($builtin env)
	  (when (void*-null? $builtin)
		 (set! $builtin
			   ($make-odbc-connection (odbc-environment-$builtin env))))))

(define-method (%odbc-handle-init obj::odbc-statement)
   (with-access::odbc-statement obj ($builtin conn)
	  (when (void*-null? $builtin)
		 (set! $builtin
			   ($make-odbc-statement (odbc-connection-$builtin conn))))))


(define-generic (odbc-handle-free! handle::odbc-handle))

(define-method (odbc-handle-free! handle::odbc-environment)
   (with-access::odbc-environment handle ($builtin)
		 ($free-odbc-environment $builtin)))

(define-method (odbc-handle-free! handle::odbc-connection)
   (with-access::odbc-connection handle ($builtin)
		 ($free-odbc-connection $builtin)))

(define-method (odbc-handle-free! handle::odbc-statement)
   (with-access::odbc-statement handle ($builtin)
		 ($free-odbc-statement $builtin)))


(define-generic (odbc-connect conn::odbc-connection conn-string::string)
   (with-access::odbc-connection conn ($builtin)
	  ($odbc-driver-connect $builtin conn-string)
	  #unspecified
	  ))

(define-generic (odbc-disconnect conn::odbc-connection)
   (with-access::odbc-connection conn ($builtin)
	  ($odbc-disconnect $builtin)))


(define-generic (odbc-set-connect-attr conn::odbc-connection
									   attrib::symbol
									   value)
   (with-access::odbc-connection conn ($builtin)
	  ($odbc-sql-set-connect-attr $builtin attrib value)))


(define-generic (odbc-exec-direct stmt::odbc-statement sql::string)
   (with-access::odbc-statement stmt ($builtin)
	  ($odbc-sql-exec-direct $builtin sql)
	  #unspecified
	  ))

(define-generic (odbc-num-result-cols stmt::odbc-statement)
   (with-access::odbc-statement stmt ($builtin)
	  ($odbc-num-result-cols $builtin)))

(define-generic (odbc-get-data-as-string stmt::odbc-statement col::uint)
   (with-access::odbc-statement stmt ($builtin)
	  ($odbc-get-data-as-string $builtin col)))

(define-generic (odbc-fetch stmt::odbc-statement)
   (with-access::odbc-statement stmt ($builtin)
	  ($odbc-sql-fetch $builtin)))

(define-generic (odbc-free-statement stmt::odbc-statement
									 option::symbol)
	   (with-access::odbc-statement stmt ($builtin)
		  ($odbc-free-statement $builtin option)))

(define-generic (odbc-cancel stmt::odbc-statement)
   (with-access::odbc-statement stmt ($builtin)
		  ($odbc-sql-cancel $builtin)))



(define-generic (odbc-end-tran handle::odbc-handle compl::symbol))

(define-method (odbc-end-tran handle::odbc-environment compl::symbol)
   (with-access::odbc-environment handle ($builtin)
	  ($odbc-sql-end-tran 'environment $builtin compl)
	  #unspecified))

(define-method (odbc-end-tran handle::odbc-connection compl::symbol)
   (with-access::odbc-connection handle ($builtin)
	  ($odbc-sql-end-tran 'connection $builtin compl)
	  #unspecified))


(define-generic (odbc-set-env-attr env::odbc-environment attr::symbol value)
   (with-access::odbc-environment env ($builtin)
	  ($odbc-sql-set-env-attr $builtin attr value)
	  #unspecified))



