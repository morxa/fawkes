
/***************************************************************************
 *  skiller_plugin.h - Fawkes Skill Execution Runtime Plugin
 *
 *  Created: Mon Feb 18 10:18:50 2008
 *  Copyright  2006-2008  Tim Niemueller [www.niemueller.de]
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <plugins/skiller/skiller_plugin.h>
#include <plugins/skiller/liaison_thread.h>
#include <plugins/skiller/exec_thread.h>

#include <core/threading/barrier.h>

/** @class SkillerPlugin <plugins/skiller/skiller_plugin.h>
 * Skill Execution Runtime Plugin.
 * This plugin facilitates the Fawkes skill module. It allows for execution of
 * so-called skills, basic action atoms that can be used for form complex
 * behavior.
 *
 * @author Tim Niemueller
 */

/** Constructor. */
SkillerPlugin::SkillerPlugin()
  : Plugin("skiller")
{
  __liaison_exec_barrier = new Barrier(2);
  thread_list.push_back(new SkillerLiaisonThread(__liaison_exec_barrier));
  thread_list.push_back(new SkillerExecutionThread(__liaison_exec_barrier));
}


/** Destructor. */
SkillerPlugin::~SkillerPlugin()
{
  delete __liaison_exec_barrier;
}


EXPORT_PLUGIN(SkillerPlugin)
