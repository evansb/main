//@author A0112054Y
#include "stdafx.h"
#include "CppUnitTest.h"

#include "internal/controller/task_builder.h"
#include "internal/controller/task_serializer.h"
#include "exception.h"

#include "common.h"

using Assert = Microsoft::VisualStudio::CppUnitTestFramework::Assert;

namespace You {
namespace QueryEngine {
namespace UnitTests {

/// \cond Imports
namespace {
	using boost::posix_time::ptime;
	using boost::posix_time::time_duration;
	using boost::gregorian::date;
	using boost::gregorian::greg_month;

	using Task = You::QueryEngine::Task;
	using TaskBuilder = You::QueryEngine::Internal::TaskBuilder;
	using TaskSerializer = You::QueryEngine::Internal::TaskSerializer;
}
/// \endcond

/// Test the functionality of TaskSerializer
TEST_CLASS(TaskSerializerTests) {
public:
	static Task getMockTask() {
		Task::Description desc = L"Learn Haskell Lens";
		Task::Time start = ptime(date(2001, 1, 10),
			time_duration(1, 2, 3));
		Task::Time dead = ptime(date(2002, 1, 10),
			time_duration(1, 2, 3));
		Task::Dependencies dep = { 1, 2, 3 };
		Task::Priority prio = Task::Priority::HIGH;
		Task::ID parent = 43L;
		return TaskBuilder::get()
			.description(desc)
			.startTime(start)
			.deadline(dead)
			.priority(prio)
			.dependencies(dep)
			.parent(parent)
			.subtasks(dep)
			.completed(true);
	}

	TEST_METHOD(taskSerializerIsUtilityClass) {
		static_assert(
		!std::is_trivially_default_constructible<TaskSerializer>::value,
		"QueryEngine cannot be default constructed");
	}

	/// Should be able to serialize a complex task.
	TEST_METHOD(serializeTask) {
		Task task = getMockTask();
		auto serialized = TaskSerializer::serialize(task);
		Assert::AreEqual(serialized[TaskSerializer::KEY_DESCRIPTION],
			task.getDescription());
		Assert::AreEqual(serialized[TaskSerializer::KEY_START_TIME],
			std::wstring(L"2001;1;10;1;2;3;"));
		Assert::AreEqual(serialized[TaskSerializer::KEY_DEADLINE],
			std::wstring(L"2002;1;10;1;2;3;"));
		Assert::AreEqual(serialized[TaskSerializer::KEY_PRIORITY],
			std::wstring(L"high"));
		Assert::AreEqual(serialized[TaskSerializer::KEY_DEPENDENCIES],
			std::wstring(L"1;2;3;"));
		Assert::AreEqual(serialized[TaskSerializer::KEY_COMPLETED],
			std::wstring(L"true"));
		Assert::AreEqual(serialized[TaskSerializer::KEY_PARENT],
			std::wstring(L"43"));
		Assert::AreEqual(serialized[TaskSerializer::KEY_SUBTASKS],
			std::wstring(L"1;2;3;"));
	}

	/// Should be able to deserialize a complex task.
	TEST_METHOD(deserializeTask) {
		Task task = getMockTask();
		auto serialized = TaskSerializer::serialize(task);
		auto deserialized = TaskSerializer::deserialize(serialized);
		Assert::IsTrue(deserialized == task);
	}

	TEST_METHOD(deserializeEmptyTaskDefaultedToNotCompleted) {
		TaskSerializer::STask serialized;
		serialized.insert({
			TaskSerializer::KEY_ID, L"0"
		});
		serialized.insert({
			TaskSerializer::KEY_DESCRIPTION, L"Any"
		});
		Task task = TaskSerializer::deserialize(serialized);
		Assert::IsFalse(task.isCompleted());
	}

	TEST_METHOD(deserializeInvalidFieldShouldReturnWhatIsAvailable) {
		TaskSerializer::STask serialized;
		serialized.insert({
			TaskSerializer::KEY_ID, L"0"
		});
		serialized.insert({
			TaskSerializer::KEY_DESCRIPTION, L"Wrong!"
		});
		serialized.insert({
			TaskSerializer::KEY_DEADLINE, L"Wrong!"
		});
		Task task = TaskSerializer::deserialize(serialized);
		Assert::IsTrue(task.getID() == Task::ID(0L));
		Assert::AreEqual(task.getDescription(),
			std::wstring(L"Wrong!"));
	}
};
}  // namespace UnitTests
}  // namespace QueryEngine
}  // namespace You
