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

#include "database/transaction.hpp"
#include "database/database.hpp"
#include "database/database_impl.hpp"
#include "database/inserter.hpp"

#include "tools/byte_buffer.hpp"

#include "object/object_store.hpp"
#include "object/object.hpp"

#include <iostream>

using namespace std;

namespace oos {

void transaction::on_insert(object *o)
{
//  cout << "inserting " << *o << endl;
  /*****************
   * 
   * backup inserted object
   * on rollback the object
   * is removed from object
   * store
   * 
   *****************/
  if (!has_id(o->id())) {
    backup(new insert_action(o));
  } else {
    // ERROR: an object with that id already exists
  }
}

void transaction::on_update(object *o)
{
//  cout << "updating " << *o << endl;
  /*****************
   * 
   * backup updated object
   * on rollback the object
   * is restored to old values
   * 
   *****************/
  if (!has_id(o->id())) {
    backup(new update_action(o));
  } else {
    // An object with that id already exists
    // do nothing because the object is already
    // backed up
  }
}

void transaction::on_delete(object *o)
{
//  cout << "deleting " << *o << endl;
  /*****************
   * 
   * backup deleted object
   * on rollback the object
   * is restored into the
   * object store
   * 
   *****************/
  if (!has_id(o->id())) {
    backup(new delete_action(o));
  } else {
    // there is already an action for
    // this object inserted
    // try to find out if it is an insert
    // or update action
    iterator i = find_modify_action(o);
    if (i != action_list_.end()) {
      // found modify action
      delete *i;
      *i = new delete_action(o);
    } else {
      const action* a = find_insert_action(o);
      if (a) {
        erase_insert_action(a);
        id_set_.erase(o->id());
      } else {
        /***************
         * 
         * ERROR: this cannot happen!!!
         * 
         ***************/
      }
    }
  }
}

bool transaction::backup_visitor::backup(action *act, byte_buffer *buffer)
{
  buffer_ = buffer;
  act->accept(this);
  buffer_ = NULL;
  return true;
}

void transaction::backup_visitor::visit(insert_action *)
{
  // nothing to do
}

void transaction::backup_visitor::visit(update_action *a)
{
  // serialize object
  serializer_.serialize(a->obj(), *buffer_);
}

void transaction::backup_visitor::visit(delete_action *a)
{
  // serialize object
  serializer_.serialize(a->obj(), *buffer_);
}

bool transaction::restore_visitor::restore(action *act, byte_buffer *buffer, object_store *ostore)
{
  ostore_ = ostore;
  buffer_ = buffer;
  act->accept(this);
  buffer_ = NULL;
  ostore_ = NULL;
  return true;
}

void transaction::restore_visitor::visit(insert_action *a)
{
  // remove object from object store
  ostore_->remove_object(a->obj(), false);
}

void transaction::restore_visitor::visit(update_action *a)
{
  // deserialize data from buffer into object
  serializer_.deserialize(a->obj(), *buffer_, ostore_);
}

void transaction::restore_visitor::visit(delete_action *a)
{
  // check if there is an object with id in
  // object store
  object_proxy *oproxy = ostore_->find_proxy(a->id());
  if (!oproxy) {
    // create proxy
    oproxy = ostore_->create_proxy(a->id());
  }
  if (!oproxy->obj) {
    // create object with id and deserialize
    oproxy->obj = ostore_->create(a->type().c_str());
    // data from buffer into object
    serializer_.deserialize(oproxy->obj, *buffer_, ostore_);
    // insert object
    ostore_->insert_object(oproxy->obj, false);
  } else {
    // data from buffer into object
    serializer_.deserialize(oproxy->obj, *buffer_, ostore_);
  }
  // ERROR: throw exception if id of object
  //        isn't valid (in use)
}

transaction::transaction(database &db)
  : db_(db)
  , id_(0)
{}

transaction::~transaction()
{
}

long transaction::id_counter = 0;

long
transaction::id() const
{
  return id_;
}

void
transaction::begin()
{
  /**************
   * 
   * On begin transaction gets unique
   * id. Transaction is pushed to dbs
   * transactions stack. Previous transactions
   * observer is beeing unregistered and
   * this transactions observer is regsitered.
   *
   **************/
  id_ = ++transaction::id_counter;
  db_.push_transaction(this);
  cout << "starting transaction [" << id_ << "]\n";
}

void
transaction::commit()
{
  if (!db_.current_transaction() || db_.current_transaction() != this) {
    // throw db_exception();
    cout << "commit: transaction [" << id_ << "] isn't current transaction\n";
  } else {
    db_.impl_->commit(insert_action_map_, action_list_);
    // clear actions
    action_list_.clear();
    insert_action_map_.clear();
    object_buffer_.clear();
    id_set_.clear();
    db_.pop_transaction();
  }
}

void
transaction::rollback()
{
  if (!db_.current_transaction() || db_.current_transaction() != this) {
    // throw db_exception();
    cout << "rollback: transaction [" << id_ << "] isn't current transaction\n";
  } else {
    /**************
     *
     * rollback transaction
     * restore objects
     * and finally pop transaction
     * clear insert action map
     *
     **************/
  
    insert_action_map_t::iterator first = insert_action_map_.begin();
    insert_action_map_t::iterator last = insert_action_map_.end();
    while (first != last) {
      while (!first->second.empty()) {
        std::auto_ptr<action> a(first->second.front());
        restore(a.get());
        first->second.pop_front();
      }
      ++first;
    }
    insert_action_map_.clear();
    while (!action_list_.empty()) {
      iterator i = action_list_.begin();
      std::auto_ptr<action> a(*i);
      action_list_.erase(i);
      restore(a.get());
    }
    // clear actions
    action_list_.clear();
    object_buffer_.clear();
    id_set_.clear();
    db_.pop_transaction();
  }
}

database&
transaction::db()
{
  return db_;
}

const database&
transaction::db() const
{
  return db_;
}

void transaction::backup(insert_action *ia)
{
  id_set_.insert(ia->id());

  insert_action_map_t::iterator i = insert_action_map_.find(ia->obj()->object_type());
  if (i == insert_action_map_.end()) {
    i = insert_action_map_.insert(std::make_pair(ia->obj()->object_type(), action_list_t())).first;
  }
  i->second.push_back(ia);
}

void
transaction::backup(action *a)
{
  /*************
   * 
   * try to find object with
   * id in action map
   * 
   *************/
  
  backup_visitor_.backup(a, &object_buffer_);
  action_list_.push_back(a);
  id_set_.insert(a->obj()->id());
/*
  id_set_t::iterator i = id_set_.find(a->obj()->id());
  if (i == id_set_.end()) {
//    cout << "TR (" << id_ << ") couldn't find id [" << a->obj()->id() << "] of object [" << a->obj()->object_type() << "]: inserting action\n";
    backup_visitor_.backup(a, &object_buffer_);
    action_list_.push_back(a);
    id_set_.insert(a->obj()->id());
  } else {
    // find action with id in list
//    cout << "TR (" << id_ << ") found id [" << a->obj()->id() << "] of object [" << a->obj()->object_type() << "]\n";
    iterator first = action_list_.begin();
    iterator last = action_list_.end();
    while (first != last) {
      if ((*first)->obj()->id() == *i) {
        break;
      }
      ++first;
    }
    if (first == last) {
      // couldn't find action
      // error, throw exception
//      cout << "TR (" << id_ << ") FATAL ERROR: couldn't find corresponding object in action list!\n";
    } else {
      if (a->type() == action::DEL && (*first)->type() == action::INSERT) {
//        cout << "TR (" << id_ << ") new action is delete, old action is insert: removing action\n";
        // within this transaction inserted
        // object is also deleted
        // so we can remove insert and
        // delete action, because object
        // has never been existed
        action_list_.erase(first);
        id_set_.erase(i);
      } else if (a->type() == action::DEL && (*first)->type() == action::UPDATE) {
//        cout << "TR (" << id_ << ") new action is delete, old action is update: replace action\n";
        // updated object is also deleted
        // replace update action with delete
        // action
        *first = a;
      } else {
//        cout << "TR (" << id_ << ") nothing todo\n";
        delete a;
      }
    }
  }
  */
}

void transaction::restore(action *a)
{
  restore_visitor_.restore(a, &object_buffer_, &db_.ostore());
}

bool transaction::has_id(long id) const
{
  return id_set_.find(id) != id_set_.end();
}

const action* transaction::find_insert_action(object *o) const
{
  insert_action_map_t::const_iterator i = insert_action_map_.find(o->object_type());
  if (i != insert_action_map_.end()) {
    const_iterator first = i->second.begin();
    const_iterator last = i->second.end();
    while (first != last) {
      if ((*first)->id() == o->id()) {
        return *first;
      }
      ++first;
    }
  }
  return 0;
}

void transaction::erase_insert_action(const action* a)
{
  insert_action_map_t::iterator i = insert_action_map_.find(a->obj()->object_type());
  if (i != insert_action_map_.end()) {
    iterator first = i->second.begin();
    iterator last = i->second.end();
    while (first != last) {
      if ((*first)->id() == a->obj()->id()) {
        delete *first;
        i->second.erase(first);
        return;
      }
      ++first;
    }
  }
}

transaction::iterator transaction::find_modify_action(object *o)
{
  iterator first = action_list_.begin();
  iterator last = action_list_.end();
  while (first != last) {
    if ((*first)->id() == o->id()) {
      break;
    }
    ++first;
  }
  return first;
}

}
