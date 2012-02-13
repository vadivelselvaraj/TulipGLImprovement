#include "FiltersManagerCompareItem.h"
#include "ui_FiltersManagerCompareItem.h"

#include <QtGui/QStandardItem>
#include <QtGui/QComboBox>
#include <tulip/Graph.h>
#include <tulip/PropertyAlgorithm.h>
#include <tulip/DoubleProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/TulipItemDelegate.h>

using namespace tlp;
using namespace std;

// Fills up a combo box with the list of currently loaded algorithms (template argument is the associated plugin lister)
template<typename LISTER>
void fillAlgorithms(QComboBox* combo, QString title, int id) {
  fillTitle(combo, title);
  string s;
  forEach(s,LISTER::availablePlugins()) {
      combo->addItem(s.c_str(),id);
      lastItem(combo)->setToolTip(s.c_str());
  }
}

// Comparer methods. Associated to the operators listed in the operatorCombo
// Numeric comparers
#define NUM_CMP(N,OP) bool FiltersManagerCompareItem::N##Comparer::compare(const std::string& a, const std::string& b) { return a OP b; }\
  bool FiltersManagerCompareItem::N##Comparer::compare(double a, double b) { return a OP b; }
NUM_CMP(Equal,==)
NUM_CMP(Different,!=)
NUM_CMP(Lesser,<)
NUM_CMP(LesserEqual,<=)
NUM_CMP(Greater,>)
NUM_CMP(GreaterEqual,>=)
// String comparers
bool FiltersManagerCompareItem::ContainsComparer::compare(const string &a, const string &b) { return QString(a.c_str()).contains(b.c_str()); }
bool FiltersManagerCompareItem::ContainsComparer::compare(double, double) { return false; }
bool FiltersManagerCompareItem::MatchesComparer::compare(const string &a, const string &b) { QRegExp regExp(b.c_str()); return regExp.exactMatch(a.c_str()); }
bool FiltersManagerCompareItem::MatchesComparer::compare(double, double) { return false; }

int FiltersManagerCompareItem::DOUBLE_ALGORITHM_ID = 0;
int FiltersManagerCompareItem::STRING_ALGORITHM_ID = 1;

int FiltersManagerCompareItem::CUSTOM_VALUE_CHOICE_INDEX = 0;
QVector<int> FiltersManagerCompareItem::NUMERIC_OPERATOR_INDEXES = QVector<int>() << 2 << 3 << 4 << 5;
QVector<int> FiltersManagerCompareItem::STRING_OPERATOR_INDEXES = QVector<int>() << 6 << 7;

QVector<FiltersManagerCompareItem::Comparer*> FiltersManagerCompareItem::COMPARERS =
    QVector<FiltersManagerCompareItem::Comparer*>()
    << new EqualComparer() << new DifferentComparer()
    << new LesserComparer() << new LesserEqualComparer()
    << new GreaterComparer() << new GreaterEqualComparer()
    << new ContainsComparer() << new MatchesComparer();

