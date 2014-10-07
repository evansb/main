#include "stdafx.h"
#include "datastore.h"

namespace You {
namespace DataStore {

DataStore& DataStore::get() {
	static DataStore ds;
	return ds;
}

Transaction&& DataStore::begin() {
	while (this->isServing) { }
	Transaction t;
	return std::move(t);
}

}  // namespace DataStore
}  // namespace You