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

#ifndef OBJECT_CONVERT_HPP
#define OBJECT_CONVERT_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "object/object.hpp"
#include "object/object_ptr.hpp"

#include "tools/convert.hpp"

#include <iostream>
#include <typeinfo>

namespace oos {

class object_container;

#ifdef OOS_DOXYGEN_DOC

#else

template < class T, class U >
void
convert(const object_ptr<T> &from, object_ptr<U> &to,
        typename oos::enable_if<!CPP11_TYPE_TRAITS_NS::is_same<T, U>::value>::type* = 0)
{
  throw std::bad_cast();
}

template < class T >
void
convert(const object_base_ptr &, T &)
{
  throw std::bad_cast();
}

void convert(const object_base_ptr &from, long &to);

template < class T >
void
convert(const T &, object_base_ptr &)
{
  throw std::bad_cast();
}

void convert(const long &, object_base_ptr &);

template < class T >
void
convert(const object_container &, T &)
{
  throw std::bad_cast();
}

template < class T >
void
convert(const T &, object_container &)
{
  throw std::bad_cast();
}

void convert(const object_base_ptr &, char*, int);

void convert(const object_base_ptr &, char*, int, int);

/*
template < class T >
void
convert(const object_ptr<T> &from, object_ptr<T> &to)
{
  std::cout << "1 typeid: " << typeid(T).name() << "\n";
  to = from;
}

OOS_API void convert(const object_base_ptr &from, object_ptr<object> &to);

template < class T >
void
convert(const object_ref<T> &from, object_ptr<T> &to)
{
  std::cout << "2 typeid: " << typeid(T).name() << "\n";
  to = from;
}

template < class T >
void
convert(const object_ptr<T> &from, object_ref<T> &to)
{
  std::cout << "3 typeid: " << typeid(T).name() << "\n";
  to = from;
}
*/
/*
template < class T >
void
convert(const T &, object_base_ptr &)
{
  std::cout << "4 typeid: " << typeid(T).name() << "\n";
}
*/
/*
template < class T >
void
convert(const object_base_ptr &, T &)
{
  std::cout << "5 typeid: " << typeid(T).name() << "\n";
}

void
convert(const long &, object_ptr<object> &)
{
  std::cout << "6 typeid: object_ptr<object>\n";
}
*/

#endif /* OOS_DOXYGEN_DOC */

}

#endif /* OBJECT_CONVERT_HPP */
