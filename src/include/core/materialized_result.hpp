#pragma once

#include "duckdb.hpp"
#include "duckdb/common/types/column/column_data_collection.hpp"
#include "duckdb/main/query_result.hpp"

namespace duckdb {

// DuckDB 2.0 merged MaterializedQueryResult into QueryResult, whose rows are read through Collection().
// This wrapper keeps the row-oriented access the OpenIVM code relies on.
class MaterializedQueryResult {
public:
	explicit MaterializedQueryResult(unique_ptr<QueryResult> result_p) : result(std::move(result_p)) {
		if (!result->HasError()) {
			for (auto &name : result->GetNames()) {
				names.push_back(name.GetIdentifierName());
			}
		}
	}

	//! Column names as plain strings (OpenIVM metadata is string-based)
	vector<string> names;

	bool HasError() const {
		return result->HasError();
	}
	const string &GetError() const {
		return result->GetError();
	}
	ErrorData &GetErrorObject() {
		return result->GetErrorObject();
	}
	void ThrowError(const string &prepended_message = "") const {
		result->ThrowError(prepended_message);
	}
	idx_t ColumnCount() const {
		return result->ColumnCount();
	}
	const vector<LogicalType> &GetTypes() const {
		return result->GetTypes();
	}
	const vector<Identifier> &GetNames() const {
		return result->GetNames();
	}
	const Identifier &ColumnName(idx_t index) const {
		return result->ColumnName(index);
	}
	idx_t RowCount() {
		return result->RowCount();
	}
	Value GetValue(idx_t column, idx_t row) {
		return result->Collection().GetValue(column, row);
	}
	ColumnDataCollection &Collection() {
		return result->Collection();
	}
	string ToString() {
		return result->ToString();
	}
	QueryResult &Raw() {
		return *result;
	}

private:
	unique_ptr<QueryResult> result;
};

// Runs a query and materializes it for row access.
template <class... ARGS>
unique_ptr<MaterializedQueryResult> MatQuery(Connection &con, const string &query, ARGS &&... args) {
	return make_uniq<MaterializedQueryResult>(con.Query(query, std::forward<ARGS>(args)...));
}

inline unique_ptr<MaterializedQueryResult> MatQuery(ClientContext &context, const string &query,
                                                    QueryParameters query_parameters) {
	return make_uniq<MaterializedQueryResult>(context.Query(query, std::move(query_parameters)));
}

} // namespace duckdb
