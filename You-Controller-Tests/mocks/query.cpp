#include "stdafx.h"
#include "query.h"

namespace You {
namespace Controller {
namespace UnitTests {
namespace Mocks {
namespace Queries {

extern const You::NLP::ADD_QUERY ADD_QUERY {
	L"wow",
	You::NLP::TaskPriority::NORMAL,
	boost::posix_time::ptime(
		boost::gregorian::date(2014, boost::gregorian::Sep, 15)),
	boost::posix_time::ptime(
		boost::gregorian::date(2014, boost::gregorian::Oct, 1))
};

extern const You::NLP::SHOW_QUERY SHOW_QUERY {
	{
		{
			You::NLP::TaskField::DEADLINE,
			You::NLP::SHOW_QUERY::Predicate::GREATER_THAN,
			boost::posix_time::second_clock::local_time() +
				boost::posix_time::hours(1),
		}
	},
	{
		{
			You::NLP::TaskField::DEADLINE,
			You::NLP::SHOW_QUERY::Order::DESCENDING
		}
	}
};

extern const You::NLP::EDIT_QUERY EDIT_QUERY {
	1,
	L"new description",
	You::NLP::TaskPriority::NORMAL,
	boost::none,
	boost::posix_time::ptime(
		boost::gregorian::date(2014, boost::gregorian::Oct, 1),
		boost::posix_time::hours(1) + boost::posix_time::minutes(1)
	),
	true
};

extern const You::NLP::DELETE_QUERY DELETE_QUERY {
	1
};

extern const You::NLP::UNDO_QUERY UNDO_QUERY {
};

}  // namespace Queries
}  // namespace Mocks
}  // namespace UnitTests
}  // namespace Controller
}  // namespace You
