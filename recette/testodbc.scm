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
(module testodbc
   (library biglooODBC)
   (main main))


(define (main args)
   (let ((db (instantiate::odbc-database
		;(connection-string "DSN=testdb")
		(connection-string "DRIVER={PostgreSQL Unicode};DATABASE=testdb;SERVERNAME=localhost;PORT=5432;Uid=jwd;PWD=d0d0byrd")
		)))

      (database-close db)))