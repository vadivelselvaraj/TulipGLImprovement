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
#include <tulip/EdgeExtremityGlyph.h>
#include <tulip/EdgeExtremityGlyphManager.h>
#include <tulip/tuliphash.h>
#include <tulip/ForEach.h>

tlp::EdgeExtremityGlyphManager* tlp::EdgeExtremityGlyphManager::eeinst = 0;

using namespace std;

namespace tlp {
static TLP_HASH_MAP<int, std::string> eeglyphIdToName;
static TLP_HASH_MAP<std::string, int> nameToEeGlyphId;

const int EdgeExtremityGlyphManager::NoEdgeExtremetiesId = UINT_MAX;

EdgeExtremityGlyphManager::EdgeExtremityGlyphManager() {
}
//====================================================
string EdgeExtremityGlyphManager::glyphName(int id) {
  if (id == NoEdgeExtremetiesId) {
    return string("NONE");
  }

  if (eeglyphIdToName.find(id) != eeglyphIdToName.end()) {
    return eeglyphIdToName[id];
  }
  else {
    cerr << __PRETTY_FUNCTION__ << endl;
    cerr << "Invalid glyph id" << endl;
    return string("invalid");
  }
}
//====================================================
int EdgeExtremityGlyphManager::glyphId(string name) {
  if (name.compare("NONE") == 0) {
    return NoEdgeExtremetiesId;
  }

  if (nameToEeGlyphId.find(name) != nameToEeGlyphId.end()) {
    return nameToEeGlyphId[name];
  }
  else {
    cerr << __PRETTY_FUNCTION__ << endl;
    cerr << "Invalid glyph name" << endl;
    return 0;
  }
}
//====================================================
void EdgeExtremityGlyphManager::loadGlyphPlugins() {
  std::list<std::string> plugins = PluginLister::instance()->availablePlugins<Glyph>();
  for(std::list<std::string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
    string pluginName = *it;
    int pluginId = PluginLister::pluginInformations(pluginName)->getId();
    eeglyphIdToName[pluginId] = pluginName;
    nameToEeGlyphId[pluginName] = pluginId;
  }
}
//====================================================
void EdgeExtremityGlyphManager::initGlyphList(Graph **graph,
    GlGraphInputData* glGraphInputData, MutableContainer<
    EdgeExtremityGlyph *>& glyphs) {
  EdgeExtremityGlyphContext gc = EdgeExtremityGlyphContext(graph,
                                 glGraphInputData);
  glyphs.setAll(0);

  
  std::list<std::string> plugins = PluginLister::instance()->availablePlugins<Glyph>();
  for(std::list<std::string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
    string glyphName = *it;
    EdgeExtremityGlyph *newGlyph = PluginLister::instance()->getPluginObject<EdgeExtremityGlyph>(glyphName, &gc);
    glyphs.set(PluginLister::pluginInformations(glyphName)->getId(), newGlyph);
  }
}

void EdgeExtremityGlyphManager::clearGlyphList(Graph **, GlGraphInputData*, MutableContainer<EdgeExtremityGlyph *>& glyphs) {
  std::list<std::string> plugins = PluginLister::instance()->availablePlugins<Glyph>();
  for(std::list<std::string>::const_iterator it = plugins.begin(); it != plugins.end(); ++it) {
    string glyphName = *it;
    delete glyphs.get(PluginLister::pluginInformations(glyphName)->getId());
  }
}

}
