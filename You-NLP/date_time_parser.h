//@author A0097630B
#pragma once
#ifndef YOU_NLP_DATE_TIME_PARSER_H_
#define YOU_NLP_DATE_TIME_PARSER_H_

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include "parser.h"

namespace You {
namespace NLP {
namespace UnitTests { class DateTimeParserTests; }

/// The query parser that recognises our input syntax.
class DateTimeParser : public boost::spirit::qi::grammar<
	ParserIteratorType,
	boost::posix_time::ptime()> {
public:
	friend class UnitTests::DateTimeParserTests;

	/// The type of the iterator used in this grammar.
	typedef ParserIteratorType IteratorType;

	/// The type of the skipper used in this grammar.
	typedef ParserSkipperType SkipperType;

	/// The type of the lexeme buffer provided in a lexing semantic action.
	typedef std::vector<ParserCharEncoding::char_type> LexemeType;

	/// The type of input strings accepted by this parser.
	typedef std::basic_string<ParserCharEncoding::char_type> StringType;

public:
	DateTimeParser();

	/// Parses the given input string into a date.
	///
	/// \param[in] string The string to parse.
	/// \param[out] result The parsed date/time object.
	/// \return True if parsing was successful.
	static bool parse(
		const StringType& string,
		boost::posix_time::ptime& result);

	/// Parses the given input string into a date.
	///
	/// \param[in] string The string to parse.
	/// \return The date/time representing the string.
	/// \exception ParserException When parsing fails.
	static boost::posix_time::ptime parse(const StringType& string);

private:
	typedef int16_t Year;
	typedef int16_t Month;
	typedef int8_t Day;
	typedef boost::gregorian::date Date;
	typedef boost::posix_time::ptime DateTime;

	typedef boost::spirit::qi::rule<
		IteratorType,
		Date()> DateRule;

private:
	/// Parses a date into its boost::posix_time form.
	static DateTime buildDateTime(Date);

	/// Parses a partial date into its boost::date_time form.
	///
	/// The prototype is nasty because these are the kinds of dates that we
	/// support.
	static Date buildDate(Year, Month, Day);

	/// Parses the given year string into a full year. If this has only 2 digits
	/// then it will be treated as fuzzy; otherwise it will be a full year. This
	/// conversion will happen unless the year string is prefixed by a zero.
	///
	/// \param[in] chars The character string, containing ASCII 0-9.
	static Year parseFuzzyYear(
		const std::vector<ParserCharEncoding::char_type>& chars);

	/// Parses the given two-digit year into a full year according to system
	/// settings (on Windows) or using strptime conversion rules.
	///
	/// \param[in] year The year to convert. This must be between 0 and 99
	///                 inclusive.
	/// \return The actual year referenced.
	static Year parseTwoDigitYear(Year year);

	/// Constructs a date from the given day and month. This is the next day and
	/// month that will occur. If the month is before the current month, it will
	/// be the following year.
	///
	/// \return The date specified.
	static Date constructDayMonthDate(Day, Month);

	/// Constructs a relative date from a month.
	///
	/// \param[in] direction -1 for 'last', 0 for 'this', 1 for 'next'
	/// \param[in] month The month specified
	///
	/// \return The date specified.
	static Date constructRelativeMonthDate(int direction,
		boost::date_time::months_of_year month);

	/// Constructs a relative date from a week day.
	///
	/// \param[in] direction -1 for 'last', 0 for 'this', 1 for 'next'
	/// \param[in] month The day name specified
	///
	/// \return The date specified, relative to today.
	static Date constructRelativeWeekDayDate(int direction,
		boost::date_time::weekdays day);

	/// Constructs a relative date from today.
	///
	/// \param[in] daysFromToday The date to construct from today.
	///
	/// \return The date specified, relative to today.
	static Date constructRelativeDate(int daysFromToday);

	/// Handles failures in parsing. This raises a \ref ParseErrorException.
	///
	/// \exception ParseErrorException The exception representing the parse
	///                                error.
	static void onFailure(
		ParserIteratorType begin,
		ParserIteratorType end,
		ParserIteratorType errorPos,
		const boost::spirit::info& message);

private:
	/// The start rule.
	start_type start;

	/// Parsing dates.
	DateRule date;

	/// Rules to parse the various kinds of dates.
	/// @{
	DateRule dateYearMonthDay;
	DateRule dateYearMonth;
	DateRule dateYear;
	DateRule dateMonthYear;
	DateRule dateDayMonth;
	/// @}

	/// Rules to parse relative dates
	/// @{
	DateRule relativeDate;
	boost::spirit::qi::rule<
		IteratorType,
		Date(int)> relativeDateInDirection;  // NOLINT(readability/function)
	DateRule relativeDateInDays;
	/// @}

	/// Parsing years.
	boost::spirit::qi::rule<IteratorType, Year()> year;

	/// Parsing months.
	boost::spirit::qi::rule<IteratorType, Month()> month;

	/// List of months.
	boost::spirit::qi::symbols<
		ParserCharEncoding::char_type,
		boost::gregorian::months_of_year> monthNames;

	/// List of day names
	boost::spirit::qi::symbols<
		ParserCharEncoding::char_type,
		boost::date_time::weekdays> weekDays;

	/// Parsing days
	boost::spirit::qi::rule<IteratorType, Day()> day;

	/// A utility rule to handle whitespace.
	boost::spirit::qi::rule<IteratorType> space;
};

}  // namespace NLP
}  // namespace You

#endif  // YOU_NLP_DATE_TIME_PARSER_H_
