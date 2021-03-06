//@author A0097630B
#include "stdafx.h"
#include "query_executor.h"

namespace You {
namespace Controller {
namespace Internal {

QueryExecutor::QueryExecutor(std::unique_ptr<You::QueryEngine::Query>&& query)
: query(std::move(query)) {
}

Result QueryExecutor::execute() {
	assert(query.get() &&
		"QueryExecutors are single-use only.");
	QueryEngine::Response response = QueryEngine::executeQuery(
		std::move(query));
	return processResponse(response);
}

}  // namespace Internal
}  // namespace Controller
}  // namespace You
