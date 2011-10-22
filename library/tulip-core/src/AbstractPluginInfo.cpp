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
#include <tulip/AbstractPluginInfo.h>

using namespace tlp;
using namespace std;

std::string tlp::getMinor(const std::string &release) {
  size_t pos = release.find('.');

  //if there is no minor version number, return a default '0'
  if (pos == std::string::npos)
    return std::string("0");

  unsigned int rpos = release.rfind('.');

  //if there is only one dot, return everything after it
  if (pos == rpos)
    return release.substr(pos+1);

  //if there is more than one dot, return everything between the first and last dots
  return release.substr(pos + 1, rpos - pos - 1);
}

std::string tlp::getMajor(const std::string &release) {
  unsigned int pos = release.find('.');
  return release.substr(0, pos);
}

std::string AbstractPluginInfo::getMajor() const {
  return tlp::getMajor(getRelease());
}

std::string AbstractPluginInfo::getMinor() const {
  return tlp::getMinor(getRelease());
}

std::string AbstractPluginInfo::getTulipMajor() const {
  return tlp::getMajor(getTulipRelease());
}

std::string AbstractPluginInfo::getTulipMinor() const  {
  return tlp::getMinor(getTulipRelease());
}

int AbstractPluginInfo::getId() const {
  return 0;
}
