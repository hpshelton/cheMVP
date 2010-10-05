#ifndef TAB_H
#define TAB_H

#include <QHBoxLayout>
#include <QGraphicsView>
#include <QWidget>
#include "drawinginfo.h"
#include "drawingcanvas.h"
#include "drawingdisplay.h"
#include "toolbarstate.h"
#include "toolboxstate.h"

class Tab : public QWidget
{
public:
	DrawingInfo* drawingInfo;
	DrawingCanvas* canvas;
	FileParser* parser;
	QGraphicsView* view;
	QString label;
	QString windowLabel;
	QString currentSaveFile;
	ToolBarState* toolBarState;
	ToolBoxState* toolBoxState;
	QUndoStack* undoStack;

public:
	Tab(FileParser* parser);
	void update();
};

#endif // TAB_H
