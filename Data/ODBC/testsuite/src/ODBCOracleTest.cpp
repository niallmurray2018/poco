//
// ODBCOracleTest.cpp
//
// $Id: //poco/Main/Data/ODBC/testsuite/src/ODBCOracleTest.cpp#5 $
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "ODBCOracleTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/String.h"
#include "Poco/Tuple.h"
#include "Poco/Format.h"
#include "Poco/DateTime.h"
#include "Poco/Exception.h"
#include "Poco/Data/Common.h"
#include "Poco/Data/BLOB.h"
#include "Poco/Data/StatementImpl.h"
#include "Poco/Data/ODBC/Connector.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/ODBC/Diagnostics.h"
#include "Poco/Data/ODBC/ODBCException.h"
#include "Poco/Data/ODBC/ODBCStatementImpl.h"
#include <sqltypes.h>
#include <iostream>


using namespace Poco::Data;
using Poco::Data::ODBC::Utility;
using Poco::Data::ODBC::ConnectionException;
using Poco::Data::ODBC::StatementException;
using Poco::Data::ODBC::StatementDiagnostics;
using Poco::format;
using Poco::Tuple;
using Poco::DateTime;
using Poco::NotFoundException;


const bool ODBCOracleTest::bindValues[8] = {true, true, true, false, false, true, false, false};
Poco::SharedPtr<Poco::Data::Session> ODBCOracleTest::_pSession = 0;
Poco::SharedPtr<SQLExecutor> ODBCOracleTest::_pExecutor = 0;
std::string ODBCOracleTest::_dbConnString;
Poco::Data::ODBC::Utility::DriverMap ODBCOracleTest::_drivers;


ODBCOracleTest::ODBCOracleTest(const std::string& name): 
	CppUnit::TestCase(name)
{
}


ODBCOracleTest::~ODBCOracleTest()
{
}


void ODBCOracleTest::testBareboneODBC()
{
	if (!_pSession) fail ("Test not available.");

	std::string tableCreateString = "CREATE TABLE Test "
		"(First VARCHAR(30),"
		"Second VARCHAR(30),"
		"Third BLOB,"
		"Fourth INTEGER,"
		"Fifth NUMBER,"
		"Sixth TIMESTAMP)";

	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_MANUAL);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_BOUND);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_MANUAL);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_BOUND);

	tableCreateString = "CREATE TABLE Test "
		"(First VARCHAR(30),"
		"Second VARCHAR(30),"
		"Third BLOB,"
		"Fourth INTEGER,"
		"Fifth NUMBER,"
		"Sixth DATE)";

	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_MANUAL);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_IMMEDIATE, SQLExecutor::DE_BOUND);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_MANUAL);
	_pExecutor->bareboneODBCTest(_dbConnString, tableCreateString, SQLExecutor::PB_AT_EXEC, SQLExecutor::DE_BOUND);
}


void ODBCOracleTest::testSimpleAccess()
{
	if (!_pSession) fail ("Test not available.");

	std::string tableName("Person");
	int count = 0;

	recreatePersonTable();

	*_pSession << "SELECT count(*) FROM sys.all_all_tables WHERE table_name = upper(?)", into(count), use(tableName), now;
	assert (1 == count);

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccess();
		i += 2;
	}
}


void ODBCOracleTest::testComplexType()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexType();
		i += 2;
	}
}


void ODBCOracleTest::testSimpleAccessVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccessVector();
		i += 2;
	}
}


void ODBCOracleTest::testComplexTypeVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexTypeVector();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertVector();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertEmptyVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertEmptyVector();
		i += 2;
	}	
}


void ODBCOracleTest::testSimpleAccessList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccessList();
		i += 2;
	}
}


void ODBCOracleTest::testComplexTypeList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexTypeList();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertList();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertEmptyList()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertEmptyList();
		i += 2;
	}	
}


void ODBCOracleTest::testSimpleAccessDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->simpleAccessDeque();
		i += 2;
	}
}


void ODBCOracleTest::testComplexTypeDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->complexTypeDeque();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertDeque();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertEmptyDeque()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateStringsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertEmptyDeque();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertSingleBulk()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertSingleBulk();
		i += 2;
	}	
}


