#pragma once

#include "duckdb.hpp"
#include "duckdb/catalog/catalog.hpp"
#include "duckdb/catalog/catalog_entry/schema_catalog_entry.hpp"
#include "duckdb/catalog/catalog_entry/table_catalog_entry.hpp"
#include "duckdb/common/identifier.hpp"
#include "duckdb/transaction/meta_transaction.hpp"
#include "duckdb/transaction/transaction.hpp"
#include "duckdb/main/client_data.hpp"

namespace duckdb {

// DuckDB 2.0 names catalog objects with Identifier; OpenIVM keeps plain strings in its metadata.
// These wrappers keep the string-based call sites readable.
inline const string &Str(const string &s) {
	return s;
}
inline const string &Str(const Identifier &s) {
	return s.GetIdentifierName();
}
inline string Str(const char *s) {
	return string(s);
}

template <class T, class C, class S, class N>
optional_ptr<T> GetCatalogEntry(ClientContext &context, const C &catalog, const S &schema, const N &name,
                                OnEntryNotFound if_not_found, QueryErrorContext error_context = QueryErrorContext()) {
	return Catalog::GetEntry<T>(context,
	                            QualifiedName(Identifier(Str(catalog)), Identifier(Str(schema)), Identifier(Str(name))),
	                            if_not_found, error_context);
}

template <class T, class C, class S, class N>
T &GetCatalogEntry(ClientContext &context, const C &catalog, const S &schema, const N &name) {
	return Catalog::GetEntry<T>(
	    context, QualifiedName(Identifier(Str(catalog)), Identifier(Str(schema)), Identifier(Str(name))));
}

inline Catalog &GetCatalogByName(ClientContext &context, const string &catalog) {
	return Catalog::GetCatalog(context, Identifier(catalog));
}

inline Catalog &GetCatalogByName(ClientContext &context, const Identifier &catalog) {
	return Catalog::GetCatalog(context, catalog);
}

inline optional_ptr<SchemaCatalogEntry> GetSchemaEntry(ClientContext &context, const string &catalog,
                                                       const string &schema, OnEntryNotFound if_not_found) {
	return Catalog::GetSchema(context, Identifier(catalog), Identifier(schema), if_not_found);
}

inline unique_ptr<TableDescription> TableInfoOf(Connection &con, const string &database, const string &schema,
                                                const string &table) {
	return con.TableInfo(Identifier(database), Identifier(schema), Identifier(table));
}

inline unique_ptr<TableDescription> TableInfoOf(Connection &con, const Identifier &database, const Identifier &schema,
                                                const Identifier &table) {
	return con.TableInfo(database, schema, table);
}

template <class T>
optional_ptr<T> GetCatalogEntry(ClientContext &context, const QualifiedName &name, OnEntryNotFound if_not_found,
                                QueryErrorContext error_context = QueryErrorContext()) {
	return Catalog::GetEntry<T>(context, name, if_not_found, error_context);
}

template <class T>
T &GetCatalogEntry(ClientContext &context, const QualifiedName &name) {
	return Catalog::GetEntry<T>(context, name);
}

inline unique_ptr<TableDescription> TableInfoOf(Connection &con, const string &schema, const string &table) {
	return con.TableInfo(Identifier(schema), Identifier(table));
}

inline unique_ptr<TableDescription> TableInfoOf(Connection &con, const string &table) {
	return con.TableInfo(Identifier(table));
}

} // namespace duckdb

namespace duckdb {

inline EntryLookupInfo LookupOf(CatalogType type, const string &name,
                                QueryErrorContext error_context = QueryErrorContext()) {
	return EntryLookupInfo(type, QualifiedName(Identifier(name)), error_context);
}

inline optional_ptr<SchemaCatalogEntry> GetSchemaOf(Catalog &catalog, ClientContext &context, const string &schema,
                                                    OnEntryNotFound if_not_found) {
	return catalog.GetSchema(context, Identifier(schema), if_not_found);
}

inline optional_ptr<CatalogEntry> GetEntryQ(ClientContext &context, const string &catalog, const string &schema,
                                            const EntryLookupInfo &lookup, OnEntryNotFound if_not_found) {
	EntryLookupInfo qualified(
	    lookup, QualifiedName(Identifier(catalog), Identifier(schema), Identifier(lookup.GetEntryName())));
	return Catalog::GetEntry(context, qualified, if_not_found);
}

inline bool CIEq(const string &a, const string &b) {
	return StringUtil::CIEquals(a, b);
}
inline bool CIEq(const Identifier &a, const string &b) {
	return StringUtil::CIEquals(a.GetIdentifierName(), b);
}
inline bool CIEq(const string &a, const Identifier &b) {
	return StringUtil::CIEquals(a, b.GetIdentifierName());
}
inline bool CIEq(const Identifier &a, const Identifier &b) {
	return StringUtil::CIEquals(a.GetIdentifierName(), b.GetIdentifierName());
}

} // namespace duckdb
