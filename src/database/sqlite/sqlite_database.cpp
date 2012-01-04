/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/sqlite/sqlite_database.hpp"
#include "database/sqlite/sqlite_statement.hpp"

#include "database/transaction.hpp"

#include <exception>
#include <sqlite3.h>

namespace oos {
  
namespace sqlite {
  
sqlite_database::sqlite_database(const std::string &db)
  : db_(0)
{
  int ret = sqlite3_open(db.c_str(), &db_);
  if (ret != SQLITE_OK) {
//    throw std::runtime_error("couldn't open database: " + db);
  }
}

sqlite_database::~sqlite_database()
{
  sqlite3_close(db_);
}

void sqlite_database::visit(insert_action *a)
{
  // create insert statement

}

void sqlite_database::visit(update_action *a)
{
  // create update statement
}

void sqlite_database::visit(delete_action *a)
{
  // create delete statement
}

transaction_impl* sqlite_database::create_transaction(transaction &tr) const
{
  return new transaction_impl(tr);
}

sqlite_statement* sqlite_database::create_statement(const std::string &sql)
{
  return new sqlite_statement(*this, sql);
}

sqlite3* sqlite_database::operator()()
{
  return db_;
}

}
}