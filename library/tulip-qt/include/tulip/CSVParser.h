/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
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

#ifndef CSVDATALOADER_H_
#define CSVDATALOADER_H_
#include <tulip/tulipconf.h>
#include <string>
#include <vector>
#include <limits.h>

namespace tlp {

class CSVContentHandler;
class PluginProgress;

/*
 * @brief Parse a csv data and send each tokens to the given CSVContentHandler object.
 * Parse a csv data and send each tokens to the given CSVContentHandler object. Get each line of the file and parse them.
 * Send the found tokens to the CSVContentHandler interface.
 * \code
 * CSVParser parser;
 * \/\/Automatically remove quotes.
 * parser.setRemoveQuotes(true);
 * string filename = "fileName.csv";
 * CSVContentHandler * handler ;
 * parser.parse(filename,";",handler);
 * \endcode
 */
class TLP_QT_SCOPE CSVParser {
public:
  CSVParser();
  virtual ~CSVParser();

  void setRemoveQuotes(bool removeQuotes) {
    this->removeQuotes = removeQuotes;
  }
  virtual void parse(const std::string& fileName, const std::string& separator,
		     CSVContentHandler * handler,
		     tlp::PluginProgress* progress=NULL);

protected:
  virtual std::string treatToken(const std::string& token, int row, int column);

private:
  void tokenize(const std::string& str, std::vector<std::string>& tokens,
		const std::string& delimiters, unsigned int numberOfCol);
  std::string removeQuotesIfAny(const std::string &s);
  bool removeQuotes;
};
}
#endif /* CSVDATALOADER_H_ */
