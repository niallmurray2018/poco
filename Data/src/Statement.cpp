//
// Statement.cpp
//
// $Id: //poco/Main/Data/src/Statement.cpp#11 $
//
// Library: Data
// Package: DataCore
// Module:  Statement
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


#include "Poco/Data/Statement.h"
#include "Poco/Data/DataException.h"
#include "Poco/Data/Extraction.h"
#include "Poco/Data/Session.h"
#include "Poco/Any.h"
#include "Poco/Tuple.h"
#include <algorithm>


namespace Poco {
namespace Data {


Statement::Statement(StatementImpl* pImpl):
	_executed(false),
	_ptr(pImpl)
{
	poco_check_ptr (pImpl);
}


Statement::Statement(Session& session):
	_executed(false)
{
	reset(session);
}


Statement::Statement(const Statement& stmt):
	_executed(stmt._executed),
	_ptr(stmt._ptr)
{
}


Statement::~Statement()
{
}


Statement& Statement::operator = (const Statement& stmt)
{
	Statement tmp(stmt);
	swap(tmp);
	return *this;
}


void Statement::swap(Statement& other)
{
	std::swap(_ptr, other._ptr);
	std::swap(_executed, other._executed);
}


Poco::UInt32 Statement::execute()
{
	if (done())
	{
		_ptr->reset();
	}
	_executed = true;
	return _ptr->execute();
}


bool Statement::done()
{
	return _ptr->getState() == StatementImpl::ST_DONE;
}


bool Statement::canModifyStorage()
{
	return 0 == extractionCount() &&
		(_ptr->getState() == StatementImpl::ST_INITIALIZED ||
		_ptr->getState() == StatementImpl::ST_RESET);
}


Statement& Statement::reset(Session& session)
{
	Statement stmt(session.createStatementImpl());
	swap(stmt);
	return *this;
}


const std::string& Statement::getStorage() const
{
	switch (storage())
	{
	case STORAGE_VECTOR:
		return StatementImpl::VECTOR;
	case STORAGE_LIST:
		return StatementImpl::LIST;
	case STORAGE_DEQUE:
		return StatementImpl::DEQUE;
	case STORAGE_UNKNOWN:
		return StatementImpl::UNKNOWN;
	}

	throw IllegalStateException("Invalid storage setting.");
}


void now(Statement& statement)
{
	statement.execute();
}


void vector(Statement& statement)
{
	if (!statement.canModifyStorage())
		throw InvalidAccessException("Storage not modifiable.");

	statement.setStorage("vector");
}


void list(Statement& statement)
{
	if (!statement.canModifyStorage())
		throw InvalidAccessException("Storage not modifiable.");

	statement.setStorage("list");
}


void deque(Statement& statement)
{
	if (!statement.canModifyStorage())
		throw InvalidAccessException("Storage not modifiable.");

	statement.setStorage("deque");
}


} } // namespace Poco::Data
