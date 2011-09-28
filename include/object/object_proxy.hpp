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

#ifndef OBJECT_PROXY_HPP
#define OBJECT_PROXY_HPP

#include <ostream>

namespace oos {

class object;
class object_store;

struct object_proxy;

struct object_proxy {
  object_proxy(object *o = 0, object_store *os = 0);
  ~object_proxy();
  
  friend std::ostream& operator <<(std::ostream &os, const object_proxy &op);

  void remove();
  void insert(object_proxy* oproxy);
  void clear();

  void link_ref();
  void unlink_ref();

  void link_ptr();
  void unlink_ptr();

  object_proxy *prev;
  object_proxy *next;
  object *obj;
  unsigned long id;
  
  unsigned long ref_count;
  unsigned long ptr_count;

  object_store *ostore;
};

}

#endif /* OBJECT_PROXY_HPP */