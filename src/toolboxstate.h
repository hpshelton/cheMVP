#ifndef TOOLBOXSTATE_H
#define TOOLBOXSTATE_H

#include <QMap>
#include "drawingInfo.h"
#include "atom.h"

class ToolBoxState
{
private:
	QMap<QString, QString>* _options;

public:
	ToolBoxState();
	ToolBoxState(QMap<QString, QString>* options) { _options = options; };
	static ToolBoxState* defaultToolBoxState();
	QMap<QString, QString>* options() { return _options; };

	void setUseFogging(bool b);
	void setFoggingScale(int i);
	void setXRotation(int i);
	void setYRotation(int i);
	void setZRotation(int i);
	void setBackgroundOpacity(int i);
	void setZoom(int i);
};

#endif // TOOLBOXSTATE_H
