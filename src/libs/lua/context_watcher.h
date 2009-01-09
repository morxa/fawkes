
/***************************************************************************
 *  context_watcher.h - Fawkes Lua Context Watcher
 *
 *  Created: Thu Jan 01 15:16:40 2009
 *  Copyright  2006-2009  Tim Niemueller [www.niemueller.de]
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

#ifndef __LUA_CONTEXT_WATCHER_H_
#define __LUA_CONTEXT_WATCHER_H_

#include <lua/context.h>

namespace fawkes {
#if 0 /* just to make Emacs auto-indent happy */
}
#endif

class LuaContextWatcher
{
 public:
  virtual ~LuaContextWatcher();

  virtual void lua_restarted(LuaContext *context) = 0;
};


} // end of namespace fawkes

#endif
