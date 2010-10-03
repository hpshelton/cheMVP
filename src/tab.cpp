#include "tab.h"

Tab::Tab(FileParser* parser)
{
	this->currentSaveFile = "";
	this->label = "";
	this->windowLabel = "";
	this->toolBarState = new ToolBarState();
	this->undoStack = new QUndoStack();
	this->parser = parser;
	this->drawingInfo = new DrawingInfo();
	this->canvas = new DrawingCanvas(this->drawingInfo, this->parser);

	this->view = new DrawingDisplay(this->canvas, this->drawingInfo);
	this->view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->view->setGeometry(0, 0, static_cast<int>(DEFAULT_SCENE_SIZE_X), static_cast<int>(DEFAULT_SCENE_SIZE_Y));
	this->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	this->drawingInfo->setHeight(this->view->sceneRect().height());
	this->drawingInfo->setWidth(this->view->sceneRect().width());
	this->drawingInfo->determineScaleFactor();
	this->canvas->setSceneRect(this->view->sceneRect());
	this->canvas->refresh();

	QHBoxLayout* tabLayout = new QHBoxLayout;
	tabLayout->setContentsMargins(0,0,0,0);
	tabLayout->addWidget(this->view);

	this->setLayout(tabLayout);
}

void Tab::update()
{
	if(this->label.isEmpty())
	{
		if(this->parser->fileName().isEmpty())
			this->label = "cheMVP";
		else
			this->label = parser->fileName().right(parser->fileName().length() - parser->fileName().lastIndexOf("/") - 1);
	}

	if(this->windowLabel.isEmpty())
	{
		if(this->parser->fileName().isEmpty())
			this->windowLabel = "cheMVP";
		else
			this->windowLabel = parser->fileName() + " - cheMVP";
	}

	if(this->layout()->indexOf(this->view) != -1)
		this->layout()->removeWidget(this->view);

	this->view = new DrawingDisplay(this->canvas, this->drawingInfo);
	this->view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->view->setGeometry(0, 0, static_cast<int>(DEFAULT_SCENE_SIZE_X), static_cast<int>(DEFAULT_SCENE_SIZE_Y));
//	this->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Causes display issues on load
	this->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	this->layout()->addWidget(this->view);
}
