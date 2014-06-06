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
 * - Updated ConsoleMoethods to be TGE 1.2 compliant			*
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


#define _mysql_CPP

#include "fimysql.h"
#include "console/consoleTypes.h"
#include <string.h>

// implement the object
IMPLEMENT_CONOBJECT (MySQL);

//-----------------------------------------------------------------------
//! MySQL
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

MySQL::MySQL()
{
	// defaults
	m_sHost= StringTable->insert ("");
	m_iPort= 0;

	m_Flags.m_bCompress= false;
	m_Flags.m_bSSL= false;

	m_sUserName= StringTable->insert ("");
	m_sUserPwd= StringTable->insert ("");

	m_sDatabase= StringTable->insert ("");


	// mysql
	m_bConnected= false;

	m_Results.m_pNext= 0;
	m_Results.m_pResult= 0;
	m_iLastResultID= S32_MIN;

	if (!mysql_init (&m_Instance))
		Con::errorf ("MySQL: init failed");
}

//-----------------------------------------------------------------------
//! ~MySQL
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

MySQL::~MySQL()
{
	if (m_bConnected)
		Close();

//	if (!mysql_shutdown (&m_Instance, SHUTDOWN_DEFAULT)) //Changed from _shutdown
//		MySQLPrintError();
}

S32 MySQL::AddResult (MYSQL_RES* _pResult)
{
	// free id left?
	if (m_iLastResultID> S32_MAX)
	{
		Con::errorf ("MySQL: No ID left for storing result");

		return 0;
	}

	// go on
	ResultEntry* pCurrent;

	for (pCurrent= &m_Results; pCurrent->m_pNext; pCurrent= pCurrent->m_pNext);

	ResultEntry* pNew= new ResultEntry;

	pNew->m_iID= (++m_iLastResultID) ? m_iLastResultID : ++m_iLastResultID;	// skip 0
	pNew->m_pResult= _pResult;
	pNew->m_pNext= 0;

	pCurrent->m_pNext= pNew;

	return pNew->m_iID;
}

void MySQL::FreeResult (MYSQL_RES* _pResult)
{
	for (ResultEntry* pCurrent= &m_Results; pCurrent->m_pNext; pCurrent= pCurrent->m_pNext)
		if (pCurrent->m_pResult== _pResult)
		{
			ResultEntry* pDel= pCurrent->m_pNext;

			if (pDel)
			{
				mysql_free_result (pCurrent->m_pResult);

				pCurrent->m_pNext= pDel->m_pNext;

				if (pCurrent!= &m_Results)
				{
					pCurrent->m_iID= pDel->m_iID;
					pCurrent->m_pResult= pDel->m_pResult;
					pCurrent->m_Row= pDel->m_Row;
				}

				delete pDel;
			}

			break;
		}
}

void MySQL::FreeAllResults()
{
	while (m_Results.m_pNext)
	{
		ResultEntry* pDel= m_Results.m_pNext;

		m_Results.m_pNext= pDel->m_pNext;

		mysql_free_result (pDel->m_pResult);

		delete pDel;
	}
}

MYSQL_RES* MySQL::GetResult (S32 _iID)
{
	for (ResultEntry* pCurrent= m_Results.m_pNext; pCurrent; pCurrent= pCurrent->m_pNext)
		if (pCurrent->m_iID== _iID)
			return pCurrent->m_pResult;

	return 0;
}

MYSQL_ROW* MySQL::GetRow (S32 _iID)
{
	for (ResultEntry* pCurrent= m_Results.m_pNext; pCurrent; pCurrent= pCurrent->m_pNext)
		if (pCurrent->m_iID== _iID)
			return &pCurrent->m_Row;

	return 0;
}


//-----------------------------------------------------------------------
//! initPersistFields
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

void MySQL::initPersistFields()
{
	Parent::initPersistFields();

	addField ("host", TypeString, Offset (m_sHost, MySQL));
	addField ("port", TypeS32, Offset (m_iPort, MySQL));

	addField ("user", TypeString, Offset (m_sUserName, MySQL));
	addField ("pwd", TypeString, Offset (m_sUserPwd, MySQL));

	addField ("flag_compress", TypeBool, Offset (m_Flags.m_bCompress, MySQL));
	addField ("flag_ssl", TypeBool, Offset (m_Flags.m_bSSL, MySQL));

	addField ("db", TypeString, Offset (m_sDatabase, MySQL));
}

//-----------------------------------------------------------------------
//! bool ValidateSettings()
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

bool MySQL::ValidateSettings()
{
	if (!strlen (m_sUserName))					Con::errorf ("MySQL: username is unset");
	else if (!strlen (m_sDatabase))				Con::errorf ("MySQL: database name is unset");
	else if ((m_iPort< 0) || (m_iPort> 65535))	Con::errorf ("MySQL: port %i is invalid", m_iPort);
	else return true;

	return false;
}

