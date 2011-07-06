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

#ifndef GLBEZIERCURVE_H_
#define GLBEZIERCURVE_H_

#include <tulip/AbstractGlCurve.h>

namespace tlp {

/**
 * \brief A class to draw Bézier curves
 *
 * This class allows to draw Bézier curves defined by an arbitrary number of control points.
 * Bézier curves are named after their inventor, Dr. Pierre Bézier. He was an engineer with the
 * Renault car company and set out in the early 1960’s to develop a curve formulation which would
 * lend itself to shape design.
 * Bézier curves are widely used in computer graphics to model smooth curves. A Bézier curve is
 * completely contained in the convex hull of its control points and passes through its first and
 * last control points. The curve is also always tangent to the first and last convex hull polygon segments.
 * In addition, the curve shape tends to follow the polygon shape.
 *
 */
class TLP_GL_SCOPE GlBezierCurve : public AbstractGlCurve {

public:

  GlBezierCurve();

  /**
   * GlBezierCurve constructor
   *
   * \param controlPoints a vector of control points (size must be greater or equal to 2)
   * \param startColor the color at the start of the curve
   * \param endColor the color at the end of the curve
   * \param startSize the width at the start of the curve
   * \param endSize the width at the end of the curve
   * \param nbCurvePoints the number of curve points to generate
   */
  GlBezierCurve(const std::vector<Coord> &controlPoints, const Color &startColor, const Color &endColor,
                const float &startSize, const float &endSize, const unsigned int nbCurvePoints = 100);

  ~GlBezierCurve();

  void drawCurve(std::vector<Coord> &controlPoints, const Color &startColor, const Color &endColor, const float startSize, const float endSize, const unsigned int nbCurvePoints=100);

protected :

  Coord computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t);

  void computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints, std::vector<Coord> &curvePoints, unsigned int nbCurvePoints);

  std::string genCurveVertexShaderSpecificCode();

  void setCurveVertexShaderRenderingSpecificParameters();

  void cleanupAfterCurveVertexShaderRendering();

private :

  static void buildPascalTriangleTexture();
  static GLuint pascalTriangleTextureId;

};

}
#endif /* GLBEZIERCURVE_H_ */
