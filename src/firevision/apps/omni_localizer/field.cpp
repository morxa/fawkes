
/***************************************************************************
 *  field.cpp field representation
 *
 *  Created: ???
 *  Copyright  2008  Volker Krause <volker.krause@rwth-aachen.de>
 *
 *  $Id: pipeline_thread.cpp 1049 2008-04-24 22:40:12Z beck $
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

#include <apps/omni_localizer/field.h>
#include <apps/omni_localizer/debug.h>
#include <apps/omni_localizer/utils.h>

#include <fvutils/draw/drawer.h>

#include <blackboard/blackboard.h>
#include <config/config.h>
#include <interfaces/object.h>
#include <utils/math/angle.h>

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

/** @class Field <apps/omni_localizer/field.h>
 * Model of the playing field.
 *
 * @todo Load field parameter from a file?
 *
 * @author Volker Krause
 */

static f_point_t make_point( float x, float y )
{
  f_point_t p;
  p.x = x;
  p.y = y;
  return p;
}

/**
  Initialize field model.
  @param blackboard The blackboard.
  @param config The configuration.
*/
Field::Field( BlackBoard *blackboard, Configuration *config ) :
    mDebugBuffer( 0 ),
    mWidth( 0 ),
    mHeight( 0 ),
    mBlackBoard( blackboard ),
    mWMBallInterface( 0 )
{
  // defaults
  config->set_default_float( "/firevision/omni/localizer/lower_range", 0.25 );
  config->set_default_float( "/firevision/omni/localizer/upper_range", 3.8 );
  config->set_default_float( "/firevision/omni/localizer/ball_position_weight", 0.25 );

  const string fieldFile = config->get_string( "/firevision/omni/localizer/field" );
  const string fieldPath = string( CONFDIR ) + "/firevision/fields/" + fieldFile;
  load( fieldPath.c_str() );

  mLowerRange = config->get_float( "/firevision/omni/localizer/lower_range" );
  mUpperRange = config->get_float( "/firevision/omni/localizer/upper_range" );
  mBallPositionWeight = config->get_float( "/firevision/omni/localizer/ball_position_weight" );

#ifdef DEBUG_SENSOR_WEIGHTS
  field_pos_t pos;
  pos.x = 0.0;
  pos.y = 0.0;
  pos.ori = 0.0;
  dumpSensorProbabilities( pos, "sensor-prob-test-center.dat" );
  pos.x = 3.0;
  dumpSensorProbabilities( pos, "sensor-prob-test-centerarea.dat" );
  pos.x = 5.0;
  dumpSensorProbabilities( pos, "sensor-prob-test-goalarea.dat" );
  pos.x = 6.75;
  dumpSensorProbabilities( pos, "sensor-prob-test-goal.dat" );
  pos.y = 3.0;
  dumpSensorProbabilities( pos, "sensor-prob-test-cornerarea.dat" );
  pos.y = 4.75;
  dumpSensorProbabilities( pos, "sensor-prob-test-corner.dat" );
#endif

  // try to get the ball and obstacle interfaces if they are already there
  try {
    mWMBallInterface = blackboard->open_for_reading<ObjectPositionInterface>( "WM Ball" );
    list<ObjectPositionInterface *> *lst = blackboard->open_all_of_type_for_reading<ObjectPositionInterface>("WM Obstacle");
    for ( list<ObjectPositionInterface *>::const_iterator it = lst->begin(); it != lst->end(); ++it ) {
      mWMObstacleInterfaces.push_back( *it );
      bbil_add_writer_interface( *it );
    }
    delete lst;

  } catch ( Exception &e ) {
    e.append( "Ball/Obstacle interface registration for field model failed." );
    throw;
  }

  bbio_add_interface_create_type("ObjectPositionInterface");
  blackboard->register_observer( this, BlackBoard::BBIO_FLAG_CREATED );
}

/** Destructor. */
Field::~ Field()
{
  mBlackBoard->close( mWMBallInterface );
}

