
/***************************************************************************
 *  plugin.cpp - Omni Ball Plugin
 *
 *  Created: Thu July 05 19:00:19 2007
 *  Copyright  2007  Daniel Beck
 *             2007  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#include <apps/omni_ball/plugin.h>
#include <apps/omni_ball/pipeline_thread.h>


/** @class FvOmniBallPlugin <apps/omni_ball/plugin.h>
 * OmniVision Ball Plugin.
 *
 * @author Daniel Beck
 */


/** Constructor.
 * Just adds the pipeline thread to the list of threads
 */
FvOmniBallPlugin::FvOmniBallPlugin()
  : Plugin("fvomniball")
{
  thread_list.push_back( new FvOmniBallPipelineThread() );
}

EXPORT_PLUGIN(FvOmniBallPlugin)

