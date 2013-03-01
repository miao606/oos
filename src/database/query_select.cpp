#include "database/query_select.hpp"

namespace oos {

query_select::query_select(sql &s)
  : dialect(s)
  , first(true)
{}
query_select::~query_select()
{}

void query_select::write(const char *id, char)
{
  write(id, sql::type_char);
}

void query_select::write(const char *id, short)
{
  write(id, sql::type_short);
}

void query_select::write(const char *id, int)
{
  write(id, sql::type_int);
}

void query_select::write(const char *id, long)
{
  write(id, sql::type_long);
}

void query_select::write(const char *id, unsigned char)
{
  write(id, sql::type_unsigned_char);
}

void query_select::write(const char *id, unsigned short)
{
  write(id, sql::type_unsigned_short);
}

void query_select::write(const char *id, unsigned int)
{
  write(id, sql::type_unsigned_int);
}

void query_select::write(const char *id, unsigned long)
{
  write(id, sql::type_unsigned_long);
}

void query_select::write(const char *id, float)
{
  write(id, sql::type_float);
}

void query_select::write(const char *id, double)
{
  write(id, sql::type_double);
}

void query_select::write(const char *id, bool)
{
  write(id, sql::type_char_pointer);
}

void query_select::write(const char *id, const char *, int )
{
  write(id, sql::type_char_pointer);
}

void query_select::write(const char *id, const varchar_base &)
{
  write(id, sql::type_varchar);
}

void query_select::write(const char *id, const std::string &)
{
  write(id, sql::type_text);
}

void query_select::write(const char *id, const object_base_ptr &)
{
  write(id, sql::type_long);
}

void query_select::write(const char *, const object_container &)
{}

void query_select::write(const char *id, sql::data_type_t type)
{
  if (first) {
    first = false;
  } else {
    dialect.append(", ");
  }
  dialect.append(id, type);
}

}