void FiltersManagerCompareItem::fillCombo(QComboBox* combo) {
  disconnect(_ui->elem1,SIGNAL(currentIndexChanged(int)),this,SLOT(elementChanged()));
  disconnect(_ui->elem2,SIGNAL(currentIndexChanged(int)),this,SLOT(elementChanged()));
  disconnect(_ui->elem1,SIGNAL(currentIndexChanged(int)),this,SIGNAL(titleChanged()));
  disconnect(_ui->elem2,SIGNAL(currentIndexChanged(int)),this,SIGNAL(titleChanged()));
  disconnect(_ui->customValueEdit,SIGNAL(textChanged(QString)),this,SIGNAL(titleChanged()));

  combo->clear();
  if (combo == _ui->elem2) {
    combo->addItem(trUtf8("Custom value"));
  }
  // Properties
  fillTitle(combo,trUtf8("Properties"));
  if (_graph != NULL) {
    string s;
    forEach(s,_graph->getProperties()) {
      QString name = QString(s.c_str()) + " (" + _graph->getProperty(s)->getTypename().c_str() + ")";
      combo->addItem(name,QVariant::fromValue<tlp::PropertyInterface*>(_graph->getProperty(s)));
      lastItem(combo)->setToolTip(name);
    }
  }
  fillAlgorithms<DoublePluginLister>(combo,trUtf8("Metrics"),DOUBLE_ALGORITHM_ID);
  fillAlgorithms<StringPluginLister>(combo, trUtf8("Labels"),STRING_ALGORITHM_ID);

  connect(_ui->elem1,SIGNAL(currentIndexChanged(int)),this,SLOT(elementChanged()));
  connect(_ui->elem2,SIGNAL(currentIndexChanged(int)),this,SLOT(elementChanged()));
  connect(_ui->elem1,SIGNAL(currentIndexChanged(int)),this,SIGNAL(titleChanged()));
  connect(_ui->elem2,SIGNAL(currentIndexChanged(int)),this,SIGNAL(titleChanged()));
  connect(_ui->customValueEdit,SIGNAL(textChanged(QString)),this,SIGNAL(titleChanged()));

  foreach(QComboBox* combo, QList<QComboBox*>() << _ui->elem1 << _ui->elem2) {
    for (int i=0;i<combo->count();++i) {
      if (itemAt(combo,i)->isSelectable()) {
        combo->setCurrentIndex(i);
        break;
      }
    }
  }
}
FiltersManagerCompareItem::ComboElementType FiltersManagerCompareItem::comboElementType(QComboBox* combo) const {
  if (combo == _ui->elem2 && combo->currentIndex() == CUSTOM_VALUE_CHOICE_INDEX) {
    return E_CustomValue;
  }
  QStandardItem* item = itemAt(combo,combo->currentIndex());
  if (item == NULL)
    return E_Unknown;
  QVariant data = item->data(Qt::UserRole);
  if (data.value<PropertyInterface*>() != NULL)
    return E_Property;
  if (data.toInt() == STRING_ALGORITHM_ID)
    return E_StringAlgorithm;
  return E_DoubleAlgorithm;
}
PropertyInterface* FiltersManagerCompareItem::comboProperty(QComboBox* combo) const {
  QStandardItem* item = itemAt(combo,combo->currentIndex());
  QVariant data = item->data(Qt::UserRole);
  return data.value<tlp::PropertyInterface*>();
}
ParameterList FiltersManagerCompareItem::comboAlgorithmParams(QComboBox* combo) const {
  if (comboElementType(combo) == E_DoubleAlgorithm)
    return DoublePluginLister::getPluginParameters(combo->currentText().toStdString());
  else
    return StringPluginLister::getPluginParameters(combo->currentText().toStdString());
}
bool FiltersManagerCompareItem::isComparisonNumeric(QComboBox* combo) const {
  ComboElementType elType = comboElementType(combo);
  bool isCustomDouble;
  _ui->customValueEdit->text().toDouble(&isCustomDouble);
  return (elType == E_CustomValue && isCustomDouble) ||
         (elType == E_Property && (dynamic_cast<DoubleProperty*>(comboProperty(combo)) != NULL || dynamic_cast<IntegerProperty*>(comboProperty(combo)) != NULL)) ||
         (elType == E_DoubleAlgorithm);
}
bool FiltersManagerCompareItem::isComboAlgorithm(QComboBox* combo) const {
  return comboElementType(combo) == E_DoubleAlgorithm || comboElementType(combo) == E_StringAlgorithm;
}

FiltersManagerCompareItem::FiltersManagerCompareItem(QWidget* parent): AbstractFiltersManagerItem(parent), _ui(new Ui::FiltersManagerCompareItem) {
  _ui->setupUi(this);

  comboParams[_ui->elem1] = _ui->alg1Params;
  comboParams[_ui->elem2] = _ui->alg2Params;
  _ui->alg1Params->hide();
  _ui->alg2Params->hide();
  _ui->alg1Params->setItemDelegate(new TulipItemDelegate);
  _ui->alg2Params->setItemDelegate(new TulipItemDelegate);
}
void FiltersManagerCompareItem::elementChanged() {
  QComboBox* combo = static_cast<QComboBox*>(sender());
  setNumerics(isComparisonNumeric(_ui->elem1) && isComparisonNumeric(_ui->elem2));
  _ui->customValueEdit->setVisible(comboElementType(_ui->elem2) == E_CustomValue);

  foreach(QComboBox* c, QList<QComboBox*>() << _ui->elem1 << _ui->elem2) {
    comboParams[c]->setVisible(isComboAlgorithm(c));
    if (c == combo) {
      if (isComboAlgorithm(c))
        comboParams[c]->setModel(new ParameterListModel(comboAlgorithmParams(c),_graph));
      else
        comboParams[c]->setModel(NULL);
    }
  }
}
void FiltersManagerCompareItem::setNumerics(bool f) {
  foreach(int i,NUMERIC_OPERATOR_INDEXES)
    itemAt(_ui->operatorCombo,i)->setEnabled(f);
  foreach(int i,STRING_OPERATOR_INDEXES)
    itemAt(_ui->operatorCombo,i)->setEnabled(!f);
}
void FiltersManagerCompareItem::graphChanged() {
  fillCombo(_ui->elem1);
  fillCombo(_ui->elem2);
}
QString FiltersManagerCompareItem::title() const {
  return (_ui->selectModeCombo->currentIndex() == 0 ? "+  " : "-  ") +
      _ui->elem1->currentText() + " " +
      _ui->operatorCombo->currentText() + " " +
      (comboElementType(_ui->elem2) == E_CustomValue ? _ui->customValueEdit->text() : _ui->elem2->currentText());
}

