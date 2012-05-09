/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#ifndef Tulip_GLFEEDBACKRECORDER_H
#define Tulip_GLFEEDBACKRECORDER_H

#include <tulip/tulipconf.h>
#include <cstdlib>

#include <tulip/Vector.h>

#include <tulip/GlFeedBackBuilder.h>

namespace tlp {

/** \brief Class use to build an object with an OpenGL feedback buffer
 *
 * Class use to build an object with an OpenGL feedback buffer
 * This class call functions of the GlFeedBackBuilder passed to the constructor
 */
class TLP_GL_SCOPE GlFeedBackRecorder {

public:

  /**
   * Constructor : the recorder use GlFeedBackBuilder : builder
   */
  GlFeedBackRecorder(GlFeedBackBuilder *builder,unsigned int pointSize=7):
    feedBackBuilder(builder),
    pointSize(pointSize) {}
  /**
   * Record a new feedback buffer
   * \param doSort : sort the feedback buffer
   * \param size : size of the feedback buffer
   * \param feedBackBuffer : the feedback buffer
   * \param viewport : the viewport of the scene
   */
  void record(bool doSort, GLint size, GLfloat *feedBackBuffer,const Vector<int,4>& viewport);

private:

  void sortAndRecord(GLint size, GLfloat *feedBackBuffer);
  void record(GLint size, GLfloat *feedBackBuffer);
  GLfloat* recordPrimitive(GLfloat *loc);

  GlFeedBackBuilder *feedBackBuilder;
  unsigned int pointSize;

};

}

#endif // Tulip_GLFEEDBACKRECORDER_H
