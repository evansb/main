#include "stdafx.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include "filter.h"

namespace You {
namespace QueryEngine {

namespace {
	using day_clock = boost::gregorian::day_clock;
	using weeks = boost::gregorian::weeks;
	using boost::gregorian::to_tm;
}

#pragma region Common Filters
Filter Filter::anyTask() {
	return Filter([] (const Task&) {
		return true;
	});
}

Filter Filter::idIsIn(std::vector<Task::ID> taskIDs) {
	return Filter([taskIDs] (const Task& task) {
		return std::find(taskIDs.begin(), taskIDs.end(),
			task.getID()) != taskIDs.end();
	});
}

Filter Filter::completed() {
	return Filter([] (const Task& task) {
		return task.isCompleted();
	});
}

Filter Filter::dependsOn(const Task::ID id) {
	return Filter([id] (const Task& task) {
		return task.isDependOn(id);
	});
}

Filter Filter::highPriority() {
	return Filter([] (const Task& task) {
		return task.getPriority() == Task::Priority::HIGH;
	});
}

Filter Filter::normalPriority() {
	return Filter([] (const Task& task) {
		return task.getPriority() == Task::Priority::NORMAL;
	});
}

Filter Filter::overdue() {
	return Filter([] (const Task& task) {
		return task.getDeadline().date() < day_clock::local_day();
	});
}

Filter Filter::dueThisYear() {
	return !completed() && Filter([] (const Task& task) {
		return task.getDeadline().date().year()
			== now().year;
	});
}

Filter Filter::dueThisMonth() {
	return dueThisYear() && Filter([] (const Task& task) {
		return task.getDeadline().date().month()
			== now().month;
	});
}

Filter Filter::dueToday() {
	return dueThisMonth() && Filter([] (const Task& task) {
		return task.getDeadline().date().day()
			== now().day;
	});
}

Filter Filter::dueThisWeek() {
	return dueThisMonth() && Filter([] (const Task& task) {
		return (task.getDeadline().date() - day_clock::local_day()) <
			weeks(1);
	});
}

Filter Filter::dueNever() {
	return Filter([] (const Task& task) {
		return task.getDeadline() == Task::NEVER;
	});
}

#pragma endregion

Filter& Filter::operator&&(const Filter& filter) {
	ffilter = AND(ffilter, filter.ffilter);
	return *this;
}

Filter& Filter::operator||(const Filter& filter) {
	ffilter = OR(ffilter, filter.ffilter);
	return *this;
}

Filter& Filter::operator!(void) {
	ffilter = NOT(ffilter);
	return *this;
}

bool Filter::operator()(const Task& task) const {
	return ffilter(task);
}

Filter::FFilter Filter::AND(const FFilter& f, const FFilter& g) {
	return [=] (FFilter::argument_type x) {
		return f(x) && g(x);
	};
}

Filter::FFilter Filter::OR(const FFilter& f, const FFilter& g) {
	return [=] (FFilter::argument_type x) {
		return f(x) || g(x);
	};
}

Filter::FFilter Filter::NOT(const FFilter& f) {
	return [=] (FFilter::argument_type x) {
		return !f(x);
	};
}

Filter::operator FFilter() const {
	return this->ffilter;
}

Filter::Time Filter::now() {
	auto date = day_clock::local_day();
	auto tm = to_tm(date);
	return {
		date.year(),
		date.month(),
		date.day(),
		tm.tm_hour,
		tm.tm_min
	};
}

}  // namespace QueryEngine
}  // namespace You
