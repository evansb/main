//@author A0112054Y
#include "stdafx.h"

#include "internal/action/add_task.h"
#include "internal/action/update_task.h"
#include "internal/action/delete_task.h"
#include "internal/action/get_task.h"
#include "internal/action/delete_task.h"
#include "internal/action/update_task.h"
#include "internal/action/undo.h"
#include "internal/action/batch_add_subtasks.h"
#include "internal/action/batch_add_dependencies.h"
#include "internal/state.h"
#include "internal/controller.h"
#include "api.h"

namespace You {
namespace QueryEngine {

const std::wstring Query::logCategory = L"[QE]";

using Internal::Action::AddTask;
using Internal::Action::BatchAddSubTasks;
using Internal::Action::BatchAddDependencies;
using Internal::Action::GetTask;
using Internal::Action::DeleteTask;
using Internal::Action::Undo;
using Internal::Controller;

std::unique_ptr<Query>
Query::getReverse() {
	throw Exception::NotUndoAbleException();
}

std::unique_ptr<Query>
QueryEngine::AddTask(
	const Task::Description& description,
	const Task::Time& startTime,
	const Task::Time& deadline,
	const Task::Priority& priority,
	std::vector<std::unique_ptr<Query>>&& dependencies,
	std::vector<std::unique_ptr<Query>>&& subtasks) {
	assert(!(dependencies.size() > 0 && subtasks.size() > 0));
	if (dependencies.size() > 0) {
		return std::unique_ptr<Query>(new BatchAddDependencies(
				description, startTime, deadline, priority,
				std::move(dependencies), {}));
	} else if (subtasks.size() > 0) {
		return std::unique_ptr<Query>(new BatchAddSubTasks(
				description, startTime, deadline, priority,
				{}, std::move(subtasks)));
	} else {
		return std::unique_ptr<Query>(
			new Internal::Action::AddTask(
				description, startTime, deadline,
				priority, {}, {}));
	}
}

std::unique_ptr<Query>
QueryEngine::GetTask(const Filter& filter) {
	return std::unique_ptr<Query>(
		new Internal::Action::GetTask(filter));
}

std::unique_ptr<Query>
QueryEngine::GetTask(const Filter& filter,
	const Comparator& comparator) {
	return std::unique_ptr<Query>(
		new Internal::Action::GetTask(filter, comparator));
}

std::unique_ptr<Query>
QueryEngine::DeleteTask(Task::ID id) {
	return std::unique_ptr<Query>(
		new Internal::Action::DeleteTask(id));
}

std::unique_ptr<Query>
QueryEngine::UpdateTask(Task::ID id,
	You::Utils::Option<Task::Description> description,
	You::Utils::Option<Task::Time> startTime,
	You::Utils::Option<Task::Time> deadline,
	You::Utils::Option<Task::Priority> priority,
	You::Utils::Option<Task::Dependencies> dependencies,
	You::Utils::Option<bool> completed,
	You::Utils::Option<Task::ID> parent,
	You::Utils::Option<Task::Subtasks> subtasks,
	You::Utils::Option<Task::Attachment> attachment) {
	using UpdateTask = Internal::Action::UpdateTask;
	return std::unique_ptr<Query>(new UpdateTask(id, description,
		startTime, deadline, priority, dependencies, completed,
		parent, subtasks, attachment));
}

std::unique_ptr<Query>
QueryEngine::UpdateTask(const Task& task) {
	using UpdateTask = Internal::Action::UpdateTask;
	return std::unique_ptr<Query>(new UpdateTask(task));
}

std::unique_ptr<Query>
QueryEngine::Undo() {
	return std::unique_ptr<Query>(new Internal::Action::Undo());
}

std::unique_ptr<Query>
QueryEngine::AddDependency(Task::ID id, Task::ID dependency) {
	Task task = Internal::State::get().graph().getTask(id);
	Task::Dependencies dependencies = task.getDependencies();
	dependencies.insert(dependency);
	task.setDependencies(dependencies);
	return std::unique_ptr<Query>(new Internal::Action::UpdateTask(task));
}

std::unique_ptr<Query>
QueryEngine::RemoveDependency(Task::ID id, Task::ID dependency) {
	Task task = Internal::State::get().graph().getTask(id);
	Task::Dependencies dependencies = task.getDependencies();
	dependencies.erase(dependency);
	task.setDependencies(dependencies);
	return std::unique_ptr<Query>(new Internal::Action::UpdateTask(task));
}

std::unique_ptr<Query>
QueryEngine::AddSubtask(Task::ID id, Task::ID subtask) {
	Task task = Internal::State::get().graph().getTask(id);
	Task::Subtasks subtasks = task.getSubtasks();
	subtasks.insert(subtask);
	task.setSubtasks(subtasks);
	return std::unique_ptr<Query>(new Internal::Action::UpdateTask(task));
}

std::unique_ptr<Query>
QueryEngine::RemoveSubtask(Task::ID id, Task::ID subtask) {
	Task task = Internal::State::get().graph().getTask(id);
	Task::Subtasks subtasks = task.getSubtasks();
	subtasks.erase(subtask);
	task.setSubtasks(subtasks);
	return std::unique_ptr<Query>(new Internal::Action::UpdateTask(task));
}

Response QueryEngine::executeQuery(std::unique_ptr<Query> query) {
	Response response;
	std::unique_ptr<Query> reverse;
	response = query->execute(Internal::State::get());
	try {
		reverse = query->getReverse();
		Internal::State::get().undoStack().emplace(std::move(reverse));
	} catch (const Exception::NotUndoAbleException&) {
	}
	return response;
}

std::wstring ToString(const Task& task) {
	using Serializer = Controller::Serializer;
	auto serialized = Serializer::serialize(task);
	std::wstring TASK_FORMAT = L"[%1%][%2%][%3%][%4%][%5%][%6%][%7%][%8%]";
	return (boost::wformat(TASK_FORMAT)
		% serialized.at(Serializer::KEY_ID)
		% serialized.at(Serializer::KEY_DESCRIPTION)
		% serialized.at(Serializer::KEY_PRIORITY)
		% serialized.at(Serializer::KEY_DEADLINE)
		% serialized.at(Serializer::KEY_DEPENDENCIES)
		% serialized.at(Serializer::KEY_PARENT)
		% serialized.at(Serializer::KEY_SUBTASKS)
		% serialized.at(Serializer::KEY_ATTACHMENT)).str();
}

}  // namespace QueryEngine
}  // namespace You

