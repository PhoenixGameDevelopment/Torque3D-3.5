/****************************************************************/
/*              - Copyright Future Interactive -                */
/****************************************************************/
/*   This project is distributed under the terms of the FISL.   */
/****************************************************************/
/*              www:      http://www.futureint.de               */
/*            email:      devel@futureint.de                    */
/****************************************************************/

/****************************************************************
 *				Updated By Ronald J. Yacketta 03/02/2004        *
 ****************************************************************
 * - Updated COnsoleMoethods to be TGE 1.2 compliant			*
 * - Added ConsoleMethod (setupEnv) which takes all required	*
 *   paremeters to make a MySQL DB connection					*
 * - #ifdef'ed the winsock.h header as a first pass at cross	*
 *   platform compliancy										*
 ****************************************************************/

/****************************************************************
 *				Updated By Henry Garle 19/12/2009			    *
 *					  HenryGarle@gmail.com						*
 ****************************************************************
 * - Updated all outdated syntax errors							*
 * - Updated to work with newer version of libmySQL				*
 * - Works with all current versions of torque					*
 *     TGE 1.5.2												*
 *     TGEA 1.8.2												*
 *     Torque3D 1.1 Alpha										*
 ****************************************************************/

#define _mysql_cs_CPP

#include "fimysql.h"

ConsoleMethod (MySQL, ValidateSettings, bool, 2, 2, "MySQL.ValidateSettings()")
{
	return object->ValidateSettings();
}

ConsoleMethod (MySQL, Connect, bool, 2, 2, "MySQL.Connect()")
{
	return object->Connect();
}

ConsoleMethod (MySQL, Close, void, 2, 2, "MySQL.Close()")
{
	object->Close();
}

ConsoleMethod (MySQL, Query, bool, 3, 3, "MySQL.Query (QueryStringValue)")
{
	return object->Query (argv[2]);
}

ConsoleMethod (MySQL, doquery, bool, 3, 3, "MySQL.doquery (QueryStringValue)")
{
	return object->Query (argv[2]);
}

ConsoleMethod (MySQL, StoreResult, S32, 2, 2, "MySQL.StoreResult()")
{
	return object->StoreResult();
}

ConsoleMethod (MySQL, FreeResult, void, 3, 3, "MySQL.FreeResult (ResultID)")
{
	object->FreeResult (atoi (argv[2]));
}

ConsoleMethod (MySQL, NumRows, S32, 3, 3, "MySQL.NumRows (ResultID)")
{
	return object->NumRows (atoi (argv[2]));
}

ConsoleMethod (MySQL, FetchRow, bool, 3, 3, "MySQL.FetchRow (ResultID)")
{
	return object->FetchRow (atoi (argv[2]));
}

ConsoleMethod (MySQL, GetRowCellDirect, const char *, 4, 4, "MySQL.GetRowCellDirect (ResultID, FieldNr)")
{
	return object->GetRowCell (atoi (argv[2]), atoi (argv[3]));
}

ConsoleMethod (MySQL, GetRowCell, const char *, 4, 4, "MySQL.GetRowCell (ResultID, FieldName)")
{
	return object->GetRowCell (atoi (argv[2]), argv[3]);
}

ConsoleMethod (MySQL, NumFields, S32, 2, 2, "MySQL.NumFields()")
{
	return object->NumFields();
}

ConsoleMethod (MySQL, GetFieldName, const char *, 4, 4, "MySQL.GetFieldName (ResultID, FieldIndex)")
{
	return object->GetFieldName (atoi (argv[2]), atoi (argv[3]));
}

ConsoleMethod (MySQL, InsertID, S32, 2, 2, "MySQL.InsertID()")
{
	return object->InsertID();
}

// RJY - Added this ConsoleMethod to make it easier to setup the MySQL variables to make a
// MySQL DB connection
ConsoleMethod( MySQL, setupEnv, void, 9, 9, "MySQL.setupEnv(DB,hostname,port,username,password,compression,ssl)")
{
	// set the DB name
	object->setDatabase(argv[2]);
	// set the DB hostname and or ip
	object->setHost(argv[3]);
	// set the DB Port
	object->setPort(atoi(argv[4]));
	//set the DB username to use
	object->setUser(argv[5]);
	// set the DB password to use
	object->setPass(argv[6]);
	// set the compression and ssl flags (***UNTESTED***)
	object->setFlags(argv[7],argv[8]);
}

