#include "stdafx.h"
#include <boost/format.hpp>

#include "internal/query_parser.h"
#include "exception.h"
#include "../../internal/test_helpers.h"

using Assert = Microsoft::VisualStudio::CppUnitTestFramework::Assert;

namespace You {
namespace NLP {
namespace Internal {
namespace UnitTests {

using You::NLP::Internal::EDIT_QUERY;

TEST_CLASS(EditQueryTests) {
public:
	TEST_METHOD(convertsToStream) {
		{
			std::wostringstream stream;
			stream << DUMMY;
			Assert::AreEqual(
				std::wstring(L"Edit task #1 (Description => the lols)"),
				stream.str());
		}

		{
			EDIT_QUERY local = DUMMY;
			local.fieldsToChange = EDIT_QUERY::FIELDS::DESCRIPTION |
				EDIT_QUERY::FIELDS::DUE;

			std::wostringstream stream;
			stream << local;
			Assert::AreEqual(
				std::wstring(L"Edit task #1 (Description => the lols, "
					L"Due => 1970-Jan-01 00:00:00)"),
				stream.str());
		}
	}

	TEST_METHOD(convertsToString) {
		Assert::AreEqual(
			std::wstring(L"Edit task #1 (Description => the lols)"),
			boost::lexical_cast<std::wstring>(DUMMY));
	}

	TEST_METHOD(comparesEquality) {
		EDIT_QUERY local = DUMMY;

		Assert::AreEqual(DUMMY, local);
	}

	TEST_METHOD(comparesInequality) {
		EDIT_QUERY local {
			1,
			EDIT_QUERY::FIELDS::DESCRIPTION,
			L""
		};

		Assert::AreNotEqual(DUMMY, local);
	}

private:
	/// A dummy object.
	static const EDIT_QUERY DUMMY;
};

const EDIT_QUERY EditQueryTests::DUMMY {
	1,
	EDIT_QUERY::FIELDS::DESCRIPTION,
	L"the lols",
	boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1),
		boost::posix_time::hours(0))
};

}  // namespace UnitTests
}  // namespace Internal
}  // namespace NLP
}  // namespace You
