
/***************************************************************************
 *  zauberstab.h - Header of class "Zauberstab"
 *                 which offers methods for finding 
 *                 maximal, color-contiguous region
 *                 around a seed pixel
 *
 *  Generated: Mon Jul 02 2005
 *  Copyright  2005       Martin Heracles  <Martin.Heracles@rwth-aachen.de>
 *             2005-2006  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version. A runtime exception applies to
 *  this software (see LICENSE.GPL_WRE file mentioned below for details).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL_WRE file in the doc directory.
 */

#ifndef __FIREVISION_FVUTILS_ZAUBERSTAB_H_
#define __FIREVISION_FVUTILS_ZAUBERSTAB_H_

#include <fvutils/base/types.h>

#include <vector>


/** a "slice" is a row of consecutive pixels
   (horizontal) */
struct ZSlice {
  int leftX;	/**< left X */
  int rightX;	/**< right X */
  int y;	/**< Y value */
};

/** a region is a stack of slices,
   together with the y-position of the slice at the top */
struct ZRegion {
  std::vector<ZSlice*> *slices;	/**< slices */
  int topSliceY;		/**< top slice Y */
};


class Zauberstab {
 public:
  Zauberstab();
  ~Zauberstab();

  void setThreshold(unsigned int t);
  unsigned int getThreshold();
  void setBuffer(unsigned char *b,
		 unsigned int w,
		 unsigned int h);
  void findRegion(int seedX,
		  int seedY);
  void addRegion(int seedX,
		 int seedY);
  void addRegion(ZRegion *region2);
  void deleteRegion();
  bool isEmptyRegion();

  ZRegion * getRegion() const;
  std::vector< rectangle_t >  getSelection();

 private:
  unsigned int threshold;
  ZRegion *region;
  unsigned char *buffer;
  unsigned int width;
  unsigned int height;

  ZSlice* findSlice(int x,
		    int y,
		    unsigned int vSeed);
  bool isSimilarV(unsigned int v1,
		  unsigned int v2);
};


#endif

