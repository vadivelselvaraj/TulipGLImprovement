#include <string>

#include <tulip/tulipconf.h>

namespace tlp {
class PluginProgress;
}

class yajl_gen_t;

class QIODevice;

/**
 * @brief A Simple C++ wrapper around the C library 'yajl' parsing capabilities.
 *
 * It uses callbacks to signal what is being parsed (map start and end, strings, ...), and this class is intended to be subclassed,
 * with the subclass overriding the callbacks to process the events.
 **/
class TLP_SCOPE YajlParseFacade {
public:
  YajlParseFacade(tlp::PluginProgress* progress=NULL) : _progress(progress), _parsingSucceeded(true) {}

  virtual ~YajlParseFacade() {
  }
  /**
   * @brief Parses a JSON file.
   * Once this function is called, the callbacks (all the parse* functions) will get called when the corresponding event happens.
   *
   * @param filename The file to parse.
   * @return void
   **/
  void parse(std::string filename);
  void parse(QIODevice* stream);

  virtual void parseNull();
  virtual void parseBoolean(bool boolVal);
  virtual void parseInteger(long long integerVal);
  virtual void parseDouble(double doubleVal);
  virtual void parseNumber(const char * numberVal, size_t numberLen);
  virtual void parseString(const std::string& value);
  virtual void parseMapKey(const std::string& value);
  virtual void parseStartMap();
  virtual void parseEndMap();
  virtual void parseStartArray();
  virtual void parseEndArray();

  bool parsingSucceeded() const;
  std::string errorMessage() const;

protected:
  tlp::PluginProgress* _progress;
  bool _parsingSucceeded;
  std::string _errorMessage;
};

class YajlWriteFacade {
public:
  YajlWriteFacade();
  ~YajlWriteFacade();

  std::string generatedString();

  void writeInteger(long long int number);
  void writeDouble(double number);
  void writeNumber(const char* str, size_t len);
  void writeString(const std::string& text);
  void writeNull();
  void writeBool(bool value);
  void writeMapOpen();
  void writeMapClose();
  void writeArrayOpen();
  void writeArrayClose();
protected:
  yajl_gen_t* _generator;
};
