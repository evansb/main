//@author A0097630B
#include "stdafx.h"
#include "exceptions/parse_error_exception.h"
#include "query_parser.h"

namespace You {
namespace NLP {

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace phoenix = boost::phoenix;

QUERY QueryParser::parse(const QueryParser::StringType& string) {
	QUERY result;
	bool success = qi::phrase_parse(
		begin(string),
		end(string),
		QueryParser(),
		ParserSkipperType(),
		result);

	if (success) {
		return result;
	} else {
		throw ParserException();
	}
}

QueryParser::QueryParser() : QueryParser::base_type(start) {
	start %= (
		(qi::lit(L'/') > explicitCommand) |
		addCommand
	) >> qi::eoi;
	start.name("start");

	explicitCommand %= (
		(qi::lit(L"add") >> addCommand) |
		(qi::lit(L"show") >> showCommand) |
		(qi::lit(L"edit") >> editCommand) |
		(qi::lit(L"delete") >> deleteCommand)
	);
	explicitCommand.name("explicitCommand");

	#pragma region Adding tasks
	addCommand = (
		ParserCharTraits::char_ >> addCommandDescription
	)[qi::_val = phoenix::bind(&constructAddQuery, qi::_1, qi::_2)];
	addCommand.name("addCommand");

	addCommandDescription = (
		ParserCharTraits::char_ >> addCommandDescriptionTail
	)[qi::_val = phoenix::bind(&constructAddQuery, qi::_1, qi::_2)];
	addCommandDescription.name("addCommandDescription");

	addCommandDescriptionTail %= (
		(qi::omit[*ParserCharTraits::blank] >> addCommandPriority) |
		addCommandDescription
	);
	addCommandDescriptionTail.name("addCommandDescriptionTail");

	addCommandPriority %= qi::skip(ParserCharTraits::blank)[(
		(
			qi::lit('!') >> addCommandDeadlineOptional)
		[qi::_val = phoenix::bind(&constructAddQueryWithPriority, qi::_1)] |
		addCommandDeadlineOptional
	)];
	addCommandPriority.name("addCommandPriority");

	addCommandDeadline = (
		(qi::lit("by") | qi::lit("before")) >>
		qi::lexeme[+ParserCharTraits::char_]
	)[qi::_val = phoenix::bind(
		&QueryParser::constructAddQueryWithDeadline, qi::_1)];
	addCommandDeadline.name("addCommandDeadline");

	addCommandDeadlineOptional = (
		addCommandDeadline || qi::eoi
	)[qi::_val = phoenix::bind(&constructAddQueryWithOptionalDeadline, qi::_1)];

	#pragma endregion

	#pragma region Showing tasks
	showCommand = (
		-showCommandFiltering >>
		(qi::lit(L"sorted by") | qi::lit(L"order by") | qi::lit(L"sort")) >>
		showCommandSorting
	)[qi::_val = phoenix::bind(&constructShowQuery, qi::_1, qi::_2)];

	showCommandSorting %= (
		showCommandSortingColumn % (qi::lit(L",") | qi::lit(L"then"))
	);

	showCommandSortingColumn = (
		showCommandFields >> -showCommandSortingOrders
	)[qi::_val = phoenix::bind(&constructShowQuerySortColumn, qi::_1, qi::_2)];

	showCommandSortingOrders.add
		(L"asc", SHOW_QUERY::Order::ASCENDING)
		(L"ascending", SHOW_QUERY::Order::ASCENDING)
		(L"desc", SHOW_QUERY::Order::DESCENDING)
		(L"descending", SHOW_QUERY::Order::DESCENDING);
	showCommandSortingOrders.name("showCommandSortingOrders");

	showCommandFields.add
		(L"description", TaskField::DESCRIPTION)
		(L"deadline", TaskField::DEADLINE)
		(L"done", TaskField::COMPLETE)
		(L"complete", TaskField::COMPLETE);
	showCommandFields.name("showCommandFields");
	#pragma endregion

	#pragma region Editing tasks
	editCommand = (
		qi::uint_ >> qi::lit(L"set") >> editCommandRule
	)[qi::_val = phoenix::bind(&QueryParser::constructEditQuery,
		qi::_1, qi::_2)];
	editCommand.name("editCommand");

	editCommandRule %= (
		editCommandRuleNullary |
		editCommandRuleUnary |
		editCommandRulePriorities
	);
	editCommandRule.name("editCommandRule");

	editCommandRuleNullary = (
		editCommandFieldsNullary
	)[qi::_val = phoenix::bind(&constructEditQueryNullary, qi::_1)];
	editCommandRuleNullary.name("editCommandRuleNullary");

	editCommandRuleUnary = (
		editCommandFieldsUnary >> qi::lexeme[*ParserCharTraits::char_])
	[qi::_val = phoenix::bind(&constructEditQueryUnary, qi::_1, qi::_2)];
	editCommandRuleUnary.name("editCommandRuleUnary");

	editCommandRulePriorities = (
		qi::lit(L"priority") >> editCommandFieldPriorities
	)[qi::_val = phoenix::bind(&constructEditQueryPriority, qi::_1)];
	editCommandRulePriorities.name("editCommandRulePriorities");

	editCommandFieldsUnary.add
		(L"description", TaskField::DESCRIPTION)
		(L"deadline", TaskField::DEADLINE);
	editCommandFieldsUnary.name("editCommandFieldsUnary");

	editCommandFieldsNullary.add
		(L"done", TaskField::COMPLETE)
		(L"complete", TaskField::COMPLETE);
	editCommandFieldsNullary.name("editCommandFieldsNullary");

	editCommandFieldPriorities.add
		(L"normal", TaskPriority::NORMAL)
		(L"high", TaskPriority::HIGH);
	editCommandFieldPriorities.name("editCommandFieldPriorities");
	#pragma endregion

	#pragma region Deleting tasks
	deleteCommand = (
		qi::uint_
	)[qi::_val = phoenix::bind(&QueryParser::constructDeleteQuery, qi::_1)];
	deleteCommand.name("deleteCommand");
	#pragma endregion

	qi::on_error<qi::fail>(start,
		phoenix::bind(&QueryParser::onFailure, qi::_1, qi::_2, qi::_3, qi::_4));
}

void QueryParser::onFailure(
	ParserIteratorType /*begin*/,
	ParserIteratorType end,
	ParserIteratorType errorPos,
	const spirit::info& message) {
	StringType lexeme(errorPos, end);
	throw ParseErrorException(message, lexeme);
}

}  // namespace NLP
}  // namespace You
