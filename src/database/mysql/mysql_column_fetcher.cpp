#include "database/mysql/mysql_column_fetcher.hpp"

#include "object/object.hpp"

namespace oos {

namespace mysql {

mysql_column_fetcher::mysql_column_fetcher(MYSQL_STMT *stmt, MYSQL_BIND *bind, mysql_prepared_result::result_info *info)
  : generic_object_reader<mysql_column_fetcher>(this)
  , column_index_(0)
  , stmt_(stmt)
  , bind_(bind)
  , info_(info)
{}

mysql_column_fetcher::~mysql_column_fetcher()
{}

void mysql_column_fetcher::fetch(object *o)
{
  column_index_ = 0;
  o->deserialize(*this);
}

void mysql_column_fetcher::read_value(const char *, std::string &x)
{
  if (info_[column_index_].length > 0) {
    bind_[column_index_].buffer = new char[info_[column_index_].length];
    bind_[column_index_].buffer_length = info_[column_index_].length;
    if (mysql_stmt_fetch_column(stmt_, &bind_[column_index_], column_index_, 0) != 0) {
      // an error occured
    } else {
      char *data = (char*)bind_[column_index_].buffer;
      unsigned long len = bind_[column_index_].buffer_length;
      x.assign(data, len);
    }
  }
  ++column_index_;
}

void mysql_column_fetcher::read_value(const char *id, varchar_base &x)
{
  ++column_index_;
}

void mysql_column_fetcher::read_value(const char *, char *, int )
{
  ++column_index_;
}

}

}
