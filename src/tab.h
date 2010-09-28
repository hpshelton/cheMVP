#include <QHBoxLayout>
#include <QGraphicsView>
#include <QWidget>
#include "drawinginfo.h"
#include "drawingcanvas.h"
#include "drawingdisplay.h"

#ifndef TAB_H
#define TAB_H

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

public:
	Tab(FileParser* parser);
	void update();
};

#endif // TAB_H