/** Width of the field. */
float Field::fieldWidth() const
{
  return mFieldWidth;
}

/** Height of the field. */
float Field::fieldHeight() const
{
  return mFieldHeight;
}

/** Width of the field including safety boundaries. */
float Field::totalWidth() const
{
  return mTotalWidth;
}

/** Height of the field including safety boundaries. */
float Field::totalHeight() const
{
  return mTotalHeight;
}

/**
  Calculates the weight for a given sensor reading at distance @p sensorDistance
  when a line was expected at @p lineDistance.
  @param lineDistance Expected distance.
  @param sensorDistance Observed distance.
*/
float Field::weightForDistance( float lineDistance, float sensorDistance ) const
{
  const float distance2 = ( sensorDistance - lineDistance ) * ( sensorDistance - lineDistance );
#if 1
  const float sigma = std::max( 1.0f, lineDistance * 0.5f ) * 0.75;
  return expf( - distance2/(sigma * sigma) ) / (sigma  * std::max( 1.5f, lineDistance * 0.5f ));
#else
  const float sigma = 0.75;
  return expf( - distance2/(sigma * sigma) ) / sigma;
#endif
}

/**
  Calculates the weight for a given obstacle sensor reading @p seenObs when obstacle
  @p expectedObs is expected.
  @param pos The current position.
  @param expectedObs The expected obstacle, in global cartesian coordinates.
  @param seenObs The detected obstacle, in global cartesian coordinates.
*/
float Field::weightForObstacle( const field_pos_t &pos, const obstacle_t & expectedObs, const obstacle_t & seenObs)
{
  const float globalDiffX = pos.x - expectedObs.x;
  const float globalDiffY = pos.y - expectedObs.y;
  const float globalDist = sqrtf( globalDiffX * globalDiffX + globalDiffY * globalDiffY );
  if ( globalDist > mUpperRange ) // ignore lower_range since we might be the obstacle ourselves
    return 0.0;

  const float diffX = expectedObs.x - seenObs.x;
  const float diffY = expectedObs.y - seenObs.y;
  const float dist = sqrtf( diffX * diffX + diffY * diffY );

  // ### TODO
  const float sigma = expectedObs.extent * globalDist;
  return expf( - ((dist * dist) / (sigma * sigma)) ) / sigma;
}

#define mapToImageX(x) ((unsigned int)(scale * ((x) + totalWidth()/2.0)))
#define mapToImageY(y) ((unsigned int)(scale * ((y) + totalHeight()/2.0)))

