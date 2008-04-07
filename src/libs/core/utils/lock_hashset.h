
/***************************************************************************
 *  lock_hashset.h - Lockable hash set
 *
 *  Created: Sat May 12 13:06:31 2007
 *  Copyright  2006-2007  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#ifndef __CORE_UTILS_LOCK_HASHSET_H_
#define __CORE_UTILS_LOCK_HASHSET_H_

#include <core/threading/mutex.h>
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
#  include <tr1/unordered_set>
#else
#  include <ext/hash_set>
#endif

template <class KeyType,
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
          class HashFunction = std::tr1::hash<KeyType>,
          class EqualKey     = std::equal_to<KeyType> >
class LockHashSet : public std::tr1::unordered_set<KeyType, HashFunction, EqualKey>
#else
          class HashFunction = __gnu_cxx::hash<KeyType>,
          class EqualKey     = std::equal_to<KeyType> >
class LockHashSet : public __gnu_cxx::hash_set<KeyType, HashFunction, EqualKey>
#endif
{
 public:
  LockHashSet();
  LockHashSet(const LockHashSet<KeyType, HashFunction, EqualKey> &lh);
  virtual ~LockHashSet();

  void     lock();
  bool     try_lock();
  void     unlock();
  Mutex *  mutex() const;

  void     insert_locked(const KeyType& x);

 private:
  Mutex *__mutex;

};


/** @class LockHashSet core/utils/lock_hashset.h
 * Hash set with a lock.
 * This class provides a hash set that has an intrinsic lock. The lock can be applied
 * with the regular locking methods.
 *
 * @see Mutex
 * @ingroup FCL
 * @author Tim Niemueller
 */


/** Constructor. */
template <class KeyType, class HashFunction, class EqualKey>
LockHashSet<KeyType, HashFunction, EqualKey>::LockHashSet()
{
  __mutex = new Mutex();
}


/** Copy constructor.
 * @param lh LockHashSet to copy
 */
template <class KeyType, class HashFunction, class EqualKey>
LockHashSet<KeyType, HashFunction, EqualKey>::LockHashSet(const LockHashSet<KeyType, HashFunction, EqualKey> &lh)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
  : std::tr1::unordered_set<KeyType, HashFunction, EqualKey>::unordered_set(lh)
#else
  : __gnu_cxx::hash_set<KeyType, HashFunction, EqualKey>::hash_set(lh)
#endif
{
  __mutex = new Mutex();
}


/** Destructor. */
template <class KeyType, class HashFunction, class EqualKey>
LockHashSet<KeyType, HashFunction, EqualKey>::~LockHashSet()
{
  delete __mutex;
}


/** Lock list. */
template <class KeyType, class HashFunction, class EqualKey>
void
LockHashSet<KeyType, HashFunction, EqualKey>::lock()
{
  __mutex->lock();
}


/** Try to lock list.
 * @return true, if the lock has been aquired, false otherwise.
 */
template <class KeyType, class HashFunction, class EqualKey>
bool
LockHashSet<KeyType, HashFunction, EqualKey>::try_lock()
{
  return __mutex->try_lock();
}


/** Unlock list. */
template <class KeyType, class HashFunction, class EqualKey>
void
LockHashSet<KeyType, HashFunction, EqualKey>::unlock()
{
  return __mutex->unlock();
}


/** Insert element to hash set with lock protection.
 * @param x element to add
 */
template <class KeyType, class HashFunction, class EqualKey>
void
LockHashSet<KeyType, HashFunction, EqualKey>::insert_locked(const KeyType& x)
{
  __mutex->lock();
  insert(x);
  __mutex->unlock();
}


/** Get access to the internal mutex.
 * Can be used with MutexLocker.
 * @return internal mutex
 */
template <typename KeyType, class HashFunction, class EqualKey>
Mutex *
LockHashSet<KeyType, HashFunction, EqualKey>::mutex() const
{
  return __mutex;
}

#endif