class DoublePropertyFacade {
  IntegerProperty* _intProp;
  DoubleProperty* _doubleProp;
public:
  DoublePropertyFacade(PropertyInterface* pi) {
    _intProp = dynamic_cast<IntegerProperty*>(pi);
    _doubleProp = dynamic_cast<DoubleProperty*>(pi);
  }

  double getNodeValue(node n) {
    if (_intProp != NULL) return (double)(_intProp->getNodeValue(n));
    return _doubleProp->getNodeValue(n);
  }
  double getEdgeValue(edge e) {
    if (_intProp != NULL) return (double)(_intProp->getEdgeValue(e));
    return _doubleProp->getEdgeValue(e);
  }
};

struct BooleanTransformer {
  virtual bool tr(bool original, bool mask)=0;
};
struct RemoveBooleanTransformer: public BooleanTransformer {
  bool tr(bool original, bool mask) { return original && !mask; }
};
struct NoopBooleanTransformer: public BooleanTransformer {
  bool tr(bool original, bool mask) { return original || mask; }
};

void FiltersManagerCompareItem::applyFilter(BooleanProperty* out) {
  // Pre-check: if some of the comparison element is undefined, do nothing
  if (comboElementType(_ui->elem1) == E_Unknown || comboElementType(_ui->elem2) == E_Unknown)
    return;

  // First step: convert each comparison element into a property interface
  QVector<PropertyInterface*> comparisonProperties;
  QVector<double> temporaryProperties;
  foreach(QComboBox* combo, QList<QComboBox*>() << _ui->elem1 << _ui->elem2) {
    ComboElementType elType = comboElementType(combo);
    temporaryProperties << (elType != E_Property);
    if (elType == E_Property)
      comparisonProperties << comboProperty(combo);
    else if (elType == E_DoubleAlgorithm) {
      DoubleProperty* prop = new DoubleProperty(_graph);
      string msg;
      _graph->computeProperty<DoubleProperty>(combo->currentText().toStdString(), prop, msg);
      comparisonProperties << prop;
    }
    else if (elType == E_StringAlgorithm) {
      StringProperty* prop = new StringProperty(_graph);
      string msg;
      _graph->computeProperty<StringProperty>(combo->currentText().toStdString(), prop, msg);
      comparisonProperties << prop;
    }
    else if (elType == E_CustomValue) {
      QString customText = _ui->customValueEdit->text();
      bool isDouble;
      double customDouble = customText.toDouble(&isDouble);
      if (isDouble) {
        DoubleProperty* prop = new DoubleProperty(_graph);
        prop->setAllNodeValue(customDouble);
        prop->setAllEdgeValue(customDouble);
        comparisonProperties << prop;
      }
      else {
        StringProperty* prop = new StringProperty(_graph);
        prop->setAllNodeValue(customText.toStdString());
        prop->setAllEdgeValue(customText.toStdString());
        comparisonProperties << prop;
      }
    }
  }

  // We will add or remove from selection using functors
  BooleanTransformer* trans;
  if (_ui->selectModeCombo->currentIndex() == 0)
    trans = new NoopBooleanTransformer;
  else
    trans = new RemoveBooleanTransformer;
  Comparer* cmp = COMPARERS[_ui->operatorCombo->currentIndex()];

  // Check if we are in a numeric comparison or not and apply filter
  if (NUMERIC_OPERATOR_INDEXES.contains(_ui->operatorCombo->currentIndex())) { // Numeric
    DoublePropertyFacade* facade1 = new DoublePropertyFacade(comparisonProperties[0]);
    DoublePropertyFacade* facade2 = new DoublePropertyFacade(comparisonProperties[1]);
    node n;
    forEach(n, _graph->getNodes()) {
      bool res = cmp->compare(facade1->getNodeValue(n),facade2->getNodeValue(n));
      out->setNodeValue(n,trans->tr(out->getNodeValue(n),res));
    }
    edge e;
    forEach(e, _graph->getEdges()) {
      bool res = cmp->compare(facade1->getEdgeValue(e),facade2->getEdgeValue(e));
      out->setEdgeValue(e,trans->tr(out->getEdgeValue(e),res));
    }
  }
  else {
    PropertyInterface* pi1 = comparisonProperties[0];
    PropertyInterface* pi2 = comparisonProperties[1];
    node n;
    forEach(n, _graph->getNodes()) {
      bool res = cmp->compare(pi1->getNodeStringValue(n),pi2->getNodeStringValue(n));
      out->setNodeValue(n,trans->tr(out->getNodeValue(n),res));
    }
    edge e;
    forEach(e, _graph->getEdges()) {
      bool res = cmp->compare(pi1->getEdgeStringValue(e),pi2->getEdgeStringValue(e));
      out->setEdgeValue(e,trans->tr(out->getEdgeValue(e),res));
    }
  }

  for(int i=0;i<temporaryProperties.size();++i) {
    if (temporaryProperties[i])
      delete comparisonProperties[i];
  }
  delete trans;
}
