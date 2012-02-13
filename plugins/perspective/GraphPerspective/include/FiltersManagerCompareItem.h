#ifndef FILTERSMANAGERCOMPAREITEM_H
#define FILTERSMANAGERCOMPAREITEM_H

#include "AbstractFiltersManagerItem.h"

#include <string>
#include <QtCore/QVector>

class QComboBox;

namespace tlp {
class PropertyInterface;
}

namespace Ui {
class FiltersManagerCompareItem;
}

#define COMPARER(N) struct N##Comparer: public Comparer { virtual bool compare(const std::string& a, const std::string& b); virtual bool compare(double a, double b); }
class FiltersManagerCompareItem: public AbstractFiltersManagerItem {
  Q_OBJECT
  Ui::FiltersManagerCompareItem* _ui;

  struct Comparer {
    virtual bool compare(const std::string& a, const std::string& b)=0;
    virtual bool compare(double a, double b)=0;
  };
  COMPARER(Equal);
  COMPARER(Different);
  COMPARER(Lesser);
  COMPARER(LesserEqual);
  COMPARER(Greater);
  COMPARER(GreaterEqual);
  COMPARER(Contains);
  COMPARER(Matches);
  static QVector<Comparer*> COMPARERS;
  static int STRING_ALGORITHM_ID;
  static int DOUBLE_ALGORITHM_ID;

  static QVector<int> NUMERIC_OPERATOR_INDEXES;
  static QVector<int> STRING_OPERATOR_INDEXES;
  static int CUSTOM_VALUE_CHOICE_INDEX;

  QMap<QComboBox*,QTableView*> comboParams;


  enum ComboElementType {
    E_Property,
    E_DoubleAlgorithm,
    E_StringAlgorithm,
    E_CustomValue,
    E_Unknown
  };
  void fillCombo(QComboBox*);
  ComboElementType comboElementType(QComboBox*) const;
  tlp::PropertyInterface* comboProperty(QComboBox*) const;
  tlp::ParameterList comboAlgorithmParams(QComboBox*) const;
  bool isComparisonNumeric(QComboBox*) const;
  bool isComboAlgorithm(QComboBox*) const;

public:
  explicit FiltersManagerCompareItem(QWidget* parent = NULL);
  void applyFilter(tlp::BooleanProperty*);
  QString title() const;

protected:
  void graphChanged();

protected slots:
  void elementChanged();
  void setNumerics(bool);
};

#endif // FILTERSMANAGERCOMPAREITEM_H
