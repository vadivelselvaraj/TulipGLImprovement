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

#include <GL/glew.h>

#include <cstring>

#include <tulip/GlBezierCurve.h>
#include <tulip/GlCatmullRomCurve.h>
#include <tulip/ParametricCurves.h>

using namespace std;

static const unsigned int CONTROL_POINTS_LIMIT = 120;

static string bezierSpecificVertexShaderSrc =
  "uniform sampler2D pascalTriangleTex;"
  "const int maxControlPoints = 120;"
  "const float pascalTriangleTexStep =  1.0 / float(maxControlPoints-1);"

  "vec3 computeCurvePoint(float t) {"
  "	if (t == 0.0) {"
  "		return getControlPoint(0);"
  "	} else if (t == 1.0) {"
  "		return getControlPoint(nbControlPoints - 1);"
  "	} else {"
  "		float s = (1.0 - t);"
  "		vec3 bezierPoint = vec3(0.0);"
  "		for (int i = 0 ; i < nbControlPoints ; ++i) { "
  "			vec2 pascalTriangleTexIdx = vec2(float(i) * pascalTriangleTexStep, (nbControlPoints-1.0) * pascalTriangleTexStep);"
  "			bezierPoint += getControlPoint(i).xyz * texture2D(pascalTriangleTex, pascalTriangleTexIdx).r * pow(t, float(i)) * pow(s, nbControlPoints - 1.0 - float(i));"
  "		}"
  "		return bezierPoint;"
  "	}"
  "}"

  ;

namespace tlp {

GLuint GlBezierCurve::pascalTriangleTextureId(0);

GlBezierCurve::GlBezierCurve() : AbstractGlCurve("bezier vertex shader", bezierSpecificVertexShaderSrc) {}

GlBezierCurve::GlBezierCurve(const vector<Coord> &controlPoints, const Color &startColor, const Color &endColor,
                             const float &startSize, const float &endSize, const unsigned int nbCurvePoints) :
  AbstractGlCurve("bezier vertex shader", bezierSpecificVertexShaderSrc, controlPoints, startColor, endColor, startSize, endSize, nbCurvePoints) {
}

GlBezierCurve::~GlBezierCurve() {}

void GlBezierCurve::buildPascalTriangleTexture() {
  vector<vector<double> > pascalTriangle;
  buildPascalTriangle(CONTROL_POINTS_LIMIT, pascalTriangle);
  float *pascalTriangleTextureData = new float [CONTROL_POINTS_LIMIT * CONTROL_POINTS_LIMIT];
  memset(pascalTriangleTextureData, 0, CONTROL_POINTS_LIMIT * CONTROL_POINTS_LIMIT * sizeof(float));

  for (unsigned int i = 0 ; i < CONTROL_POINTS_LIMIT ; ++i) {
    for (unsigned int j = 0 ; j <= i ; ++j) {
      pascalTriangleTextureData[i * CONTROL_POINTS_LIMIT + j] = static_cast<float>(pascalTriangle[i][j]);
    }
  }

  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &pascalTriangleTextureId);
  glBindTexture(GL_TEXTURE_2D, pascalTriangleTextureId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE32F_ARB,CONTROL_POINTS_LIMIT,CONTROL_POINTS_LIMIT,0,GL_LUMINANCE,GL_FLOAT,pascalTriangleTextureData);
  glDisable(GL_TEXTURE_2D);

  delete [] pascalTriangleTextureData;
}

void GlBezierCurve::setCurveVertexShaderRenderingSpecificParameters() {
  glActiveTexture(GL_TEXTURE2);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, pascalTriangleTextureId);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  curveShaderProgram->setUniformTextureSampler("pascalTriangleTex", 2);
}

void GlBezierCurve::cleanupAfterCurveVertexShaderRendering() {
  glActiveTexture(GL_TEXTURE2);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
}

void GlBezierCurve::computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints, std::vector<Coord> &curvePoints, unsigned int nbCurvePoints) {
  computeBezierPoints(controlPoints, curvePoints, nbCurvePoints);
}

Coord GlBezierCurve::computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t) {
  return computeBezierPoint(controlPoints, t);
}

void GlBezierCurve::drawCurve(std::vector<Coord> &controlPoints, const Color &startColor, const Color &endColor, const float startSize, const float endSize, const unsigned int nbCurvePoints) {

  static bool floatTextureOk = glewIsSupported("GL_ARB_texture_float") == GL_TRUE;

  if (pascalTriangleTextureId == 0 && floatTextureOk) {
    buildPascalTriangleTexture();
  }

  if (!floatTextureOk) {
    // float texture not supported, forcing CPU rendering
    curveShaderProgramNormal = NULL;
    curveShaderProgramBillboard = NULL;
  }

  if (controlPoints.size() <= CONTROL_POINTS_LIMIT) {
    AbstractGlCurve::drawCurve(controlPoints, startColor, endColor, startSize, endSize, nbCurvePoints);
  }
  else {

    static GlCatmullRomCurve curve;

    const unsigned int nbApproximationPoints = 20;
    vector<Coord> curvePoints;
    computeBezierPoints(controlPoints, curvePoints, nbApproximationPoints);
    curve.setClosedCurve(false);
    curve.setOutlined(outlined);
    curve.setOutlineColor(outlineColor);
    curve.setTexture(texture);
    curve.setBillboardCurve(billboardCurve);
    curve.setLookDir(lookDir);
    curve.drawCurve(curvePoints, startColor, endColor, startSize, endSize, nbCurvePoints);
  }
}

}
