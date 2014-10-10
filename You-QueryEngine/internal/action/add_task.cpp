/// \author A0112054Y
#include "stdafx.h"

#include "../task_builder.h"
#include "../task_serializer.h"
#include "../task_serializer.h"
#include "../state.h"
#include "add_task.h"

namespace You {
namespace QueryEngine {
namespace Internal {
namespace Action {

Task AddTask::buildTask(const Task::ID newID) {
	return TaskBuilder::get().id(newID)
		.description(this->description)
		.deadline(this->deadline)
		.dependencies(this->dependencies)
		.priority(this->priority);
}

void AddTask::addTaskToState(const Task& task,
	State& state) const {
	state.graph().addTask(task);
}

void AddTask::makeTransaction(const Task& newTask) const {
	auto serialized = TaskSerializer::serialize(newTask);
	Transaction t(DataStore::get().begin());
	DataStore::get().post(newTask.getID(), serialized);
	t.commit();
}

Response AddTask::execute(State& state) {
	auto newTask = buildTask(state.inquireNewID());
	addTaskToState(newTask, state);
	makeTransaction(newTask);
	return newTask;
}

}  // namespace Action
}  // namespace Internal
}  // namespace QueryEngine
}  // namespace You
