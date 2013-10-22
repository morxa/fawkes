
/***************************************************************************
 *  base_motor_instruct.h - Abstract base class for a motor instructor
 *
 *  Created: Fri Oct 18 15:16:23 2013
 *  Copyright  2002  Stefan Jacobs
 *             2013  Bahram Maleki-Fard
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

#ifndef __PLUGINS_COLLI_DRIVE_REALIZATION_BASE_MOTORINSTRUCT_H_
#define __PLUGINS_COLLI_DRIVE_REALIZATION_BASE_MOTORINSTRUCT_H_

#include "../utils/rob/robo_motorcontrol.h"

#include <logging/logger.h>
#include <utils/time/time.h>

#include <cmath>

namespace fawkes
{
#if 0 /* just to make Emacs auto-indent happy */
}
#endif

class MotorInterface;

/** @class CBaseMotorInstruct <plugins/colli/drive_realization/base_motor_instruct.h>
 * The Basic of a Motorinstructor.
 */

class CBaseMotorInstruct: public MotorControl
{
 public:

  CBaseMotorInstruct( fawkes::MotorInterface* motor,
                      float frequency,
                      fawkes::Logger* logger);

  virtual ~CBaseMotorInstruct();


  ///\brief Try to realize the proposed values with respect to the maximum allowed values.
  void Drive( float proposedTrans, float proposedRot );

  ///\brief Executes a soft stop with respect to CalculateTranslation and CalculateRotation.
  void ExecuteStop( );


 protected:
  fawkes::Logger* logger_; /**< The fawkes logger */


 private:

  float m_execTranslation, m_execRotation;
  float m_desiredTranslation, m_desiredRotation;
  float m_currentTranslation, m_currentRotation;
  float m_Frequency;

  fawkes::Time m_OldTimestamp;


  ///\brief setCommand sets the executable commands and sends them
  void SetCommand( );


  /** calculates rotation speed
   * has to be implemented in its base classes!
   * is for the smoothness of rotation transitions.
   * calculate maximum allowed rotation change between proposed and desired values
   */
  virtual float CalculateRotation( float currentRotation, float desiredRotation,
           float time_factor ) = 0;


  /** calculates translation speed.
   * has to be implemented in its base classes!
   * is for the smoothness of translation transitions.
   * calculate maximum allowed translation change between proposed and desired values
   */
  virtual float CalculateTranslation( float currentTranslation, float desiredTranslation,
              float time_factor ) = 0;
};



/* ************************************************************************** */
/* ********************  BASE IMPLEMENTATION DETAILS  *********************** */
/* ************************************************************************** */

/** Constructor. Initializes all constants and the local pointers.
 * @param motor The MotorInterface with all the motor information
 * @param frequency The frequency of the colli (should become deprecated!)
 * @param logger The fawkes logger
 */
inline
CBaseMotorInstruct::CBaseMotorInstruct( fawkes::MotorInterface* motor,
                                        float frequency,
                                        fawkes::Logger* logger )
 : MotorControl( motor ),
   logger_(logger)
{
  logger_->log_info("CBaseMotorInstruct", "(Constructor): Entering");
  // init all members, zero, just to be on the save side
  m_desiredTranslation = m_desiredRotation = 0.0;
  m_currentTranslation = m_currentRotation = 0.0;
  m_execTranslation    = m_execRotation    = 0.0;
  m_OldTimestamp.stamp();
  m_Frequency = frequency;
  logger_->log_info("CBaseMotorInstruct", "(Constructor): Exiting");
}


/** Desctructor. */
inline
CBaseMotorInstruct::~CBaseMotorInstruct()
{
  logger_->log_info("CBaseMotorInstruct", "(Destructor): Entering");
  logger_->log_info("CBaseMotorInstruct", "(Destructor): Exiting");
}


/** Sends the drive command to the motor. */
inline void
CBaseMotorInstruct::SetCommand()
{
  // This case here should be removed after Alex's RPC does work.
  // SJ TODO!!!
  if ( !(GetMovingAllowed()) )
    SetRecoverEmergencyStop();
  // SJ TODO!!!


  // Translation borders
  if ( fabs(m_execTranslation) < 0.05 )
    SetDesiredTranslation( 0.0 );
  else
    if ( fabs(m_execTranslation) > 3.0 )
      if ( m_execTranslation > 0.0 )
  SetDesiredTranslation( 3.0 );
      else
  SetDesiredTranslation( -3.0 );
    else
      SetDesiredTranslation( m_execTranslation );

  // Rotation borders
  if ( fabs(m_execRotation) < 0.01 )
    SetDesiredRotation( 0.0 );
  else
    if ( fabs(m_execRotation) > 2*M_PI )
      if ( m_execRotation > 0.0 )
  SetDesiredRotation( 2*M_PI );
      else
  SetDesiredRotation( -2*M_PI );
    else
      SetDesiredRotation( m_execRotation );

  // Send the commands to the motor. No controlling afterwards done!!!!
  SendCommand();
}


/** Try to realize the proposed values with respect to the physical constraints of the robot.
 * @param proposedTrans the proposed translation velocity
 * @param proposedRot the proposed rotation velocity
 */
inline void
CBaseMotorInstruct::Drive( float proposedTrans, float proposedRot )
{
  // initializing driving values (to be on the sure side of life)
  m_execTranslation = 0.0;
  m_execRotation = 0.0;

  // timediff storie to realize how often one was called
  Time currentTime;
  currentTime.stamp();
  float timediff = (currentTime - m_OldTimestamp).in_sec();
  float time_factor = ( (timediff*1000.0) / m_Frequency);

  if (time_factor < 0.5) {
    logger_->log_debug("CBaseMotorInstruct","( Drive ): Blackboard timing(case 1) strange, time_factor is %f", time_factor);
  } else if (time_factor > 2.0) {
    logger_->log_debug("CBaseMotorInstruct", "( Drive ): Blackboard timing(case 2) strange, time_factor is %f", time_factor);
  }

  m_OldTimestamp = currentTime;

  // getting current performed values
  m_currentRotation    = GetMotorDesiredRotation();
  m_currentTranslation = GetMotorDesiredTranslation();

  // calculate maximum allowed rotation change between proposed and desired values
  m_desiredRotation = proposedRot;
  m_execRotation    = CalculateRotation( m_currentRotation, m_desiredRotation, time_factor );

  // calculate maximum allowed translation change between proposed and desired values
  m_desiredTranslation = proposedTrans;
  m_execTranslation    = CalculateTranslation( m_currentTranslation, m_desiredTranslation, time_factor );

  // send the command to the motor
  SetCommand( );
}


/** Executes a soft stop with respect to CalculateTranslation and CalculateRotation
 * if it is called several times
 */
inline void
CBaseMotorInstruct::ExecuteStop()
{
  m_currentTranslation = GetMotorDesiredTranslation();
  m_currentRotation    = GetMotorDesiredRotation();

  // with respect to the physical borders do a stop to 0.0, 0.0.
  Drive( 0.0, 0.0 );
  SetCommand( );
}



} // namespace fawkes

#endif