void ODBCOracleTest::testInsertSingleBulkVec()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->insertSingleBulkVec();
		i += 2;
	}	
}


void ODBCOracleTest::testLimit()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->limits();
		i += 2;
	}
}


void ODBCOracleTest::testLimitZero()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->limitZero();
		i += 2;
	}	
}


void ODBCOracleTest::testLimitOnce()
{
	if (!_pSession) fail ("Test not available.");

	recreateIntsTable();
	_pExecutor->limitOnce();
	
}


void ODBCOracleTest::testLimitPrepare()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->limitPrepare();
		i += 2;
	}
}



void ODBCOracleTest::testPrepare()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateIntsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->prepare();
		i += 2;
	}
}


void ODBCOracleTest::testSetSimple()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->setSimple();
		i += 2;
	}
}


void ODBCOracleTest::testSetComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->setComplex();
		i += 2;
	}
}


void ODBCOracleTest::testSetComplexUnique()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->setComplexUnique();
		i += 2;
	}
}

void ODBCOracleTest::testMultiSetSimple()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->multiSetSimple();
		i += 2;
	}
}


void ODBCOracleTest::testMultiSetComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->multiSetComplex();
		i += 2;
	}	
}


void ODBCOracleTest::testMapComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->mapComplex();
		i += 2;
	}
}


void ODBCOracleTest::testMapComplexUnique()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->mapComplexUnique();
		i += 2;
	}
}


void ODBCOracleTest::testMultiMapComplex()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->multiMapComplex();
		i += 2;
	}
}


void ODBCOracleTest::testSelectIntoSingle()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->selectIntoSingle();
		i += 2;
	}
}


void ODBCOracleTest::testSelectIntoSingleStep()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->selectIntoSingleStep();
		i += 2;
	}	
}


void ODBCOracleTest::testSelectIntoSingleFail()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->selectIntoSingleFail();
		i += 2;
	}	
}


void ODBCOracleTest::testLowerLimitOk()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->lowerLimitOk();
		i += 2;
	}	
}


void ODBCOracleTest::testSingleSelect()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->singleSelect();
		i += 2;
	}	
}


void ODBCOracleTest::testLowerLimitFail()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->lowerLimitFail();
		i += 2;
	}
}


void ODBCOracleTest::testCombinedLimits()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->combinedLimits();
		i += 2;
	}
}



void ODBCOracleTest::testRange()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->ranges();
		i += 2;
	}
}


void ODBCOracleTest::testCombinedIllegalLimits()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->combinedIllegalLimits();
		i += 2;
	}
}



void ODBCOracleTest::testIllegalRange()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->illegalRange();
		i += 2;
	}
}


void ODBCOracleTest::testEmptyDB()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->emptyDB();
		i += 2;
	}
}


void ODBCOracleTest::testBLOB()
{
	if (!_pSession) fail ("Test not available.");
	
	const std::size_t maxFldSize = 1000000;
	_pSession->setProperty("maxFieldSize", Poco::Any(maxFldSize-1));
	recreatePersonBLOBTable();

	try
	{
		_pExecutor->blob(maxFldSize);
		fail ("must fail");
	}
	catch (DataException&) 
	{
		_pSession->setProperty("maxFieldSize", Poco::Any(maxFldSize));
	}

	for (int i = 0; i < 8;)
	{
		recreatePersonBLOBTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->blob(maxFldSize);
		i += 2;
	}

	recreatePersonBLOBTable();
	try
	{
		_pExecutor->blob(maxFldSize+1);
		fail ("must fail");
	}
	catch (DataException&) { }
}


void ODBCOracleTest::testBLOBStmt()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonBLOBTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->blobStmt();
		i += 2;
	}
}


void ODBCOracleTest::testDateTime()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreatePersonDateTimeTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->dateTime();
		i += 2;
	}
}


void ODBCOracleTest::testFloat()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateFloatsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->floats();
		i += 2;
	}
}


void ODBCOracleTest::testDouble()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateFloatsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->doubles();
		i += 2;
	}
}


void ODBCOracleTest::testTuple()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateTuplesTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->tuples();
		i += 2;
	}
}


