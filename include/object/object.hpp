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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif
#include <iostream>

struct object_proxy;
class object_store;
class object_atomizer;

class object
{
	// don't allow copying
	object(const object&);
	object& operator=(const object&);
	
public:
	object();
	virtual ~object();
	
	virtual void read_from(object_atomizer *a);
	virtual void write_to(object_atomizer *a) const;

	const char* type() const;
	long id() const;
	void id(long oid);

  // returns the containing object store
  object_store* ostore() const;

  //bool delete_object();

  friend std::ostream& operator <<(std::ostream &os, const object &o);

protected:
	void mark_modified();

private:
	friend class object_store;
  friend class base_object_ptr;
	
	long id_;
  object_proxy *proxy_;
};

#endif /* OBJECT_HPP */