
/***************************************************************************
 *  exec_thread.h - Fawkes Skiller: Execution Thread
 *
 *  Created: Mon Feb 18 10:28:38 2008
 *  Copyright  2006-2008  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
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

#ifndef __PLUGINS_SKILLER_EXEC_THREAD_H_
#define __PLUGINS_SKILLER_EXEC_THREAD_H_

#include <core/threading/thread.h>
#include <aspect/blocked_timing.h>
#include <aspect/logging.h>
#include <aspect/configurable.h>
#include <aspect/clock.h>
#include <aspect/blackboard.h>
#include <utils/system/fam.h>
#include <blackboard/interface_listener.h>

#include <string>
#include <cstdlib>

namespace fawkes {
  class ComponentLogger;
  class Mutex;
  class LuaContext;
  class LuaInterfaceImporter;
  class Interface;
  class SkillerInterface;
  class SkillerDebugInterface;
}

class SkillerExecutionThread
: public fawkes::Thread,
  public fawkes::BlockedTimingAspect,
  public fawkes::LoggingAspect,
  public fawkes::BlackBoardAspect,
  public fawkes::ConfigurableAspect,
  public fawkes::ClockAspect,
  public fawkes::BlackBoardInterfaceListener
{
 public:
  SkillerExecutionThread();
  virtual ~SkillerExecutionThread();

  virtual void init();
  virtual void loop();
  virtual void finalize();

  /* BlackBoardInterfaceListener */
  void bb_interface_reader_removed(fawkes::Interface *interface, unsigned int instance_serial) throw();

 private: /* methods */
  void init_failure_cleanup();
  void publish_skill_status(std::string &curss);
  void publish_skdbg();
  void publish_error();
  void process_skdbg_messages();

 private: /* members */
  fawkes::ComponentLogger *__clog;

  unsigned int __last_exclusive_controller;
  bool         __reader_just_left;

  bool        __continuous_run;
  bool        __continuous_reset;
  bool        __error_written;

  std::string __skdbg_what;

  // config values
  std::string __cfg_skillspace;
  bool        __cfg_watch_files;

  fawkes::SkillerInterface      *__skiller_if;
  fawkes::SkillerDebugInterface *__skdbg_if;

  fawkes::LuaContext  *__lua;
  fawkes::LuaInterfaceImporter  *__lua_ifi;
};

#endif
