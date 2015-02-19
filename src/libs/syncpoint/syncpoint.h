/***************************************************************************
 *  syncpoint.h - Fawkes SyncPoint
 *
 *  Created: Thu Jan 09 12:22:03 2014
 *  Copyright  2014  Till Hofmann
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#ifndef __SYNCPOINT_SYNCPOINT_H_
#define __SYNCPOINT_SYNCPOINT_H_

#include <interface/interface.h>
#include <syncpoint/syncpoint_call.h>
#include <core/threading/mutex.h>
#include <core/threading/wait_condition.h>
#include <utils/time/time.h>

#include <core/utils/refptr.h>
#include <core/utils/circular_buffer.h>

#include <set>
#include <map>

namespace fawkes {
#if 0 /* just to make Emacs auto-indent happy */
}
#endif

class SyncPointManager;


class SyncPoint
{
  public:
    SyncPoint(std::string identifier);
    virtual ~SyncPoint();

    /** send a signal to all waiting threads */
    virtual void emit(const std::string & component);

    /** wait for the sync point to be emitted */
    virtual void wait(const std::string & component);

    std::string get_identifier() const;
    bool operator==(const SyncPoint & other) const;
    bool operator<(const SyncPoint & other) const;

    std::set<std::string> get_watchers() const;
    CircularBuffer<SyncPointCall> get_wait_calls() const;
    CircularBuffer<SyncPointCall> get_emit_calls() const;


    /**
     * allow Syncpoint Manager to edit
     */
    friend class SyncPointManager;

  protected:
    std::pair<std::set<std::string>::iterator,bool> add_watcher(std::string watcher);

  protected:
    /** The unique identifier of the SyncPoint */
    const std::string identifier_;
    /** Set of all components which use this SyncPoint */
    std::set<std::string> watchers_;
    /** Set of all components which are currently waiting for the SyncPoint */
    std::set<std::string> waiting_watchers_;

    /** A buffer of the most recent emit calls. */
    CircularBuffer<SyncPointCall> emit_calls_;
    /** A buffer of the most recent wait calls. */
    CircularBuffer<SyncPointCall> wait_calls_;
    /** Time when this SyncPoint was created */
    const Time creation_time_;

    /** Mutex used to protect all member variables */
    Mutex *mutex_;
    /** WaitCondition which is used for wait() and emit() */
    WaitCondition *wait_condition_;

  private:
    /** The predecessor SyncPoint, which is the SyncPoint one level up
     *  e.g. "/test/sp" -> "/test"
     */
    RefPtr<SyncPoint> predecessor_;
};

} // end namespace fawkes

#endif