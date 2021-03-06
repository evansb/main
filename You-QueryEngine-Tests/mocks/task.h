//@author A0112054Y
#pragma once
#ifndef YOU_QUERYENGINE_TESTS_MOCKS_TASK_H_
#define YOU_QUERYENGINE_TESTS_MOCKS_TASK_H_

#include "../You-QueryEngine/task_model.h"

namespace You {
namespace QueryEngine {
namespace UnitTests {

/// \name Various task examples for testing
/// @{
	/// Has specific ID
	extern Task TASK_WITH_ID_1();
	extern Task TASK_WITH_ID_2();
	extern Task TASK_WITH_ID_3();

	/// Has description.
	extern Task FEED_THE_DOGGY();
	extern Task FEED_THE_KITTEN();

	/// Has priority
	extern Task SUPER_IMPORTANT_TASK();
	extern Task LESS_IMPORTANT_TASK();

	/// Has due dates.
	extern Task FOR_TOMMOROW();
	extern Task FOR_NEXT_WEEK();

	/// Has dependencies.
	extern Task DEPENDS_ON_1_2_3();
	extern Task DEPENDS_ON_3_4_5();
	extern Task DEPENDS_ON_NOTHING();
	extern Task TEN_DEPENDENCIES();
	extern Task NINE_DEPENDENCIES();

	/// Due on various time point
	extern Task OVERDUE();
	extern Task DUE_THIS_MONTH();
	extern Task DUE_THIS_WEEK();
	extern Task DUE_TODAY();
	extern Task DUE_ON_CHRISTMAS();
	extern Task DUE_BEFORE_CHRISTMAS();
	extern Task DUE_AFTER_CHRISTMAS();

	/// Subtasks
	extern Task ONE_SUBTASK();
	extern Task TWO_SUBTASKS();
	extern Task CIRCULAR_SUBTASKS();


	/// Related to
	extern Task RELATED_TO_1();
	extern Task RELATED_TO_2();
	extern Task RELATED_TO_3();

	/// Descendant
	extern Task GRANDMOTHER();
	extern Task MOTHER();
	extern Task CHILD();

/// @}

}  // namespace UnitTests
}  // namespace QueryEngine
}  // namespace You

#endif  // YOU_QUERYENGINE_TESTS_MOCKS_TASK_H_
