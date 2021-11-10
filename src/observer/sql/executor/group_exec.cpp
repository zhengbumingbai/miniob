#include "group_exec.h"

#include <unordered_map>

#include "execute_stage.h"

RC group_decompose(TupleSet& tupleset_in, std::vector<TupleSet>& tuplesets_out,
                   const Selects& selects, const char* db) {
  std::vector<GroupAttr> group_attrs;
  if (selects.group_num > 0) {
    for (int i = selects.group_num - 1; i >= 0; i--) {
      // 反向遍历以与输入顺序对齐
      GroupAttr group_attr(selects.group_attr[i]);
      Table* table;
      if (group_attr.relation_name == nullptr) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr must have table name in multi table, field [%s]",
                   group_attr.attribute_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          group_attr.relation_name = selects.relations[0];
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
        }
      } else {
        table = DefaultHandler::get_default().find_table(
            db, group_attr.relation_name);
      }
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", group_attr.relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      const FieldMeta* field_meta =
          table->table_meta().field(group_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("No such field in group. %s.%s", table->name(),
                 group_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      group_attrs.push_back(group_attr);
    }
  }
  std::unordered_map<int, bool> loc_pin;
  for (int i = 0; i < tupleset_in.size(); i++) {
    if (loc_pin[i] == 1) {
      continue;
    }
    TupleSet tupleset_gen;
    std::vector<TupleValue*> values_compare;
    Tuple& tuple = tupleset_in.get_edit(i);
    for (int j = 0; j < group_attrs.size(); j++) {
      int index = tupleset_in.schema().index_of_field(
          group_attrs[j].relation_name, group_attrs[j].attribute_name);
      values_compare.push_back(tuple.get_edit(index).get());
    }

    {
      Tuple tuple_copy(tuple);
      tupleset_gen.add(std::move(tuple_copy));
      loc_pin[i] = 1;
    }

    for (int j = i + 1; j < tupleset_in.size(); j++) {
      const Tuple& tuple_compare = tupleset_in.get(j);
      bool equal = true;
      for (int k = 0; k < group_attrs.size(); k++) {
        int index = tupleset_in.schema().index_of_field(
            group_attrs[k].relation_name, group_attrs[k].attribute_name);
        const TupleValue& tuple_value = tuple_compare.get(index);
        if (values_compare[k]->compare(tuple_value) != 0) {
          equal = false;
        }
      }
      if (equal) {
        Tuple tuple_copy(tuple_compare);
        tupleset_gen.add(std::move(tuple_copy));
        loc_pin[j] = 1;
      }
    }
    tupleset_gen.set_schema(tupleset_in.get_schema());

    tuplesets_out.emplace_back(std::move(tupleset_gen));
  }
  return RC::SUCCESS;
}