//-----------------------------------------------------------------------
//! Connect
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

bool MySQL::Connect()
{
	int iFlags= 0;

	iFlags|= (m_Flags.m_bCompress) ? CLIENT_COMPRESS : 0;
	iFlags|= (m_Flags.m_bSSL) ? CLIENT_SSL : 0;

	if (!mysql_real_connect (&m_Instance, m_sHost, m_sUserName, m_sUserPwd, m_sDatabase, m_iPort, NULL, iFlags))
	{
		MySQLPrintError();

		return false;
	}

	m_bConnected= true;

	return true;
}

//-----------------------------------------------------------------------
//! Close
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

void MySQL::Close()
{
	FreeAllResults();

	// close
	mysql_close (&m_Instance);

	m_bConnected= false;
}

//-----------------------------------------------------------------------
//! Query
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

bool MySQL::Query (StringTableEntry _sQuery)
{
	if (mysql_query (&m_Instance, _sQuery))
	{
		MySQLPrintError();

		return false;
	}

	return true;
}

//-----------------------------------------------------------------------
//! StoreResult
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

S32 MySQL::StoreResult()
{
	MYSQL_RES* pResult;

	if (!(pResult= mysql_store_result (&m_Instance)))
	{
		MySQLPrintError();

		return 0;
	}

	return AddResult (pResult);
}

//-----------------------------------------------------------------------
//! FreeResult
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

void MySQL::FreeResult (S32 _iResult)
{
	MYSQL_RES* pRes;

	MySQLGetResult (pRes);

	FreeResult (pRes);
}

//-----------------------------------------------------------------------
//! NumRows
/*!
\author Felix Kollmann

\date 2001-mm-dd: init
*/

S32 MySQL::NumRows (S32 _iResult)
{
	MYSQL_RES* pRes;

	MySQLGetResultEx (pRes, -1);

	return mysql_num_rows (pRes);
}

//-----------------------------------------------------------------------
//! FetchRow
/*!
\author Felix Kollmann

\date 2001-mm-dd: init
*/

bool MySQL::FetchRow (S32 _iResult)
{
	MYSQL_RES* pRes;

	MySQLGetResultEx (pRes, 0);

	MYSQL_ROW pRow= mysql_fetch_row (pRes);
	if (!pRow)
	{
		MySQLPrintError();

		return false;
	}

	*GetRow (_iResult)= pRow;

	return true;
}

//-----------------------------------------------------------------------
//! GetRowCell
/*!
\author Felix Kollmann

\date 2001-11-25: init
*/

StringTableEntry MySQL::GetRowCell (S32 _iResult, S32 _iField)
{
	MYSQL_RES* pRes;
	
	MySQLGetResultEx (pRes, 0);

	// check position
	if ((_iField< 0) || (_iField> mysql_num_fields (GetResult (_iResult))))
	{
		Con::errorf ("MySQL: invalid field index: %i", _iField);

		return 0;
	}

	// get it
	return (*GetRow (_iResult))[_iField];
}

//-----------------------------------------------------------------------
//! GetRowCell
/*!
\author Felix Kollmann

\date 2001-12-02: init
*/

StringTableEntry MySQL::GetRowCell (S32 _iResult, StringTableEntry _sFieldName)
{
    MYSQL_RES   *pRes;
    MYSQL_FIELD *pField;

    MySQLGetResultEx (pRes, 0);

    // Go through all of the fields
    for (U32 i = 0; i < pRes->field_count; i++)
        {
        // Get a field
        pField = mysql_fetch_field_direct(pRes, i);
        // Do we have a match
        if (dStrcmp(pField->name, _sFieldName) == 0)
           {
           // Yes we do
           return(GetRowCell(_iResult, i));
           }
        }

    // Let the user know
    Con::errorf ("MySQL: unknown field name: %s", _sFieldName);

    return(0);
}

//-----------------------------------------------------------------------
//! NumFields
/*!
\author Felix Kollmann

\date 2001-12-02: init
*/

S32 MySQL::NumFields()
{
	return mysql_field_count (&m_Instance);
}

//-----------------------------------------------------------------------
//! GetFieldName
/*!
\author Felix Kollmann

\date 2001-12-11: init
*/

StringTableEntry MySQL::GetFieldName (S32 _iResult, S32 _iIndex)
{
	MYSQL_RES* pRes;

	MySQLGetResultEx (pRes, 0);

	if ((_iIndex< 0) || (_iIndex>= NumFields()))
		return 0;

	MYSQL_FIELD* pField= mysql_fetch_field_direct (pRes, _iIndex);

	return (pField) ? pField->name : 0;
}

//-----------------------------------------------------------------------
//! InsertID
/*!
\author Felix Kollmann

\date 2001-12-02: init
*/


S32 MySQL::InsertID()
{
	return mysql_insert_id (&m_Instance);
}

