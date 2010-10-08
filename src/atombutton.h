#ifndef ATOMBUTTON_H
#define ATOMBUTTON_H

#include <QToolButton>
#include <QPushButton>
#include <QDialog>
#include <QColor>

#include "atom.h"
#include "tab.h"
#include "preferences.h"

class AtomButton : public QToolButton
{
	Q_OBJECT

public:
	AtomButton(Tab* t, const char* label);
	void refreshColor();

public slots:
	void setAtomColor();

protected:
	Tab* _currentTab;
	QString _label;
};

#endif /* ATOMBUTTON_H */
