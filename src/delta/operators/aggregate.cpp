#include "delta/delta_operator.hpp"
#include "core/openivm_constants.hpp"
#include "core/openivm_debug.hpp"
#include "duckdb/planner/expression/bound_columnref_expression.hpp"
#include "duckdb/planner/operator/logical_aggregate.hpp"

namespace duckdb {

DeltaPlanFragment CompileAggregateDelta(DeltaOperatorInput input) {
	auto &agg_node = input.plan->Cast<LogicalAggregate>();
	LogDeltaOperatorStrategy(input, DeltaOperatorStrategy::AGGREGATE_GROUP_BY_MULTIPLICITY);
	OPENIVM_DEBUG_PRINT("[DeltaAggregate] Rewriting AGGREGATE node, %zu groups, %zu aggregates\n",
	                    agg_node.groups.size(), agg_node.expressions.size());
	auto child_mul = input.CompileChild(input.plan->children[0], input.root);
	input.plan->children[0] = std::move(child_mul.op);
	ColumnBinding input_mul_binding = child_mul.mul_binding;

	auto &modified_node = input.plan->Cast<LogicalAggregate>();
	idx_t multiplicity_group_index = modified_node.groups.size();

	auto mult_group_by =
	    make_uniq<BoundColumnRefExpression>(openivm::MULTIPLICITY_COL, input.mul_type, input_mul_binding);
	ColumnBinding mod_mul_binding;
	mod_mul_binding.column_index = ProjectionIndex(modified_node.groups.size());
	modified_node.groups.emplace_back(std::move(mult_group_by));

	auto mult_group_by_stats = make_uniq<BaseStatistics>(BaseStatistics::CreateUnknown(input.mul_type));
	modified_node.group_stats.emplace_back(std::move(mult_group_by_stats));

	// An empty grouping_sets vector means one implicit set containing every group.
	// Keep it empty so the newly-added multiplicity group participates in that set.
	// For explicit grouping sets, include multiplicity in every set at its actual
	// position in groups (not at the set's cardinality).
	for (auto &grouping_set : modified_node.grouping_sets) {
		grouping_set.insert(ProjectionIndex(multiplicity_group_index));
	}

	mod_mul_binding.table_index = modified_node.group_index;
	input.plan->Verify(input.context.input.context);
	OPENIVM_DEBUG_PRINT("[DeltaAggregate] Done, now %zu groups (added mul), mul_binding: table=%lu col=%lu\n",
	                    modified_node.groups.size(), (unsigned long)mod_mul_binding.table_index.index,
	                    (unsigned long)mod_mul_binding.column_index.GetIndexUnsafe());
	return {std::move(input.plan), mod_mul_binding};
}

} // namespace duckdb