RC group_aggr(std::vector<TupleSet>& tuplesets_in,
              std::vector<TupleSet>& tuplesets_out, const Selects& selects,
              const char* db) {
  std::vector<GroupAttr> group_attrs;
  if (selects.group_num > 0) {
    for (int i = selects.group_num - 1; i >= 0; i--) {
      // 反向遍历以与输入顺序对齐
      GroupAttr group_attr(selects.group_attr[i]);
      Table* table;
      if (group_attr.relation_name == nullptr) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr must have table name in multi table, field [%s]",
                   group_attr.attribute_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          group_attr.relation_name = selects.relations[0];
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
        }
      } else {
        table = DefaultHandler::get_default().find_table(
            db, group_attr.relation_name);
      }
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", group_attr.relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      const FieldMeta* field_meta =
          table->table_meta().field(group_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("No such field in group. %s.%s", table->name(),
                 group_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      group_attrs.push_back(group_attr);
    }
  }
  std::vector<RelAttr> attrs;
  std::vector<AttrType> types;
  if (selects.attr_num > 0) {
    for (int i = 0; i < selects.attr_num; i++) {
      // 反向遍历以与输入顺序对齐
      RelAttr attr(selects.attributes[i]);
      Table* table;
      if (attr.relation_name == nullptr) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr must have table name in multi table, field [%s]",
                   attr.attribute_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          attr.relation_name = selects.relations[0];
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
        }
      } else {
        table =
            DefaultHandler::get_default().find_table(db, attr.relation_name);
      }
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", attr.relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      const FieldMeta* field_meta =
          table->table_meta().field(attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("No such field in group select. %s.%s", table->name(),
                 attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      bool exist = false;
      for (int j = 0; j < group_attrs.size(); j++) {
        if (strcmp(group_attrs[j].relation_name, attr.relation_name) == 0 &&
            strcmp(group_attrs[j].attribute_name, attr.attribute_name) == 0) {
          exist = true;
        }
      }
      if (exist) {
        attrs.push_back(attr);
        types.push_back(field_meta->type());
      } else {
        LOG_DEBUG("Select column not existed in group.");
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    }
  }
  for (int i = 0; i < tuplesets_in.size(); i++) {
    TupleSet tupleset_out;
    RC rc = aggr_execution(selects, db, tuplesets_in[i], tupleset_out);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    for (int j = attrs.size() - 1; j >= 0; j--) {
      tupleset_out.get_edit_schema().add_front(types[j], attrs[j].relation_name,
                                               attrs[j].attribute_name);
      int index = tuplesets_in[i].get_schema().index_of_field(
          attrs[j].relation_name, attrs[j].attribute_name);
      tupleset_out.get_edit_tuples()[0].add_front(
          tuplesets_in[i].tuples()[0].get_edit(index));
    }
    tuplesets_out.emplace_back(std::move(tupleset_out));
  }
  return RC::SUCCESS;
}

RC group_de_duplication(std::vector<TupleSet>& tuplesets_in,
                        std::vector<TupleSet>& tuplesets_out,
                        const Selects& selects, const char* db) {
  std::vector<GroupAttr> group_attrs;
  if (selects.group_num > 0) {
    for (int i = selects.group_num - 1; i >= 0; i--) {
      // 反向遍历以与输入顺序对齐
      GroupAttr group_attr(selects.group_attr[i]);
      Table* table;
      if (group_attr.relation_name == nullptr) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr must have table name in multi table, field [%s]",
                   group_attr.attribute_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          group_attr.relation_name = selects.relations[0];
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
        }
      } else {
        table = DefaultHandler::get_default().find_table(
            db, group_attr.relation_name);
      }
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", group_attr.relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      const FieldMeta* field_meta =
          table->table_meta().field(group_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("No such field in group. %s.%s", table->name(),
                 group_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      group_attrs.push_back(group_attr);
    }
  }
  std::vector<RelAttr> attrs;
  std::vector<AttrType> types;
  std::vector<const RelAttr*> attrs_old;
  if (selects.attr_num > 0) {
    for (int i = 0; i < selects.attr_num; i++) {
      // 反向遍历以与输入顺序对齐
      RelAttr attr(selects.attributes[i]);
      Table* table;
      if (attr.relation_name == nullptr) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr must have table name in multi table, field [%s]",
                   attr.attribute_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          attr.relation_name = selects.relations[0];
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
        }
      } else {
        table =
            DefaultHandler::get_default().find_table(db, attr.relation_name);
      }
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", attr.relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      const FieldMeta* field_meta =
          table->table_meta().field(attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("No such field in group select. %s.%s", table->name(),
                 attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      bool exist = false;
      for (int j = 0; j < group_attrs.size(); j++) {
        if (strcmp(group_attrs[j].relation_name, attr.relation_name) == 0 &&
            strcmp(group_attrs[j].attribute_name, attr.attribute_name) == 0) {
          exist = true;
        }
      }
      if (exist) {
        attrs.push_back(attr);
        attrs_old.push_back(&selects.attributes[i]);
        types.push_back(field_meta->type());
      } else {
        LOG_DEBUG("Select column not existed in group.");
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    }
  }
  for (int i = 0; i < tuplesets_in.size(); i++) {
    TupleSet tupleset_out;
    Tuple tuple_out;
    for (int j = attrs.size() - 1; j >= 0; j--) {
      tupleset_out.get_edit_schema().add_front(types[j], attrs[j].relation_name,
                                               attrs[j].attribute_name);
      int index = tuplesets_in[i].get_schema().index_of_field(
          attrs[j].relation_name, attrs[j].attribute_name);
      tuple_out.add_front(tuplesets_in[i].tuples()[0].get_edit(index));
    }
    tupleset_out.add(std::move(tuple_out));
    tuplesets_out.emplace_back(std::move(tupleset_out));
  }
  return RC::SUCCESS;
}

RC group_compose(std::vector<TupleSet>& tuplesets_in, TupleSet& tupleset_out) {
  tupleset_out.set_schema(tuplesets_in[0].get_schema());
  for (int i = 0; i < tuplesets_in.size(); i++) {
    tupleset_out.add(std::move(tuplesets_in[i].get_edit_tuples()[0]));
  }
  return RC::SUCCESS;
}