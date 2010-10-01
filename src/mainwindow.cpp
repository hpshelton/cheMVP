#include <QtGui>

#include <iostream>
#include <iomanip>
#include <assert.h>
#include "mainwindow.h"
#include "drawingdisplay.h"
#include "drawingcanvas.h"
#include "defines.h"

MainWindow::MainWindow(FileParser *parser_in)
{
	undoStack = new QUndoStack();

	createActions();
	createToolBox();
	createMenus();
	createToolbars();

	tabWidget = new QTabWidget(this);
	tabWidget->setTabPosition(QTabWidget::North);
	tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	tabWidget->setGeometry(0, 0, static_cast<int>(DEFAULT_SCENE_SIZE_X), static_cast<int>(DEFAULT_SCENE_SIZE_Y));
	tabWidget->setDocumentMode(true);
	tabWidget->setTabsClosable(true);
	tabWidget->setMovable(true);
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
	connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosed(int)));

	Atom::fillLabelToVdwRadiusMap();
	Atom::fillLabelToMassMap();

	QSettings settings;
	QMap<QString, QVariant> colorMap = settings.value("Default Atom Colors", QVariant(QMap<QString, QVariant>())).toMap();
	if(colorMap.isEmpty())
		Atom::fillLabelToColorMap();
	else
		Atom::labelToColor = colorMap;

	QDesktopWidget qdw;
	int screenCenterX = qdw.width() / 2;
	int screenCenterY = qdw.height() / 2;
	this->setGeometry(screenCenterX - 600, screenCenterY - 350, 1200, 700);

	layout = new QHBoxLayout;
	splitter = new QSplitter(Qt::Horizontal);
	splitter->addWidget(tabWidget);
	splitter->addWidget(toolBox);
	layout->addWidget(splitter);

	QWidget* centerWidget = new QWidget;
	centerWidget->setLayout(layout);
	setCentralWidget(centerWidget);

	this->setWindowIconText("cheMVP");
	this->setWindowTitle("cheMVP");
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("Recently Opened Files", QVariant(recentlyOpenedFiles));
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
	if(event->reason() != Qt::TabFocusReason)
		QMainWindow::focusOutEvent(event);
	else
	{
		QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
		QMainWindow::keyPressEvent(newEvent);
	}
}

void MainWindow::insertTextAtCursor(QAction *action)
{
	foreach(QGraphicsItem* item, canvas()->items()){
		if(ITEM_IS_LABEL){
			Label *label = dynamic_cast<Label*>(item);
			if(label->textInteractionFlags() & Qt::TextEditorInteraction){
				QTextCursor cursor = label->textCursor();
				cursor.insertText(action->iconText());
			}
		}
	}
	update();
}

QIcon MainWindow::textToIcon(const QString &string)
{
	QSize iconBox(32, 32);
	// Start by drawing the button icon
	QPixmap pixmap(iconBox);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	QFont font;
	font.setPointSize(22);
	painter.setFont(font);
	painter.drawText(QRectF(0, 0, 32, 32), Qt::AlignCenter, string);
	return QIcon(pixmap);
}

void MainWindow::setLabelBoldness(bool bold)
{
	foreach(QGraphicsItem* item, canvas()->items())
	{
		if(ITEM_IS_LABEL)
		{
			Label* label = dynamic_cast<Label*>(item);
			if(label->isSelected() || (label->textInteractionFlags() & Qt::TextEditorInteraction))
				label->setBold(bold);
		}
	}
	drawingInfo()->determineScaleFactor();
	canvas()->refresh();
}

void MainWindow::setLabelItalics(bool italic)
{
	foreach(QGraphicsItem* item, canvas()->items())
	{
		if(ITEM_IS_LABEL)
		{
			Label* label = dynamic_cast<Label*>(item);
			if(label->isSelected() || (label->textInteractionFlags() & Qt::TextEditorInteraction))
				label->setItalic(italic);
		}
	}
	drawingInfo()->determineScaleFactor();
	canvas()->refresh();
}

void MainWindow::setLabelUnderline(bool underline)
{
	foreach(QGraphicsItem* item, canvas()->items())
	{
		if(ITEM_IS_LABEL)
		{
			Label* label = dynamic_cast<Label*>(item);
			if(label->isSelected() || (label->textInteractionFlags() & Qt::TextEditorInteraction))
				label->setUnderline(underline);
		}
	}
	drawingInfo()->determineScaleFactor();
	canvas()->refresh();
}

