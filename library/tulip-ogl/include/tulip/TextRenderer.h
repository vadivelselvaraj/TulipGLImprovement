#ifndef _TEXTRENDERER_H_
#define _TEXTRENDERER_H_
#ifndef DOXYGEN_NOTFOR_DEVEL

#include <string>
#include <libxml/parser.h>
#include <tulip/tulipconf.h>
#include <tulip/Context.h>
#include <tulip/Fonts.h>

enum TextMode{XML,VERBATIM};

namespace tlp {

enum LabelPosition {ON_CENTER = 0, ON_TOP = 1, ON_BOTTOM = 2, ON_LEFT = 3, ON_RIGHT = 4};

class Paragraph;
class Document;
class Parser;


class TLP_GL_SCOPE TextRenderer {
  
 public:
  TextRenderer();
  ~TextRenderer();
  void draw(float w_max, float& w, int relPos) const;
  void getBoundingBox(float w_max, float& h, float& w) const;
  void setString(const std::string &str, TextMode mode);
  void setContext(const std::string &font, int size, unsigned char r, unsigned char v, unsigned char b);
  void setColor(unsigned char r, unsigned char g, unsigned char b);
  void setMode(FontMode  m);

 private:
  Document* doc;
  Context c;
  void initTextManager(const std::string & str);
  Paragraph* textXMLManager(Document* d, Paragraph* f, char* text);
  void initTextXMLManager(Parser* P, xmlNodePtr courant, Document* d);
  void finalTextXMLManager(Paragraph* f);
};

}
#endif //DOXYGEN_NOTFOR_DEVEL
#endif
