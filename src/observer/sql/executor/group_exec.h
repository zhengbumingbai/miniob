#include <string>
#include <sstream>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "event/execution_plan_event.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"

RC group_decompose(TupleSet& tupleset_in, std::vector<TupleSet>& tuplesets_out, const Selects& selects, const char * db);

RC group_aggr(std::vector<TupleSet>& tuplesets_in, std::vector<TupleSet>& tuplesets_out, const Selects& selects, const char * db);

RC group_de_duplication(std::vector<TupleSet>& tuplesets_in, std::vector<TupleSet>& tuplesets_out, const Selects& selects, const char * db);

RC group_compose(std::vector<TupleSet>& tuplesets_in, TupleSet& tupleset_out);