void MainWindow::setLabelFont(QFont font)
{
	foreach(QGraphicsItem* item, canvas()->items())
	{
		if(ITEM_IS_LABEL)
		{
			Label* label = dynamic_cast<Label*>(item);
			if(label->isSelected() || (label->textInteractionFlags() & Qt::TextEditorInteraction))
				label->setCurrentFont(font);
		}
	}
	drawingInfo()->determineScaleFactor();
	canvas()->refresh();
}

void MainWindow::setLabelFontSize(QString size)
{
	foreach(QGraphicsItem* item, canvas()->items())
	{
		if(ITEM_IS_LABEL)
		{
			Label* label = dynamic_cast<Label*>(item);
			if(label->isSelected() || (label->textInteractionFlags() & Qt::TextEditorInteraction))
				label->setCurrentFontSize(size.toInt());
		}
	}
	drawingInfo()->determineScaleFactor();
	canvas()->refresh();
}

void MainWindow::mouseModeButtonGroupClicked(int buttonID)
{
	// This is just in case the mode was changed automatically by the canvas
	foreach(QAbstractButton *button, mouseModeButtonGroup->buttons()){
		if(mouseModeButtonGroup->id(button) == buttonID){
			button->setChecked(true);
		}
	}

	canvas()->setMode(DrawingCanvas::Mode(mouseModeButtonGroup->checkedId()));
	if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Rotate){
		canvas()->setAcceptsHovers(false);
	}else{
		canvas()->setAcceptsHovers(true);
	}
	if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Rotate){
		view()->setCursor(canvas()->rotateCursor());
	}
	if(mouseModeButtonGroup->checkedId() == DrawingCanvas::AddText){
		view()->setCursor(Qt::IBeamCursor);
	}
	if(mouseModeButtonGroup->checkedId() == DrawingCanvas::Select){
		view()->setCursor(Qt::ArrowCursor);
	}
	if(mouseModeButtonGroup->checkedId() == DrawingCanvas::AddBond){
		view()->setCursor(Qt::ArrowCursor);
	}
}

void MainWindow::setGeometryStep(int geom)
{
	assert(geom >= 0 && geom < parser()->numMolecules());
	parser()->setCurrent(geom);
	canvas()->storeLabeledBonds();
	canvas()->clearAll();
	canvas()->loadFromParser();
	canvas()->restoreLabeledBonds();
}

void MainWindow::rotateFromInitialCoordinates()
{
	drawingInfo()->setXRot(xRotationBox->text().toInt());
	drawingInfo()->setYRot(yRotationBox->text().toInt());
	drawingInfo()->setZRot(zRotationBox->text().toInt());
	canvas()->rotateFromInitialCoordinates();
}

void MainWindow::setAddArrowMode()
{
	mouseModeButtonGroupClicked((int) DrawingCanvas::AddArrow);
}

void MainWindow::setAtomLabels()
{
	canvas()->setAtomLabels(atomLabelInput->text());
}

void MainWindow::changeAtomSize()
{
	if(atomSizeSpinBox->value() == atomSizeSpinBox->minimum()) return;

	if(atomSizeSpinBox->specialValueText().size()){
		atomSizeSpinBox->setSpecialValueText(tr(""));
		atomSizeSpinBox->setValue(DEFAULT_ATOM_SCALE_FACTOR);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas()->selectedItems()){
			if(item->type() == Atom::Type){
				Atom *atom = dynamic_cast<Atom*>(item);
				atom->setScaleFactor(atomSizeSpinBox->value());
			}
		}
		canvas()->refresh();
	}
}


void MainWindow::foggingToggled(int useFogging)
{
	drawingInfo()->setUseFogging(useFogging);
	useFoggingBox->setEnabled((bool)useFogging);
	useFoggingBox->setVisible(false);
	std::cout<<"I was called!"<<std::endl;
}


void MainWindow::changeBondSize()
{
	if(bondSizeSpinBox->value() == bondSizeSpinBox->minimum()) return;

	if(bondSizeSpinBox->specialValueText().size()) {
		bondSizeSpinBox->setSpecialValueText(tr(""));
		bondSizeSpinBox->setValue(DEFAULT_BOND_THICKNESS);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas()->selectedItems()) {
			if(item->type() == Bond::Type) {
				Bond *bond = dynamic_cast<Bond*>(item);
				bond->setThickness(bondSizeSpinBox->value());
			}
		}
		canvas()->refresh();
	}
}

