
/***************************************************************************
 *  example_mutex_sync.cpp - example application for using mutexes
 *                           to synchronize several threads to a given point
 *                           in time
 *
 *  Generated: Thu Sep 21 11:55:59 2006
 *  Copyright  2006  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

/// @cond EXAMPLES

#include <core/threading/thread.h>
#include <core/threading/wait_condition.h>
#include <core/threading/mutex.h>

#include <iostream>
#include <string>
#include <vector>

using namespace std;

/** Small example hread serializing with other threads using a wait condition.
 * Run the program and see them printing out numbers serialized.
 *
 * NOTE: This can be done more easily by using ThreadList and Threads in 
 * wait-for-wakeup mode! This is just a demonstration to improve understanding
 * of sync constructs.
 */
class ExampleMutexWaitThread : public Thread
{
 public:
  ExampleMutexWaitThread(string s)
    : Thread("ExampleMutexWaitThread", Thread::OPMODE_CONTINUOUS)
  {
    this->s      = s;

    m.lock();
  }

  ~ExampleMutexWaitThread()
  {
  }

  void wake()
  {
    m.unlock();;
  }

  string getS()
  {
    return s;
  }

  /** Action!
   */
  virtual void loop()
  {
    m.lock();
    cout << s << ": my turn" << endl;
    // unlock mutex inside wait condition
    m.unlock();
  }

 private:
  Mutex          m;
  string         s;

};

class ExampleMutexWaitStarterThread : public Thread
{
 public:
  ExampleMutexWaitStarterThread()
    : Thread("ExampleMutexWaitStarterThread", Thread::OPMODE_CONTINUOUS)
  {
    threads.clear();
  }

  void wakeThreads()
  {
    vector< ExampleMutexWaitThread * >::iterator tit;
    for (tit = threads.begin(); tit != threads.end(); ++tit) {
      cout << "Waking thread " << (*tit)->getS() << endl;
      (*tit)->wake();
    }
  }

  void addThread(ExampleMutexWaitThread *t)
  {
    threads.push_back(t);
  }


  virtual void loop()
  {
    sleep(2423423);
    wakeThreads();
  }

 private:
  vector< ExampleMutexWaitThread * > threads;
};

/* This small app uses a condition variable to serialize
 * a couple of threads
 */
int
main(int argc, char **argv)
{

  ExampleMutexWaitThread *t1 = new ExampleMutexWaitThread("t1");
  ExampleMutexWaitThread *t2 = new ExampleMutexWaitThread("t2");
  ExampleMutexWaitThread *t3 = new ExampleMutexWaitThread("t3");

  ExampleMutexWaitStarterThread *st = new ExampleMutexWaitStarterThread();
  st->addThread( t1 );
  st->addThread( t2 );
  st->addThread( t3 );

  t1->start();
  t2->start();
  t3->start();
  st->start();

  t1->join();
  t2->join();
  t3->join();
  st->join();

  delete st;
  delete t3;
  delete t2;
  delete t1;

  return 0;
}


/// @endcond