/**
  Finds all radi that intersect with a field line begin on position @p position
  looking at @p phi.
  @param position The position.
  @param phi The angle we are looking at.
*/
vector< float > Field::findIntersections(const field_pos_t & position, float phi)
{
#ifdef DEBUG_MCL_UPDATE
  Drawer debugDrawer;
  const float scale = mWidth / totalWidth();
  if ( mDebugBuffer )
    debugDrawer.setBuffer( mDebugBuffer, mWidth, mHeight );
#endif

  vector<float> rv;

  typedef std::vector< std::pair<f_point_t, f_point_t> >::const_iterator LineIterator;
  const float b1 = cosf( position.ori + phi );
  const float b2 = sinf( position.ori + phi );
  for ( LineIterator it = mLines.begin(); it != mLines.end(); ++it ) {
    const float a1 = (*it).second.x - (*it).first.x;
    const float a2 = (*it).second.y - (*it).first.y;
    const float c1 = position.x - (*it).first.x;
    const float c2 = position.y - (*it).first.y;
    float g = 0.0, r = 0.0;
    if ( a1 == 0.0 ) {
      r = - (c1 / b1);
      g = (c2 + (r*b2)) / a2;
    } else if ( a2 == 0.0 ) {
      r = -(c2 / b2);
      g = (c1 + (r*b1)) / a1;
    } else {
      // FIXME: this only works for horizontal or vertical lines!
      cout << "Not implemented! " << a1 << " " << a2 << endl;
    }

    if ( g >= 0.0 && g <= 1.0 && r > mLowerRange && r <= mUpperRange ) {
#ifdef DEBUG_MCL_UPDATE
      if ( mDebugBuffer ) {
        debugDrawer.setColor( 128, 0, 255 );
        debugDrawer.drawCircle( mapToImageX(position.x + (r*b1)), mapToImageY(position.y + (r*b2)), 3 );
      }
#endif
      rv.push_back( r );
    }
  }

  for ( vector<arc_t>::const_iterator it = mArcs.begin(); it != mArcs.end(); ++it ) {
    const float a1 = position.x - (*it).m.x;
    const float a2 = position.y - (*it).m.y;

    const float t = (b1 * b1) + (b2 * b2);
    const float p = ((a1 * b1) + (a2 * b2)) / t;
    const float q = ((a1 * a1) + (a2 * a2) - ((*it).r * (*it).r))/t;

    float w = (p*p) - q;
    if ( w < 0 )
      continue;
    w = sqrt( w );
    const float s1 = - p + w;
    const float s2 = - p - w;

    if ( s1 > mLowerRange && s1 <= mUpperRange ) {
      float phi = atan2f( ((s1*b2) + a2)/(*it).r, ((s1*b1) + a1)/(*it).r );
      if ( phi < 0 ) phi += 2.0f * M_PI;
      if ( phi >= (*it).left && phi <= (*it).right ) {
        rv.push_back( s1 );
#ifdef DEBUG_MCL_UPDATE
        if ( mDebugBuffer ) {
          debugDrawer.setColor( 128, 128, 128 );
          debugDrawer.drawLine( mapToImageX(position.x), mapToImageY(position.y),
                                mapToImageX(position.x + (s1*b1)),
                                mapToImageY(position.y + (s1*b2)) );
          debugDrawer.setColor( 255, 255, 255 );
          debugDrawer.drawCircle( mapToImageX(position.x + (s1*b1)),
                                  mapToImageY(position.y + (s1*b2)), 3 );
        }
#endif
      }
    }

    if ( s2 > mLowerRange && s2 <= mUpperRange ) {
      float phi = atan2f( ((s2*b2) + a2)/(*it).r, ((s2*b1) + a1)/(*it).r );
      if ( phi < 0 ) phi += 2.0f * M_PI;
      if ( phi >= (*it).left && phi <= (*it).right ) {
        rv.push_back( s2 );
#ifdef DEBUG_MCL_UPDATE
        if ( mDebugBuffer ) {
          debugDrawer.setColor( 128, 128, 128 );
          debugDrawer.drawLine( mapToImageX(position.x), mapToImageY(position.y),
                                mapToImageX(position.x + (s2*b1)),
                                mapToImageY(position.y + (s2*b2)) );
          debugDrawer.setColor( 255, 255, 255 );
          debugDrawer.drawCircle( mapToImageX(position.x + (s2*b1)),
                                  mapToImageY(position.y + (s2*b2)), 3 );
        }
#endif
      }
    }
  }

  return rv;
}

/**
  Set debug buffer.
  @param buffer The buffer.
  @param width Buffer width.
  @param height Buffer height.
*/
void Field::setDebugBuffer(unsigned char * buffer, unsigned int width, unsigned int height)
{
  mDebugBuffer = buffer;
  mWidth = width;
  mHeight = height;
}

