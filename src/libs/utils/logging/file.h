
/***************************************************************************
 *  file.h - Fawkes file logger
 *
 *  Created: Tue Jan 16 16:47:03 2007
 *  Copyright  2006-2007  Tim Niemueller [www.niemueller.de]
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

#ifndef __LOGGING_FILE_H_
#define __LOGGING_FILE_H_

#include <utils/logging/logger.h>
#include <utils/system/file.h>

class FileLogger : public Logger
{
 public:
  FileLogger(char* filename);
  virtual ~FileLogger();

  virtual void log_debug(const char *component, const char *format, ...);
  virtual void log_info(const char *component, const char *format, ...);
  virtual void log_warn(const char *component, const char *format, ...);
  virtual void log_error(const char *component, const char *format, ...);

  virtual void vlog_debug(const char *component, const char *format, va_list va);
  virtual void vlog_info(const char *component, const char *format, va_list va);
  virtual void vlog_warn(const char *component, const char *format, va_list va);
  virtual void vlog_error(const char *component, const char *format, va_list va);

  virtual void log_debug(const char *component, Exception &e);
  virtual void log_info(const char *component, Exception &e);
  virtual void log_warn(const char *component, Exception &e);
  virtual void log_error(const char *component, Exception &e);

 private:
  struct timeval *now;
  struct tm *now_s;

  File *log_file;
};

#endif
