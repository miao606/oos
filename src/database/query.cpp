#include "database/query.hpp"
#include "database/query_create.hpp"
#include "database/query_insert.hpp"
#include "database/query_update.hpp"
#include "database/query_select.hpp"

#include "database/session.hpp"
#include "database/statement.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

namespace oos {

query::query(session &s)
  : state(QUERY_BEGIN)
  , session_(s)
{}

query::~query()
{
}

query& query::create(const prototype_node &node)
{
  sql_.append(std::string("CREATE TABLE IF NOT EXISTS ") + node.type + std::string(" ("));
  
  object *o = node.producer->create();
  query_create s(sql_, session_.db());
  o->serialize(s);
  delete o;

  sql_.append(")");

  state = QUERY_CREATE;
  return *this;
}

query& query::drop(const prototype_node &node)
{
  sql_.append(std::string("DROP TABLE ") + node.type);

  state = QUERY_DROP;
  return *this;
}

query& query::select(const prototype_node &node)
{
  throw_invalid(QUERY_OBJECT_SELECT, state);

  sql_.append("SELECT ");

  object *o = node.producer->create();
  query_select s(sql_);
  o->serialize(s);
  delete o;

  sql_.append(" FROM ");
  sql_.append(node.type);

  state = QUERY_OBJECT_SELECT;

  return *this;
}
query& query::insert(object *o)
{
  throw_invalid(QUERY_OBJECT_INSERT, state);

  sql_.append(std::string("INSERT INTO ") + o->proxy_->node->type + std::string(" ("));

  query_insert s(sql_);
  s.fields();
  o->serialize(s);

  sql_.append(") VALUES (");

  s.values();
  o->serialize(s);

  sql_.append(")");

  state = QUERY_OBJECT_INSERT;

  return *this;
}
query& query::update(object *o)
{
  throw_invalid(QUERY_OBJECT_UPDATE, state);

  sql_.append(std::string("UPDATE ") + o->proxy_->node->type + std::string(" SET "));

  query_update s(sql_);
  o->serialize(s);

  state = QUERY_OBJECT_UPDATE;

  return *this;
}

query& query::remove(const prototype_node &node)
{
  throw_invalid(QUERY_DELETE, state);

  sql_.append(std::string("DELETE FROM ") + node.type);

  state = QUERY_DELETE;

  return *this;
}

query& query::where(const std::string &clause)
{
  // throw on invalid state
  throw_invalid(QUERY_WHERE, state);

  sql_.append(std::string(" WHERE ") + clause);

  state = QUERY_WHERE;

  return *this;
}
query& query::order_by(const std::string &by)
{
  throw_invalid(QUERY_ORDERBY, state);

  sql_.append(std::string(" ORDER BY ") + by);

  state = QUERY_ORDERBY;

  return *this;
}
query& query::limit(int l)
{
  std::stringstream limval;
  limval << " LIMIT(" << l << ")";
  sql_.append(limval.str());
  return *this;
}
query& query::group_by(const std::string &fld)
{
  throw_invalid(QUERY_GROUPBY, state);

  sql_.append(std::string(" GROUP BY ") + fld);

  state = QUERY_GROUPBY;

  return *this;
}

query& query::select()
{
  throw_invalid(QUERY_SELECT, state);
  sql_.append("SELECT ");
  state = QUERY_SELECT;
  return *this;
}

query& query::column(const std::string &name, sql::data_type_t type)
{
  throw_invalid(QUERY_COLUMN, state);
  if (state == QUERY_COLUMN) {
    sql_.append(", ");
  }
  sql_.append(name.c_str(), type);
  state = QUERY_COLUMN;
  return *this;
}

query& query::from(const std::string &table)
{
  // check state (simple select)
  sql_.append(" FROM " + table + " ");
  return *this;
}

query& query::update(const std::string &table)
{
  throw_invalid(QUERY_UPDATE, state);
  sql_.append("UPDATE " + table + " SET ");
  state = QUERY_UPDATE;
  return *this;
}

result* query::execute()
{
  return session_.execute(sql_.direct().c_str());
}

statement* query::prepare()
{
  std::cout << "prepared statement: " << sql_.prepare() << "\n";

  statement *stmt = session_.create_statement();
  // TODO: fix call to prepare
//  stmt->prepare(sql_);
  return stmt;
}

query& query::reset()
{
  stmt.reset();
  sql_.reset();
  state = QUERY_BEGIN;
  return *this;
}

void query::throw_invalid(query::state_t next, query::state_t current) const
{
  std::stringstream msg;
  switch (next) {
    case query::QUERY_CREATE:
    case query::QUERY_DROP:
    case query::QUERY_SELECT:
    case query::QUERY_INSERT:
    case query::QUERY_UPDATE:
    case query::QUERY_DELETE:
    case query::QUERY_OBJECT_SELECT:
    case query::QUERY_OBJECT_INSERT:
    case query::QUERY_OBJECT_UPDATE:
      if (current != QUERY_BEGIN) {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case query::QUERY_WHERE:
      if (current != query::QUERY_SELECT &&
          current != query::QUERY_SET &&
          current != query::QUERY_DELETE &&
          current != query::QUERY_OBJECT_SELECT &&
          current != query::QUERY_OBJECT_UPDATE)
      {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case query::QUERY_COLUMN:
      if (current != query::QUERY_SELECT &&
          current != query::QUERY_COLUMN)
      {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case query::QUERY_FROM:
      if (current != query::QUERY_COLUMN) {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    case query::QUERY_SET:
      if (current != query::QUERY_UPDATE &&
          current != query::QUERY_SET)
      {
        msg << "invalid next state: [" << next << "] (current: " << current << ")";
        throw std::logic_error(msg.str());
      }
      break;
    default:
      throw std::logic_error("unknown state");
  }
}

}