/** Draw field into debug buffer. */
void Field::drawField()
{
  if ( !mDebugBuffer )
    return;

  Drawer drawer;
  drawer.setBuffer( mDebugBuffer, mWidth, mHeight );
  drawer.setColor( 255, 128, 128 );
  const float scale = mWidth / totalWidth();

  typedef std::vector< std::pair<f_point_t, f_point_t> >::const_iterator LineIterator;
  for ( LineIterator it = mLines.begin(); it != mLines.end(); ++it ) {
    drawer.drawLine( mapToImageX((*it).first.x), mapToImageY((*it).first.y),
                     mapToImageX((*it).second.x), mapToImageY((*it).second.y) );
  }

  for ( vector<arc_t>::const_iterator it = mArcs.begin(); it != mArcs.end(); ++it ) {
    for ( float phi = (*it).left; phi < (*it).right; phi += 0.01 ) {
      drawer.drawPoint( mapToImageX( (cos(phi)*(*it).r) + (*it).m.x ),
                        mapToImageY( (sin(phi)*(*it).r) + (*it).m.y ) );
    }
  }

  if ( mWMBallInterface && mWMBallInterface->has_writer() ) {
    mWMBallInterface->read();
    drawer.setColor( 128, 0, 192 );
    float radius = 0.2f;
    if ( mWMBallInterface->dbs_covariance() )
      radius = mWMBallInterface->dbs_covariance()[4];
    radius = max( radius, 0.1f );
    drawer.drawCircle( mapToImageX( mWMBallInterface->world_x() ),
                       mapToImageY( mWMBallInterface->world_y() ),
                       (unsigned int)(scale * radius) );
  } else {
    cout << "No active WM Ball interface!" << endl;
  }
}

/**
 Load field description from file.
 @param filename The filename.
*/
void Field::load(const char * filename)
{
  mLines.clear();
  mArcs.clear();

  ifstream s( filename );
  while ( !s.eof() ) {
    string type;
    s >> type;
    if ( type == "field" ) {
      s >> mTotalWidth >> mTotalHeight >> mFieldWidth >> mFieldHeight;
    } else if ( type == "line" ) {
      float x1, y1, x2, y2;
      s >> x1 >> y1 >> x2 >> y2;
      mLines.push_back( make_pair( make_point( x1, y1 ), make_point( x2, y2 ) ) );
    } else if ( type == "arc" ) {
      arc_t arc;
      s >> arc.m.x >> arc.m.y >> arc.r >> arc.left >> arc.right;
      mArcs.push_back( arc );
    }
  }
}

/**
  Store field description in file.
  @param filename The filename.
*/
void Field::save(const char * filename)
{
  ofstream s( filename );
  s << "field " << totalWidth() << " " << totalHeight() << " " << fieldWidth() << " " << fieldHeight() << endl;
  typedef vector< pair< f_point_t, f_point_t > >::const_iterator LineIterator;
  for ( LineIterator it = mLines.begin(); it != mLines.end(); ++it ) {
    s << "line " << (*it).first.x << " " << (*it).first.y << " " << (*it).second.x << " " << (*it).second.y << endl;
  }
  for ( vector<arc_t>::const_iterator it = mArcs.begin(); it != mArcs.end(); ++it ) {
    s << "arc " << (*it).m.x << " " << (*it).m.y << " " << (*it).r << " " << (*it).left << " " << (*it).right << endl;
  }
}

/**
  Write gnuplot file with sensor weights for the given position for debugging.
  @param position The position.
  @param filename The target file.
  @param filenameObs The target file for obstacle/ball probablility dumps.
*/
void Field::dumpSensorProbabilities( const field_pos_t & position, const char * filename,
                                     const char* filenameObs )
{
  ofstream dbg( filename );
  ofstream dbgObs;
  if ( filenameObs );
    dbgObs.open( filenameObs );
  for ( float x = -totalWidth()/2.0; x < totalWidth()/2.0; x += 0.1 ) {
    for ( float y = -totalHeight()/2.0; y < totalHeight()/2.0; y += 0.1 ) {
      const float phi = atan2( y - position.y, x - position.x );
      const float r = (x - position.x) / cos( phi );
      polar_coord_t sensor;
      sensor.r = r;
      sensor.phi = phi;
      vector<float> expectedHits = findIntersections( position, phi );
      if ( expectedHits.size() > 0 ) {
        sort( expectedHits.begin(), expectedHits.end() );
        const float expectedDist = find_closest( expectedHits.begin(), expectedHits.end(), r );
        dbg << x << " " << y << " " << weightForDistance( expectedDist, r ) << endl;
      } else {
        dbg << x << " " << y << " " << 0.0 << endl;
      }

      if ( dbgObs.is_open() ) {
        vector<obstacle_t> obs = obstacles();
        float maxObsWeight = 0.0;
        obstacle_t seenObs;
        seenObs.x = x;
        seenObs.y = y;
        seenObs.extent = 0.1;
        // TODO covariance!?
        for ( vector<obstacle_t>::const_iterator it = obs.begin(); it != obs.end(); ++it ) {
          maxObsWeight = std::max( maxObsWeight, weightForObstacle( position, *it, seenObs ) );
        }
        f_point_t seenBall;
        seenBall.x = x -position.x;
        seenBall.y = y - position.y;
        const float ballWeight = weightForBall( position, seenBall );
        dbgObs << x << " " << y << " " << (maxObsWeight + ballWeight) << endl;
      }
    }
    dbg << endl;
    if ( dbgObs.is_open() )
      dbgObs << endl;
  }
}

