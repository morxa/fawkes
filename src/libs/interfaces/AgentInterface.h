
/***************************************************************************
 *  AgentInterface.h - Fawkes BlackBoard Interface - AgentInterface
 *
 *  Templated created:   Thu Oct 12 10:49:19 2006
 *  Copyright  2014  Till Hofmann
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

#ifndef __INTERFACES_AGENTINTERFACE_H_
#define __INTERFACES_AGENTINTERFACE_H_

#include <interface/interface.h>
#include <interface/message.h>
#include <interface/field_iterator.h>

namespace fawkes {

class AgentInterface : public Interface
{
 /// @cond INTERNALS
 INTERFACE_MGMT_FRIENDS(AgentInterface)
 /// @endcond
 public:
  /* constants */

 private:
#pragma pack(push,4)
  /** Internal data storage, do NOT modify! */
  typedef struct {
    int64_t timestamp_sec;  /**< Interface Unix timestamp, seconds */
    int64_t timestamp_usec; /**< Interface Unix timestamp, micro-seconds */
    char history[20000]; /**< All actions performed in the past. */
    char plan[20000]; /**< All planned actions. */
  } AgentInterface_data_t;
#pragma pack(pop)

  AgentInterface_data_t *data;

 public:
  /* messages */
  virtual bool message_valid(const Message *message) const;
 private:
  AgentInterface();
  ~AgentInterface();

 public:
  /* Methods */
  char * history() const;
  void set_history(const char * new_history);
  size_t maxlenof_history() const;
  char * plan() const;
  void set_plan(const char * new_plan);
  size_t maxlenof_plan() const;
  virtual Message * create_message(const char *type) const;

  virtual void copy_values(const Interface *other);
  virtual const char * enum_tostring(const char *enumtype, int val) const;

};

} // end namespace fawkes

#endif
