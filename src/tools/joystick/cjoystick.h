
/***************************************************************************
 *  cjoystick.h - Joystick interface
 *
 *  Generated: Sat Jun 02 17:31:27 2007
 *  Copyright  2007  Nils Springob <nils.springob@crazy-idea.de>
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
 
#ifndef __TOOLS_JOYSTICK_JOYSTICK_H_
#define __TOOLS_JOYSTICK_JOYSTICK_H_

#include <vector>
#include <string>

class CJoystick
{

 public:

  CJoystick(const std::string & dev_name = "/dev/js0");

  ~CJoystick();
  

  int Update();

  int getError() const;

  unsigned int getNumAxes() const;

  unsigned int getNumButtons() const;

  const std::string &getDeviceName() const;

  int getAxisInt(unsigned int nr) const;

  float getAxis(unsigned int nr) const;

  int getAxisSig(unsigned int nr) const;

  float getX(unsigned int poffset=0) const;

  float getY(unsigned int poffset=0) const;

  bool getButton(unsigned int nr) const;
  
  
 private:
 
  std::vector<bool> m_Buttons;
  
  std::vector<int> m_AxesInt;
  
  std::string m_DevName;
  
  int m_FD;
  
  int m_Error;

};

#endif
