#include "drawingdisplay.h"

DrawingDisplay::DrawingDisplay(DrawingCanvas *scene, DrawingInfo *info):
		QGraphicsView(scene),
		drawingInfo(info),
		canvas(scene)
{
	QGraphicsView::setBackgroundBrush(QBrush(QColor(255,255,255))); // HPS - removes crazy black line for some reason
	QGraphicsView::setStyleSheet("QGraphicsView {border: 2px solid #A0A0A0}");
}

void DrawingDisplay::resizeEvent(QResizeEvent *event)
{
	drawingInfo->setHeight(event->size().height());
	drawingInfo->setWidth(event->size().width());
	drawingInfo->determineScaleFactor();
	canvas->setSceneRect(0, 0, event->size().width(), event->size().height());
	canvas->refresh();
}


void DrawingDisplay::focusOutEvent(QFocusEvent *event)
{
	if(event->reason() != Qt::TabFocusReason){
		QGraphicsView::focusOutEvent(event);
	}else{
		QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
		QGraphicsView::keyPressEvent(newEvent);
	}
}
