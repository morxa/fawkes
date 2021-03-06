
/***************************************************************************
 *  amcl_laser.h: LASER sensor model for AMCL
 *
 *  Created: Thu May 24 18:49:44 2012
 *  Copyright  2000  Brian Gerkey
 *             2000  Kasper Stoy
 *             2012  Tim Niemueller [www.niemueller.de]
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

/*  From:
 *  Player - One Hell of a Robot Server (LGPL)
 *  Copyright (C) 2000  Brian Gerkey   &  Kasper Stoy
 *                      gerkey@usc.edu    kaspers@robotics.usc.edu
 */
///////////////////////////////////////////////////////////////////////////
// Desc: LASER sensor model for AMCL
// Author: Andrew Howard
// Date: 17 Aug 2003
///////////////////////////////////////////////////////////////////////////

#ifndef AMCL_LASER_H
#define AMCL_LASER_H

#include "amcl_sensor.h"
#include "../map/map.h"

/// @cond EXTERNAL

namespace amcl
{

typedef enum
{
  LASER_MODEL_BEAM,
  LASER_MODEL_LIKELIHOOD_FIELD
} laser_model_t;

// Laser sensor data
class AMCLLaserData : public AMCLSensorData
{
  public:
    AMCLLaserData () {ranges=NULL;};
    virtual ~AMCLLaserData() {delete [] ranges;};
  // Laser range data (range, bearing tuples)
  public: int range_count;
  public: double range_max;
  public: double (*ranges)[2];
};


// Laseretric sensor model
class AMCLLaser : public AMCLSensor
{
  // Default constructor
  public: AMCLLaser(size_t max_beams, map_t* map);

  public: void SetModelBeam(double z_hit,
                            double z_short,
                            double z_max,
                            double z_rand,
                            double sigma_hit,
                            double labda_short,
                            double chi_outlier);

  public: void SetModelLikelihoodField(double z_hit,
                                       double z_rand,
                                       double sigma_hit,
                                       double max_occ_dist);
  
  // Update the filter based on the sensor model.  Returns true if the
  // filter has been updated.
  public: virtual bool UpdateSensor(pf_t *pf, AMCLSensorData *data);

  // Set the laser's pose after construction
  public: void SetLaserPose(pf_vector_t& laser_pose) 
          {this->laser_pose = laser_pose;}

  // Determine the probability for the given pose
  private: static double BeamModel(AMCLLaserData *data, 
                                   pf_sample_set_t* set);
  // Determine the probability for the given pose
  private: static double LikelihoodFieldModel(AMCLLaserData *data, 
                                              pf_sample_set_t* set);

  private: laser_model_t model_type;

  // Current data timestamp
  private: double time;

  // The laser map
  private: map_t *map;

  // Laser offset relative to robot
  private: pf_vector_t laser_pose;
  
  // Max beams to consider
  private: int max_beams;

  // Laser model params
  //
  // Mixture params for the components of the model; must sum to 1
  private: double z_hit;
  private: double z_short;
  private: double z_max;
  private: double z_rand;
  //
  // Stddev of Gaussian model for laser hits.
  private: double sigma_hit;
  // Decay rate of exponential model for short readings.
  private: double lambda_short;
  // Threshold for outlier rejection (unused)
  private: double chi_outlier;
};


}

/// @endcond

#endif