void ODBCOracleTest::testTupleVector()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateTuplesTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->tupleVector();
		i += 2;
	}
}


void ODBCOracleTest::testInternalExtraction()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateVectorsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->internalExtraction();
		i += 2;
	}
}


void ODBCOracleTest::testInternalStorageType()
{
	if (!_pSession) fail ("Test not available.");

	for (int i = 0; i < 8;)
	{
		recreateVectorsTable();
		_pSession->setFeature("autoBind", bindValues[i]);
		_pSession->setFeature("autoExtract", bindValues[i+1]);
		_pExecutor->internalStorageType();
		i += 2;
	}
}


void ODBCOracleTest::testStoredProcedure()
{
	if (!_pSession) fail ("Test not available.");

	for (int k = 0; k < 8;)
	{
		_pSession->setFeature("autoBind", bindValues[k]);
		_pSession->setFeature("autoExtract", bindValues[k+1]);

		*_pSession << "CREATE OR REPLACE "
			"PROCEDURE storedProcedure(outParam OUT NUMBER) IS "
			" BEGIN outParam := -1; "
			"END storedProcedure;" , now;

		int i = 0;
		*_pSession << "{call storedProcedure(?)}", out(i), now;
		assert(-1 == i);
		dropObject("PROCEDURE", "storedProcedure");

		*_pSession << "CREATE OR REPLACE "
			"PROCEDURE storedProcedure(inParam IN NUMBER, outParam OUT NUMBER) IS "
			" BEGIN outParam := inParam*inParam; "
			"END storedProcedure;" , now;

		i = 2;
		int j = 0;
		*_pSession << "{call storedProcedure(?, ?)}", in(i), out(j), now;
		assert(4 == j);
		*_pSession << "DROP PROCEDURE storedProcedure;", now;

		*_pSession << "CREATE OR REPLACE "
			"PROCEDURE storedProcedure(ioParam IN OUT NUMBER) IS "
			" BEGIN ioParam := ioParam*ioParam; "
			" END storedProcedure;" , now;

		i = 2;
		*_pSession << "{call storedProcedure(?)}", io(i), now;
		assert(4 == i);
		dropObject("PROCEDURE", "storedProcedure");

		*_pSession << "CREATE OR REPLACE "
			"PROCEDURE storedProcedure(ioParam IN OUT DATE) IS "
			" BEGIN ioParam := ioParam + 1; "
			" END storedProcedure;" , now;

		DateTime dt(1965, 6, 18, 5, 35, 1);
		*_pSession << "{call storedProcedure(?)}", io(dt), now;
		assert(19 == dt.day());
		dropObject("PROCEDURE", "storedProcedure");

		k += 2;
	}

	//std::string automatic binding only
	_pSession->setFeature("autoBind", true);

	*_pSession << "CREATE OR REPLACE "
		"PROCEDURE storedProcedure(inParam IN VARCHAR2, outParam OUT VARCHAR2) IS "
		" BEGIN outParam := inParam; "
		"END storedProcedure;" , now;

	std::string inParam = 
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
	std::string outParam;
	*_pSession << "{call storedProcedure(?,?)}", in(inParam), out(outParam), now;
	assert(inParam == outParam);
	dropObject("PROCEDURE", "storedProcedure");
}


