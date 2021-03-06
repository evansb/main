//@author A0114171W
#include "stdafx.h"
#include <CppUnitTest.h>

#include "../mocks.h"
#include "internal/operations/erase_operation.h"
#include "internal/operations/post_operation.h"
#include "internal/operations/put_operation.h"
#include "internal/operations/serialization_operation.h"
#include "internal/operations/branch_operation.h"

using Assert = Microsoft::VisualStudio::CppUnitTestFramework::Assert;

namespace You {
namespace DataStore {
namespace UnitTests {

/// Mock xml_document for \ref DataStoreOperationsTest
static pugi::xml_document mockDocument;

/// Unit Test Class for DataStore class
TEST_CLASS(DataStoreOperationsTest) {
public:
	/// Create a mock xml_document containing a task with content
	/// as specified in task1
	TEST_METHOD_INITIALIZE(initializeMockDocument) {
		mockDocument.reset();
		pugi::xml_node node = mockDocument.
			append_child(TASKS_NODE.c_str()).
			append_child(L"task");
		node.append_attribute(L"id").set_value(L"0");
		node.append_child(TASK_ID.c_str()).
			append_child(pugi::xml_node_type::node_pcdata).
			set_value(task1.at(TASK_ID).c_str());
		node.append_child(DESCRIPTION.c_str()).
			append_child(pugi::xml_node_type::node_pcdata).
			set_value(task1.at(DESCRIPTION).c_str());
		node.append_child(DEADLINE.c_str()).
			append_child(pugi::xml_node_type::node_pcdata).
			set_value(task1.at(DEADLINE).c_str());
		node.append_child(PRIORITY.c_str()).
			append_child(pugi::xml_node_type::node_pcdata).
			set_value(task1.at(PRIORITY).c_str());
		node.append_child(DEPENDENCIES.c_str()).
			append_child(pugi::xml_node_type::node_pcdata).
			set_value(task1.at(DEPENDENCIES).c_str());
	}
	/// Unit tests for \ref Internal::SerializationOperation
	/// @{
	/// Generic serialization operation test
	TEST_METHOD(serializeOperation) {
		pugi::xml_node newNode = mockDocument.append_child(L"new");
		Internal::SerializationOperation::serialize(task2, newNode);
		Assert::AreEqual(task2.at(TASK_ID).c_str(),
			newNode.child(TASK_ID.c_str()).child_value());
		Assert::AreEqual(task2.at(DESCRIPTION).c_str(),
			newNode.child(DESCRIPTION.c_str()).child_value());
		Assert::AreEqual(task2.at(DEADLINE).c_str(),
			newNode.child(DEADLINE.c_str()).child_value());
		Assert::AreEqual(task2.at(PRIORITY).c_str(),
			newNode.child(PRIORITY.c_str()).child_value());
		Assert::AreEqual(task2.at(DEPENDENCIES).c_str(),
			newNode.child(DEPENDENCIES.c_str()).child_value());
	}

	/// Generic deserialization operation test
	TEST_METHOD(deserializeOperation) {
		using SerializationOperation = Internal::SerializationOperation;
		pugi::xml_node toDeserialize = mockDocument.first_child().first_child();
		KeyValuePairs task = SerializationOperation::deserialize(toDeserialize);
		Assert::AreEqual(task1.at(TASK_ID), task.at(TASK_ID));
		Assert::AreEqual(task1.at(DESCRIPTION), task.at(DESCRIPTION));
		Assert::AreEqual(task1.at(DEADLINE), task.at(DEADLINE));
		Assert::AreEqual(task1.at(PRIORITY), task.at(PRIORITY));
		Assert::AreEqual(task1.at(DEPENDENCIES), task.at(DEPENDENCIES));
	}
	/// @}

	/// Unit test for post method for valid input
	TEST_METHOD(postWithNewId) {
		Internal::PostOperation post(TASKS_NODE, L"1", task2);
		bool status = post.run(mockDocument);
		Assert::IsTrue(status);
	}

	/// Unit test for post method for invalid input
	TEST_METHOD(postWithUsedId) {
		Internal::PostOperation post(TASKS_NODE, L"0", task1);
		bool status = post.run(mockDocument);
		Assert::IsFalse(status);
	}

	/// Unit test for put method for valid input
	TEST_METHOD(putWithExistingId) {
		Internal::PutOperation put(TASKS_NODE, L"0", task1);
		bool status = put.run(mockDocument);
		Assert::IsTrue(status);
	}

	/// Unit test for put method for invalid input
	TEST_METHOD(putNonExistentId) {
		Internal::PutOperation put(TASKS_NODE, L"1", task1);
		bool status = put.run(mockDocument);
		Assert::IsFalse(status);
	}

	/// Unit test for erase method for valid input
	TEST_METHOD(eraseExistingId) {
		Internal::EraseOperation erase(TASKS_NODE, L"0");
		bool status = erase.run(mockDocument);
		Assert::IsTrue(status);
	}

	/// Unit test for erase method for invalid input
	TEST_METHOD(eraseNonExistentId) {
		Internal::EraseOperation erase(TASKS_NODE, L"1");
		bool status = erase.run(mockDocument);
		Assert::IsFalse(status);
	}

	/// Unit test for \ref BranchOperation
	/// @{
	/// Create a new branch if none exists
	TEST_METHOD(createNewNodeWithBranchOperation) {
		Internal::BranchOperation::get(mockDocument, L"tasks");
		Assert::IsFalse(mockDocument.child(L"tasks").empty());
	}

	/// Get the branch if it already exists
	TEST_METHOD(getExistingNodeWithBranchOperation) {
		pugi::xml_node expected = mockDocument.append_child(L"resources");
		pugi::xml_node actual =
			Internal::BranchOperation::get(mockDocument, L"resources");
		Assert::IsTrue(expected == actual);
	}
	/// @}
};
}  // namespace UnitTests
}  // namespace DataStore
}  // namespace You
