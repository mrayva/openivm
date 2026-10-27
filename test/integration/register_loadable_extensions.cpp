// Registers duckdb_loadable_extensions in executables that must LOAD external extensions such as ducklake.
// duckdb_link_extensions cannot do it: that archive's target does not exist yet when this CMakeLists is processed.
// Compile this file into the executable itself; in an archive the linker would drop the initializer.

#include "duckdb_static_extension.h"

extern "C" int32_t duckdb_extension_loadable_extensions_describe(duckdb_extension_descriptor *descriptor);

namespace {
struct RegisterLoadableExtensions {
	RegisterLoadableExtensions() {
		duckdb_register_static_extension(duckdb_extension_loadable_extensions_describe);
	}
};
const RegisterLoadableExtensions register_loadable_extensions;
} // namespace