void ODBCOracleTest::testStoredFunction()
{
	if (!_pSession) fail ("Test not available.");

	for (int k = 0; k < 8;)
	{
		_pSession->setFeature("autoBind", bindValues[k]);
		_pSession->setFeature("autoExtract", bindValues[k+1]);

		try{
		*_pSession << "CREATE OR REPLACE "
			"FUNCTION storedFunction RETURN NUMBER IS "
			" BEGIN return(-1); "
			" END storedFunction;" , now;
		}catch(StatementException& se) { std::cout << se.toString() << std::endl; }

		int i = 0;
		*_pSession << "{? = call storedFunction()}", out(i), now;
		assert(-1 == i);
		dropObject("FUNCTION", "storedFunction");

		*_pSession << "CREATE OR REPLACE "
			"FUNCTION storedFunction(inParam IN NUMBER) RETURN NUMBER IS "
			" BEGIN RETURN(inParam*inParam); "
			" END storedFunction;" , now;

		i = 2;
		int result = 0;
		*_pSession << "{? = call storedFunction(?)}", out(result), in(i), now;
		assert(4 == result);
		dropObject("FUNCTION", "storedFunction");

		*_pSession << "CREATE OR REPLACE "
			"FUNCTION storedFunction(inParam IN NUMBER, outParam OUT NUMBER) RETURN NUMBER IS "
			" BEGIN outParam := inParam*inParam; RETURN(outParam); "
			" END storedFunction;" , now;

		i = 2;
		int j = 0;
		result = 0;
		*_pSession << "{? = call storedFunction(?, ?)}", out(result), in(i), out(j), now;
		assert(4 == j);
		assert(j == result); 
		dropObject("FUNCTION", "storedFunction");

		*_pSession << "CREATE OR REPLACE "
			"FUNCTION storedFunction(param1 IN OUT NUMBER, param2 IN OUT NUMBER) RETURN NUMBER IS "
			" temp NUMBER := param1; "
			" BEGIN param1 := param2; param2 := temp; RETURN(param1+param2); "
			" END storedFunction;" , now;

		i = 1;
		j = 2;
		result = 0;
		*_pSession << "{? = call storedFunction(?, ?)}", out(result), io(i), io(j), now;
		assert(1 == j);
		assert(2 == i);
		assert(3 == result); 
		
		Tuple<int, int> params(1, 2);
		assert(1 == params.get<0>());
		assert(2 == params.get<1>());
		result = 0;
		*_pSession << "{? = call storedFunction(?, ?)}", out(result), io(params), now;
		assert(1 == params.get<1>());
		assert(2 == params.get<0>());
		assert(3 == result); 
		dropObject("FUNCTION", "storedFunction");

		k += 2;
	}

	//string and BLOB for automatic binding only
	_pSession->setFeature("autoBind", true);

	*_pSession << "CREATE OR REPLACE "
		"FUNCTION storedFunction(inParam IN VARCHAR2, outParam OUT VARCHAR2) RETURN VARCHAR2 IS "
		" BEGIN outParam := inParam; RETURN outParam;"
		"END storedFunction;" , now;

	std::string inParam = "123";
	std::string outParam;
	std::string ret;
	*_pSession << "{? = call storedFunction(?,?)}", out(ret), in(inParam), out(outParam), now;
	assert("123" == inParam);
	assert(inParam == outParam);
	assert(ret == outParam);
	dropObject("PROCEDURE", "storedFunction");
}


void ODBCOracleTest::dropObject(const std::string& type, const std::string& name)
{
	try
	{
		*_pSession << format("DROP %s %s", type, name), now;
	}
	catch (StatementException& ex)
	{
		bool ignoreError = false;
		const StatementDiagnostics::FieldVec& flds = ex.diagnostics().fields();
		StatementDiagnostics::Iterator it = flds.begin();
		for (; it != flds.end(); ++it)
		{
			if (4043 == it->_nativeError || //ORA-04043 (object does not exist)
				942 == it->_nativeError)//ORA-00942 (table does not exist)
			{
				ignoreError = true;
				break;
			}
		}

		if (!ignoreError) throw;
	}
}


void ODBCOracleTest::recreatePersonTable()
{
	dropObject("TABLE", "Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR2(30), FirstName VARCHAR2(30), Address VARCHAR2(30), Age INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreatePersonTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreatePersonTable()"); }
}


void ODBCOracleTest::recreatePersonBLOBTable()
{
	dropObject("TABLE", "Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Image BLOB)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreatePersonBLOBTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreatePersonBLOBTable()"); }
}


void ODBCOracleTest::recreatePersonDateTimeTable()
{
	dropObject("TABLE", "Person");
	try { *_pSession << "CREATE TABLE Person (LastName VARCHAR(30), FirstName VARCHAR(30), Address VARCHAR(30), Born DATE)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreatePersonDateTimeTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreatePersonDateTimeTable()"); }
}


void ODBCOracleTest::recreateIntsTable()
{
	dropObject("TABLE", "Strings");
	try { *_pSession << "CREATE TABLE Strings (str INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateIntsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateIntsTable()"); }
}


