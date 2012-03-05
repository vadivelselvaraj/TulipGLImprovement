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
#include <ogdf/upward/VisibilityLayout.h>
#include "tulip2ogdf/OGDFLayoutPluginBase.h"

// comments below have been extracted from OGDF/src/upward/VisibilityLayout.cpp
/*@{*/
/** \file
 * \brief Implementation of visibility layout algorithm.
 *
 * \author Hoi-Ming Wong and Carsten Gutwenger
 *
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 *
 * Copyright (C). All rights reserved.
 * See README.txt in the root directory of the OGDF installation for details.
 *
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation
 * and appearing in the files LICENSE_GPL_v2.txt and
 * LICENSE_GPL_v3.txt included in the packaging of this file.
 *
 * \par
 * In addition, as a special exception, you have permission to link
 * this software with the libraries of the COIN-OR Osi project
 * (http://www.coin-or.org/projects/Osi.xml), all libraries required
 * by Osi, and all LP-solver libraries directly supported by the
 * COIN-OR Osi project, and distribute executables, as long as
 * you follow the requirements of the GNU General Public License
 * in regard to all of the software in the executable aside from these
 * third-party libraries.
 *
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * \par
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * \see  http://www.gnu.org/copyleft/gpl.html
 ***************************************************************/

namespace {

const char * paramHelp[] = {
  HTML_HELP_OPEN()
  HTML_HELP_DEF( "type", "int" )
  HTML_HELP_BODY()
  "Minimum grid distance."
  HTML_HELP_CLOSE(),
  HTML_HELP_OPEN()
  HTML_HELP_DEF( "type", "bool" )
  HTML_HELP_BODY()
  "If true, the layout is transposed vertically ."
  HTML_HELP_CLOSE()
};
}

class OGDFVisibility : public OGDFLayoutPluginBase {

public:
  PLUGININFORMATIONSWITHGROUP("Visibility (OGDF)","Hoi-Ming Wong","12/11/2007","Ok","1.0","Hierarchical")
  OGDFVisibility(const tlp::PluginContext* context) :OGDFLayoutPluginBase(context, new ogdf::VisibilityLayout()) {
    addParameter<int>("minimum grid distance", paramHelp[0], "1");
    addParameter<bool>("transpose", paramHelp[1], "false");
  }

  ~OGDFVisibility() {}

  void beforeCall(TulipToOGDF*, ogdf::LayoutModule *ogdfLayoutAlgo) {
    ogdf::VisibilityLayout *visibility = static_cast<ogdf::VisibilityLayout*>(ogdfLayoutAlgo);

    if (dataSet != 0) {
      int ival = 0;

      if (dataSet->get("minimum grid distance", ival))
        visibility->setMinGridDistance(ival);
    }
  }

  void afterCall(TulipToOGDF*, ogdf::LayoutModule*) {
    if (dataSet != 0) {
      bool bval = false;

      if (dataSet->get("transpose", bval)) {
        if (bval) {
          transposeLayoutVertically();
        }
      }
    }
  }

};

PLUGIN(OGDFVisibility)