void MainWindow::changeZoom(int val)
{
	drawingInfo()->setZoom(val);
	canvas()->refresh();
}

void MainWindow::resetSignalsOnFileLoad()
{
	connect(canvas(), SIGNAL(selectionChanged()), this, SLOT(updateMenus()));
	connect(selectAllAction, SIGNAL(triggered()), canvas(), SLOT(selectAll()));
	connect(unselectAllAction, SIGNAL(triggered()), canvas(), SLOT(unselectAll()));
	connect(canvas(), SIGNAL(mouseModeChanged(int)), this, SLOT(mouseModeButtonGroupClicked(int)));
	connect(canvas(), SIGNAL(updateTextToolbars()), this, SLOT(updateTextLabelToolbar()));
	connect(useFoggingBox, SIGNAL(toggled(bool)), drawingInfo(), SLOT(setUseFogging(bool)));
	connect(useFoggingBox, SIGNAL(toggled(bool)), canvas(), SLOT(refresh()));
	connect(foggingScaleBox, SIGNAL(valueChanged(int)), drawingInfo(), SLOT(setFoggingScale(int)));
	connect(foggingScaleBox, SIGNAL(valueChanged(int)), canvas(), SLOT(refresh()));
	connect(backgroundColorButton, SIGNAL(clicked()), canvas(), SLOT(setBackgroundColor()));
	connect(backgroundOpacitySpinBox, SIGNAL(valueChanged(int)), canvas(), SLOT(setBackgroundOpacity(int)));
	connect(toggleBondLabelsButton, SIGNAL(pressed()), canvas(), SLOT(toggleBondLabels()));
	connect(bondLabelsPrecisionBox, SIGNAL(valueChanged(int)), canvas(), SLOT(setBondLabelPrecision(int)));
	connect(toggleAngleLabelsButton, SIGNAL(pressed()), canvas(), SLOT(toggleAngleLabels()));
	connect(angleLabelsPrecisionBox, SIGNAL(valueChanged(int)), canvas(), SLOT(setAngleLabelPrecision(int)));
	connect(toggleBondDashingButton, SIGNAL(pressed()), canvas(), SLOT(toggleBondDashing()));
	connect(atomColorButton, SIGNAL(clicked()), canvas(), SLOT(setAtomColors()));
	connect(atomDrawingStyleButtonGroup, SIGNAL(buttonClicked(int)), canvas(), SLOT(setAtomDrawingStyle(int)));
	connect(toggleAtomNumberSubscriptsButton, SIGNAL(pressed()), canvas(), SLOT(toggleAtomNumberSubscripts()));
	connect(atomLabelFontCombo, SIGNAL(currentFontChanged(const QFont &)), canvas(), SLOT(atomLabelFontChanged(const QFont &)));
	connect(atomLabelFontSizeCombo, SIGNAL(currentIndexChanged(const QString &)), canvas(), SLOT(atomLabelFontSizeChanged(const QString &)));
	connect(atomFontSizeButtonGroup, SIGNAL(buttonClicked(int)), canvas(), SLOT(setAtomFontSizeStyle(int)));

	// Re-sync toolbar to (possibly) new canvas
	mouseModeButtonGroupClicked(mouseModeButtonGroup->checkedId());
}

void MainWindow::tabSelected()
{
	if(tabWidget->count() > 0)
	{
		currentTab()->update();
		QString s = currentTab()->windowLabel;
		this->setWindowTitle(s);

		// Enable the widgets in the animation tab if there are multiple geometries
		animationSlider->blockSignals(true);
		if (parser()->numMolecules() <= 1)
			animationWidget->setEnabled(false);
		else
			animationWidget->setEnabled(true);

		// Set the sliders range and current value.
		animationSlider->setRange(0, parser()->numMolecules() - 1);
		animationSlider->setValue(parser()->current());
		animationSlider->blockSignals(false);

		resetSignalsOnFileLoad();
	}
}

void MainWindow::tabClosed(int i)
{
	Tab* tab = static_cast<Tab*>(tabWidget->widget(i));
	tabWidget->removeTab(i);

	if(!tabWidget->count())
		deactivateToolBar();

	delete tab->drawingInfo;
	delete tab->view;
	delete tab->canvas;
	delete tab->parser;
	delete tab;
}