void ODBCOracleTest::recreateStringsTable()
{
	dropObject("TABLE", "Strings");
	try { *_pSession << "CREATE TABLE Strings (str VARCHAR(30))", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateStringsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateStringsTable()"); }
}


void ODBCOracleTest::recreateFloatsTable()
{
	dropObject("TABLE", "Strings");
	try { *_pSession << "CREATE TABLE Strings (str NUMBER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateFloatsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateFloatsTable()"); }
}


void ODBCOracleTest::recreateTuplesTable()
{
	dropObject("TABLE", "Tuples");
	try { *_pSession << "CREATE TABLE Tuples "
		"(int0 INTEGER, int1 INTEGER, int2 INTEGER, int3 INTEGER, int4 INTEGER, int5 INTEGER, int6 INTEGER, "
		"int7 INTEGER, int8 INTEGER, int9 INTEGER, int10 INTEGER, int11 INTEGER, int12 INTEGER, int13 INTEGER,"
		"int14 INTEGER, int15 INTEGER, int16 INTEGER, int17 INTEGER, int18 INTEGER, int19 INTEGER)", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateTuplesTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateTuplesTable()"); }
}


void ODBCOracleTest::recreateVectorsTable()
{
	dropObject("TABLE", "Vectors");
	try { *_pSession << "CREATE TABLE Vectors (int0 INTEGER, flt0 NUMBER, str0 VARCHAR(30))", now; }
	catch(ConnectionException& ce){ std::cout << ce.toString() << std::endl; fail ("recreateVectorsTable()"); }
	catch(StatementException& se){ std::cout << se.toString() << std::endl; fail ("recreateVectorsTable()"); }
}


bool ODBCOracleTest::canConnect(const std::string& driver, const std::string& dsn)
{
	Utility::DriverMap::iterator itDrv = _drivers.begin();
	for (; itDrv != _drivers.end(); ++itDrv)
	{
		if (((itDrv->first).find(driver) != std::string::npos))
		{
			std::cout << "Driver found: " << itDrv->first 
				<< " (" << itDrv->second << ')' << std::endl;
			break;
		}
	}

	if (_drivers.end() == itDrv) 
	{
		std::cout << driver << " driver NOT found, tests not available." << std::endl;
		return false;
	}

	Utility::DSNMap dataSources;
	Utility::dataSources(dataSources);
	Utility::DSNMap::iterator itDSN = dataSources.begin();
	for (; itDSN != dataSources.end(); ++itDSN)
	{
		if (itDSN->first == dsn && itDSN->second == driver)
		{
			std::cout << "DSN found: " << itDSN->first 
				<< " (" << itDSN->second << ')' << std::endl;
			format(_dbConnString, "DSN=%s", dsn);
			return true;
		}
	}

	// DSN not found, try connect without it
	format(_dbConnString, "DRIVER={%s};"
		"UID=Scott;"
		"PWD=Tiger;"
		"TLO=O;"
		"FBS=60000;"
		"FWC=F;"
		"CSR=F;"
		"MDI=Me;"
		"MTS=T;"
		"DPM=F;"
		"NUM=NLS;"
		"BAM=IfAllSuccessful;"
		"BTD=F;"
		"RST=T;"
		"LOB=T;"
		"FDL=10;"
		"FRC=10;"
		"QTO=T;"
		"FEN=T;"
		"XSM=Default;"
		"EXC=F;"
		"APA=T;"
		"DBA=W;"
		"DBQ=XE;"
		"SERVER="
		"(DESCRIPTION="
		" (ADDRESS=(PROTOCOL=TCP)(HOST=localhost)(PORT=1521))"
		" (CONNECT_DATA=(SERVER=DEDICATED)(SERVICE_NAME=XE))"
		");", driver);

	return true;
}


void ODBCOracleTest::setUp()
{
}


void ODBCOracleTest::tearDown()
{
	dropObject("TABLE", "Person");
	dropObject("TABLE", "Strings");
	dropObject("TABLE", "Tuples");
}


bool ODBCOracleTest::init(const std::string& driver, const std::string& dsn)
{
	Utility::drivers(_drivers);
	if (!canConnect(driver, dsn)) return false;
	
	ODBC::Connector::registerConnector();
	try
	{
		_pSession = new Session(ODBC::Connector::KEY, _dbConnString);
	}catch (ConnectionException& ex)
	{
		std::cout << ex.toString() << std::endl;
		return false;
	}

	if (_pSession && _pSession->isConnected()) 
		std::cout << "*** Connected to [" << driver << "] test database." << std::endl;
	
	_pExecutor = new SQLExecutor(driver + " SQL Executor", _pSession);

	return true;
}


CppUnit::Test* ODBCOracleTest::suite()
{
	if (init("Oracle in XE", "PocoDataOracleTest"))
	{
		CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("ODBCOracleTest");

		CppUnit_addTest(pSuite, ODBCOracleTest, testBareboneODBC);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSimpleAccess);
		CppUnit_addTest(pSuite, ODBCOracleTest, testComplexType);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSimpleAccessVector);
		CppUnit_addTest(pSuite, ODBCOracleTest, testComplexTypeVector);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertVector);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertEmptyVector);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSimpleAccessList);
		CppUnit_addTest(pSuite, ODBCOracleTest, testComplexTypeList);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertList);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertEmptyList);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSimpleAccessDeque);
		CppUnit_addTest(pSuite, ODBCOracleTest, testComplexTypeDeque);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertDeque);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertEmptyDeque);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertSingleBulk);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInsertSingleBulkVec);
		CppUnit_addTest(pSuite, ODBCOracleTest, testLimit);
		CppUnit_addTest(pSuite, ODBCOracleTest, testLimitOnce);
		CppUnit_addTest(pSuite, ODBCOracleTest, testLimitPrepare);
		CppUnit_addTest(pSuite, ODBCOracleTest, testLimitZero);
		CppUnit_addTest(pSuite, ODBCOracleTest, testPrepare);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSetSimple);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSetComplex);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSetComplexUnique);
		CppUnit_addTest(pSuite, ODBCOracleTest, testMultiSetSimple);
		CppUnit_addTest(pSuite, ODBCOracleTest, testMultiSetComplex);
		CppUnit_addTest(pSuite, ODBCOracleTest, testMapComplex);
		CppUnit_addTest(pSuite, ODBCOracleTest, testMapComplexUnique);
		CppUnit_addTest(pSuite, ODBCOracleTest, testMultiMapComplex);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSelectIntoSingle);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSelectIntoSingleStep);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSelectIntoSingleFail);
		CppUnit_addTest(pSuite, ODBCOracleTest, testLowerLimitOk);
		CppUnit_addTest(pSuite, ODBCOracleTest, testLowerLimitFail);
		CppUnit_addTest(pSuite, ODBCOracleTest, testCombinedLimits);
		CppUnit_addTest(pSuite, ODBCOracleTest, testCombinedIllegalLimits);
		CppUnit_addTest(pSuite, ODBCOracleTest, testRange);
		CppUnit_addTest(pSuite, ODBCOracleTest, testIllegalRange);
		CppUnit_addTest(pSuite, ODBCOracleTest, testSingleSelect);
		CppUnit_addTest(pSuite, ODBCOracleTest, testEmptyDB);
		CppUnit_addTest(pSuite, ODBCOracleTest, testBLOB);
		CppUnit_addTest(pSuite, ODBCOracleTest, testBLOBStmt);
		CppUnit_addTest(pSuite, ODBCOracleTest, testDateTime);
		CppUnit_addTest(pSuite, ODBCOracleTest, testFloat);
		CppUnit_addTest(pSuite, ODBCOracleTest, testDouble);
		CppUnit_addTest(pSuite, ODBCOracleTest, testTuple);
		CppUnit_addTest(pSuite, ODBCOracleTest, testTupleVector);
		CppUnit_addTest(pSuite, ODBCOracleTest, testStoredProcedure);
		CppUnit_addTest(pSuite, ODBCOracleTest, testStoredFunction);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInternalExtraction);
		CppUnit_addTest(pSuite, ODBCOracleTest, testInternalStorageType);

		return pSuite;
	}

	return 0;
}
