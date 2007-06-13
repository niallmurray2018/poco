//
// AbstractPrepare.h
//
// $Id: //poco/Main/Data/include/Poco/Data/AbstractPrepare.h#4 $
//
// Library: Data
// Package: DataCore
// Module:  AbstractPrepare
//
// Definition of the AbstractPrepare class.
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


#ifndef Data_AbstractPrepare_INCLUDED
#define Data_AbstractPrepare_INCLUDED


#include "Poco/Data/Data.h"
#include <cstddef>


namespace Poco {
namespace Data {


class AbstractPreparation;


class Data_API AbstractPrepare
	/// Interface for calling the appropriate AbstractPreparation method
{
public:
	AbstractPrepare(AbstractPreparation* pPrepare);
		/// Creates the AbstractPrepare.

	virtual ~AbstractPrepare();
		/// Destroys the AbstractPrepare.

	virtual void prepare() = 0;
		/// Prepares data.

protected:
	AbstractPreparation* preparation();
		/// Returns the preparation object

	AbstractPreparation* _pPrepare;
};


//
// inlines
//
inline AbstractPreparation* AbstractPrepare::preparation()
{
	return _pPrepare;
}


} } // namespace Poco::Data


#endif // Data_AbstractPrepare_INCLUDED
