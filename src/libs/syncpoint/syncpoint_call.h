/***************************************************************************
 *  syncpoint_call.h - Utility class to represent a call to a SyncPoint
 *
 *  Created: Fri Aug 15 18:12:42 2014
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

#ifndef __SYNCPOINT_SYNCPOINT_CALL_H_
#define __SYNCPOINT_SYNCPOINT_CALL_H_

#include <utils/time/time.h>

namespace fawkes {
#if 0 /* just to make Emacs auto-indent happy */
}
#endif

class SyncPointCall
{
  public:
    SyncPointCall(const char * caller, Time call_time = Time(), Time wait_time =
      Time(0.d));

  public:
    Time get_call_time() const;
    Time get_wait_time() const;
    const char * get_caller() const;

  private:
    const char * const caller_;
    const Time call_time_;
    const Time wait_time_;
};


} // namespace fawkes

#endif
