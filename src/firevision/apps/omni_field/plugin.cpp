
/***************************************************************************
 *  omni_field_plugin.cpp - Omni Field Plugin
 *
 *  Created: Thu Nov 01 17:53:21 2007
 *  Copyright  2007  Daniel Beck
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

#include <apps/omni_field/plugin.h>
#include <apps/omni_field/pipeline_thread.h>


/** @class FvOmniFieldPlugin <apps/omni_field/plugin.h>
 * OmniVision Field Plugin.
 *
 * @author Daniel Beck
 */


/** Constructor.
 * Just adds the pipeline thread to the list of threads
 */
FvOmniFieldPlugin::FvOmniFieldPlugin()
  : Plugin("fvomnifield")
{
  thread_list.push_back( new FvOmniFieldPipelineThread() );
}

EXPORT_PLUGIN(FvOmniFieldPlugin)
