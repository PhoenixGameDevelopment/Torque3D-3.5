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


#ifndef _mysql_H
#define _mysql_H


#include "console/simBase.h"
#ifdef TORQUE_OS_WIN32
#include <winsock2.h>
#endif
#include "mysql.h"


//-----------------------------------------------------------------------
/*! mysql interface for Torque engine
\author Felix Kollmann
\date 2001-11-25
*/
//-----------------------------------------------------------------------

// RJY - Moved these defines from mysql.cc, to me #defines of this type are header material
#define MySQLPrintError() Con::errorf ("MySQL ERROR: '%s'", mysql_error (&m_Instance))

#define MySQLGetResult(var) 	{ var= GetResult (_iResult);	if (!pRes) { Con::errorf ("MySQL: invalid result id: %i", _iResult); return; } }
#define MySQLGetResultEx(var,errorret) 	{ var= GetResult (_iResult);	if (!pRes) { Con::errorf ("MySQL: invalid result id: %i", _iResult); return errorret; } }


class MySQL : public SimObject
{
	// script stuff
	typedef SimObject Parent;

public:

	DECLARE_CONOBJECT (MySQL);

	// some error constants (beffy)
	enum Constants {
	  GlobalError = 0,
	  Success = 1,
	  MissingInputs = 2,
	  UnknownUser = 3,
	  WrongPassword = 4,
	  DatabaseError = 5,
	  DatabaseSettingsError = 6,
	};


protected:

	/// server name
	StringTableEntry m_sHost;

	/// server port
	S32 m_iPort;

	/// username
	StringTableEntry m_sUserName;

	/// user's password
	StringTableEntry m_sUserPwd;

	struct
	{
		/// compress traffic
		bool m_bCompress;

		/// encrypt traffic
		bool m_bSSL;

	} m_Flags;

	/// database
	StringTableEntry m_sDatabase;

	/// mysql
	MYSQL m_Instance;

	/// connected?
	bool m_bConnected;

	/// result list entry
	struct ResultEntry
	{
		// id
		S32 m_iID;

		/// result
		MYSQL_RES* m_pResult;

		/// row
		MYSQL_ROW m_Row;

		/// next entry
		ResultEntry* m_pNext;
	};

	/// first entry
	ResultEntry m_Results;

	/// result id
	S32 m_iLastResultID;

protected:

	S32 AddResult (MYSQL_RES* _pResult);
	void FreeResult (MYSQL_RES* _pResult);
	void FreeAllResults();
	MYSQL_RES* GetResult (S32 _iID);
	MYSQL_ROW* GetRow (S32 _iID);

public:

	//-----------------------------------------------------------------------
	/*! adds class fields
	\author Felix Kollmann
	\date 2001-11-25
	*/
	//-----------------------------------------------------------------------
	static void initPersistFields();

	//-----------------------------------------------------------------------
	/*! checks if the settings are valid
	\author Felix Kollmann
	\date 2001-11-25
	
	\retval true		settings are valid
	\retval true		settings are invalid
	*/
	//-----------------------------------------------------------------------
	bool ValidateSettings();

	//-----------------------------------------------------------------------
	/*! connects to db
	\author Felix Kollmann
	\date 2001-11-25

	\retval true		succeeded
	\retval false		failed
	*/
	//-----------------------------------------------------------------------
	bool Connect();

	//-----------------------------------------------------------------------
	/*! disconnects from db
	\author Felix Kollmann
	\date 2001-11-25
	*/
	//-----------------------------------------------------------------------
	void Close();

	//-----------------------------------------------------------------------
	/*! does a query
	\author Felix Kollmann
	\date 2001-11-25

	\retval true		succeeded
	\retval false		failed
	*/
	//-----------------------------------------------------------------------
	bool Query (StringTableEntry _sQuery);


	//bool doquery (const char *fmt, va_list ap);
	//-----------------------------------------------------------------------
	/*! stores a queried result
	\author Felix Kollmann
	\date 2001-11-25
	
	\return result id
	*/
	//-----------------------------------------------------------------------
	S32 StoreResult();

	//-----------------------------------------------------------------------
	/*! frees a result; optional
	\author Felix Kollmann
	\date 2001-11-25
	\param _iResult			I	result id
	*/
	//-----------------------------------------------------------------------
	void FreeResult (S32 _iResult);

	//-----------------------------------------------------------------------
	/*! returns the number of rows in result
	\author Felix Kollmann
	\date 2001-11-25
	\param _iResult			I	result id
	
	\return row count
	*/
	//-----------------------------------------------------------------------
	S32 NumRows (S32 _iResult);

	//-----------------------------------------------------------------------
	/*! fetch next row
	\author Felix Kollmann
	\date 2001-11-25
	\param _iResult			I	result id
	
	\retval true		successful
	\retval false		failed
	*/
	//-----------------------------------------------------------------------
	bool FetchRow (S32 _iResult);

	//-----------------------------------------------------------------------
	/*! return the value of a cell
	\author Felix Kollmann
	\date 2001-11-25
	\param _iResult			I	result id
	\param _iField			I	field/column to use
	
	\return cell context
	*/
	//-----------------------------------------------------------------------
	StringTableEntry GetRowCell (S32 _iResult, S32 _iField);

	//-----------------------------------------------------------------------
	/*! return the value of a cell
	\author Felix Kollmann
	\date 2001-12-02
	\param _iResult			I	result id
	\param _sFieldName		I	field/column to use
	
	\return cell context
	*/
	//-----------------------------------------------------------------------
	StringTableEntry GetRowCell (S32 _iResult, StringTableEntry _sFieldName);

	//-----------------------------------------------------------------------
	/*! return the column/field count
	\author Felix Kollmann
	\date 2001-12-02
	
	\return field count
	*/
	//-----------------------------------------------------------------------
	S32 NumFields();

	//-----------------------------------------------------------------------
	/*! gets the name of a field
	\author Felix Kollmann
	\date 2001-12-10
	\param _iResult			I	result to use
	\param _iIndex			I	index number
	
	\return field's name
	*/
	//-----------------------------------------------------------------------
	StringTableEntry GetFieldName (S32 _iResult, S32 _iIndex);

	//-----------------------------------------------------------------------
	/*! return the AUTO_INCREMENT flaged field value created by the last INSERT query
	\author Felix Kollmann
	\date 2001-11-02
	
	\return ID
	*/
	//-----------------------------------------------------------------------
	S32 InsertID();

	void setHost(const char *host) {m_sHost= StringTable->insert(host);};
	void setPort(S32 port) {m_iPort = port;};
	void setUser(const char *user) {m_sUserName = StringTable->insert(user,true);};
	void setPass(const char *pass) {m_sUserPwd = StringTable->insert(pass,true);};
	void setFlags(bool compress, bool ssl) { m_Flags.m_bCompress= compress; m_Flags.m_bSSL= ssl;};
	void setDatabase(const char *database) {m_sDatabase = StringTable->insert(database,true);};

public:

	/// constructor
	MySQL();

	/// destructor
	virtual ~MySQL();

};

#endif
