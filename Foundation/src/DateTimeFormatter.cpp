//
// DateTimeFormatter.cpp
//
// $Id: //poco/Main/Foundation/src/DateTimeFormatter.cpp#14 $
//
// Library: Foundation
// Package: DateTime
// Module:  DateTimeFormatter
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
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


#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Timestamp.h"
#include "Poco/NumberFormatter.h"


namespace Poco {


void DateTimeFormatter::append(std::string& str, const LocalDateTime& dateTime, const std::string& fmt)
{
	DateTimeFormatter::append(str, dateTime.utc(), fmt, dateTime.tzd());
}


void DateTimeFormatter::append(std::string& str, const DateTime& dateTime, const std::string& fmt, int timeZoneDifferential)
{
	std::string::const_iterator it  = fmt.begin();
	std::string::const_iterator end = fmt.end();
	while (it != end)
	{
		if (*it == '%')
		{
			if (++it != end)
			{
				switch (*it)
				{
				case 'w': str.append(DateTimeFormat::WEEKDAY_NAMES[dateTime.dayOfWeek()], 0, 3); break;
				case 'W': str.append(DateTimeFormat::WEEKDAY_NAMES[dateTime.dayOfWeek()]); break;
				case 'b': str.append(DateTimeFormat::MONTH_NAMES[dateTime.month() - 1], 0, 3); break;
				case 'B': str.append(DateTimeFormat::MONTH_NAMES[dateTime.month() - 1]); break;
				case 'd': NumberFormatter::append0(str, dateTime.day(), 2); break;
				case 'e': NumberFormatter::append(str, dateTime.day()); break;
				case 'f': NumberFormatter::append(str, dateTime.day(), 2); break;
				case 'm': NumberFormatter::append0(str, dateTime.month(), 2); break;
				case 'n': NumberFormatter::append(str, dateTime.month()); break;
				case 'o': NumberFormatter::append(str, dateTime.month(), 2); break;
				case 'y': NumberFormatter::append0(str, dateTime.year() % 100, 2); break;
				case 'Y': NumberFormatter::append0(str, dateTime.year(), 4); break;
				case 'H': NumberFormatter::append0(str, dateTime.hour(), 2); break;
				case 'h': NumberFormatter::append0(str, dateTime.hourAMPM(), 2); break;
				case 'a': str.append(dateTime.isAM() ? "am" : "pm"); break;
                                case 'A': str.append(dateTime.isAM() ? "AM" : "PM"); break;
                                case 'M': NumberFormatter::append0(str, dateTime.minute(), 2); break;
                                case 'S': NumberFormatter::append0(str, dateTime.second(), 2); break;
                                case 's': NumberFormatter::append0(str, dateTime.second(), 2); 
                                          str += '.'; 
                                          NumberFormatter::append0(str, dateTime.millisecond()*1000 + dateTime.microsecond(), 6); 
                                          break;
                                case 'i': NumberFormatter::append0(str, dateTime.millisecond(), 3); break;
                                case 'c': NumberFormatter::append(str, dateTime.millisecond()/100); break;
                                case 'F': NumberFormatter::append0(str, dateTime.millisecond()*1000 + dateTime.microsecond(), 6); break;
				case 'z': tzdISO(str, timeZoneDifferential); break;
				case 'Z': tzdRFC(str, timeZoneDifferential); break;
				default:  str += *it;
				}
				++it;
			}
		}
		else str += *it++;
	}
}


void DateTimeFormatter::append(std::string& str, const Timespan& timespan, const std::string& fmt)
{
	std::string::const_iterator it  = fmt.begin();
	std::string::const_iterator end = fmt.end();
	while (it != end)
	{
		if (*it == '%')
		{
			if (++it != end)
			{
				switch (*it)
				{
				case 'd': NumberFormatter::append(str, timespan.days()); break;
				case 'H': NumberFormatter::append0(str, timespan.hours(), 2); break;
				case 'h': NumberFormatter::append(str, timespan.totalHours()); break;
				case 'M': NumberFormatter::append0(str, timespan.minutes(), 2); break;
				case 'm': NumberFormatter::append(str, timespan.totalMinutes()); break;
				case 'S': NumberFormatter::append0(str, timespan.seconds(), 2); break;
				case 's': NumberFormatter::append(str, timespan.totalSeconds()); break;
				case 'i': NumberFormatter::append0(str, timespan.milliseconds(), 3); break;
				case 'c': NumberFormatter::append(str, timespan.milliseconds()/100); break;
				case 'F': NumberFormatter::append0(str, timespan.milliseconds()*1000 + timespan.microseconds(), 6); break;
				default:  str += *it;
				}
				++it;
			}
		}
		else str += *it++;
	}
}


void DateTimeFormatter::tzdISO(std::string& str, int timeZoneDifferential)
{
	if (timeZoneDifferential != UTC)
	{
		if (timeZoneDifferential >= 0)
		{
			str += '+';
			NumberFormatter::append0(str, timeZoneDifferential/3600, 2);
			str += ':';
			NumberFormatter::append0(str, (timeZoneDifferential%3600)/60, 2);
		}
		else
		{
			str += '-';
			NumberFormatter::append0(str, -timeZoneDifferential/3600, 2);
			str += ':';
			NumberFormatter::append0(str, (-timeZoneDifferential%3600)/60, 2);
		}
	}
	else str += 'Z';
}


void DateTimeFormatter::tzdRFC(std::string& str, int timeZoneDifferential)
{
	if (timeZoneDifferential != UTC)
	{
		if (timeZoneDifferential >= 0)
		{
			str += '+';
			NumberFormatter::append0(str, timeZoneDifferential/3600, 2);
			NumberFormatter::append0(str, (timeZoneDifferential%3600)/60, 2);
		}
		else
		{
			str += '-';
			NumberFormatter::append0(str, -timeZoneDifferential/3600, 2);
			NumberFormatter::append0(str, (-timeZoneDifferential%3600)/60, 2);
		}		
	}
	else str += "GMT";
}


} // namespace Poco