/**
  Returns the weight for the given position based on the given ball sighting.
  @param position The current position.
  @param ballHit Relative ball coordinates.
*/
float Field::weightForBall(const field_pos_t & position, const f_point_t & ballHit)
{
  if ( !mWMBallInterface || !mWMBallInterface->is_visible() )
    return 0.0;
  const float absX = position.x + ( cosf(position.ori) * ballHit.x ) - ( sinf(position.ori) * ballHit.y );
  const float absY = position.y + ( sinf(position.ori) * ballHit.x ) + ( cosf(position.ori) * ballHit.y );
  const float diffX = absX - mWMBallInterface->world_x();
  const float diffY = absY - mWMBallInterface->world_y();
  const float distToBall = sqrtf( diffX * diffX + diffY * diffY );
  // ### TODO
  const float sigma = 0.5;
  return expf( - ((distToBall * distToBall) / (sigma * sigma)) ) * mBallPositionWeight;
}

/**
  Update dynamic objects on the field (ball, obstacles, etc.)
*/
void Field::updateDynamicObjects()
{
  mWMBallInterface->read();

  mInterfaceMutex.lock();
  mObstacles.clear();
  for ( vector<ObjectPositionInterface*>::const_iterator it = mWMObstacleInterfaces.begin();
        it != mWMObstacleInterfaces.end(); ++it )
  {
    (*it)->read();
    if ( !(*it)->is_visible() )
      continue;
    obstacle_t obs;
    obs.x = (*it)->world_x();
    obs.y = (*it)->world_y();
    obs.extent = (*it)->extent_x();
    obs.covariance = (*it)->world_xyz_covariance();
    mObstacles.push_back( obs );
  }
  mInterfaceMutex.unlock();
}

void Field::bb_interface_created(const char * type, const char * id) throw(  )
{
  if ( strncmp( id, "WM Obstacle", strlen("WM Obstacle") ) == 0 ) {
    try {
      ObjectPositionInterface *iface = mBlackBoard->open_for_reading<ObjectPositionInterface>( id );
      mInterfaceMutex.lock();
      mWMObstacleInterfaces.push_back( iface );
      mInterfaceMutex.unlock();
      bbil_add_writer_interface( iface );
    } catch ( Exception &e ) {
      cout << "Opening interface " << id << " failed: " << e.what() << endl;
    }
  }
}

void Field::bb_interface_writer_removed(Interface * interface, unsigned int instance_serial) throw(  )
{
  vector<ObjectPositionInterface*>::iterator it = find( mWMObstacleInterfaces.begin(), mWMObstacleInterfaces.end(), interface );
  if ( it != mWMObstacleInterfaces.end() ) {
    mInterfaceMutex.lock();
    mWMObstacleInterfaces.erase( it );
    mInterfaceMutex.unlock();
  }
}

/**
  Returns all currently known obstacles (in global cartesian coordinates).
*/
std::vector< obstacle_t > Field::obstacles() const
{
  return mObstacles;
}
