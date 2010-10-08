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
	createActions();
	createToolBox();
	createMenus();
	createToolbars();

	QWidget* container = new QWidget(this);
	tabWidget = new QTabWidget(container);
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

	QVBoxLayout* containerLayout = new QVBoxLayout(container);
	containerLayout->addWidget(tabWidget);
	container->setLayout(containerLayout);
	container->setContentsMargins(-12, -4, -12, -12);
	container->setStyleSheet("background-color: #979797;");
	container->setGeometry(0, 0, static_cast<int>(DEFAULT_SCENE_SIZE_X), static_cast<int>(DEFAULT_SCENE_SIZE_Y));

	splitter->addWidget(container);
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
	currentTab()->toolBarState->setBold(bold);
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
	currentTab()->toolBarState->setItalic(italic);
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
	currentTab()->toolBarState->setUnderline(underline);
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
	currentTab()->toolBarState->setFont(font);
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
	currentTab()->toolBarState->setFontSize(size);
}

void MainWindow::mouseModeButtonGroupClicked(int buttonID)
{
	// This is just in case the mode was changed automatically by the canvas
	foreach(QAbstractButton *button, mouseModeButtonGroup->buttons()){
		if(mouseModeButtonGroup->id(button) == buttonID){
			button->setChecked(true);
		}
	}

	int checkedButton = mouseModeButtonGroup->checkedId();
	canvas()->setMode(DrawingCanvas::Mode(checkedButton));
	currentTab()->toolBarState->setMouseMode(checkedButton);

	if(checkedButton == DrawingCanvas::Rotate){
		canvas()->setAcceptsHovers(false);
	}else{
		canvas()->setAcceptsHovers(true);
	}
	if(checkedButton == DrawingCanvas::Rotate){
		view()->setCursor(canvas()->rotateCursor());
	}
	if(checkedButton == DrawingCanvas::AddText){
		view()->setCursor(Qt::IBeamCursor);
	}
	if(checkedButton == DrawingCanvas::Select){
		view()->setCursor(Qt::ArrowCursor);
	}
	if(checkedButton == DrawingCanvas::AddBond){
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
	int x = xRotationBox->text().toInt();
	int y = yRotationBox->text().toInt();
	int z = zRotationBox->text().toInt();
	drawingInfo()->setXRot(x);
	drawingInfo()->setYRot(y);
	drawingInfo()->setZRot(z);
	canvas()->rotateFromInitialCoordinates();
	currentTab()->toolBoxState->setXRotation(x);
	currentTab()->toolBoxState->setYRotation(y);
	currentTab()->toolBoxState->setZRotation(z);
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
	double value = atomSizeSpinBox->value();
	if(value == atomSizeSpinBox->minimum()) return;

	if(atomSizeSpinBox->specialValueText().size()){
		atomSizeSpinBox->setSpecialValueText(tr(""));
		atomSizeSpinBox->setValue(DEFAULT_ATOM_SCALE_FACTOR);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas()->selectedItems()){
			if(item->type() == Atom::Type){
				Atom *atom = dynamic_cast<Atom*>(item);
				atom->setScaleFactor(value);
			}
		}
		canvas()->refresh();
	}
	currentTab()->toolBoxState->setAtomSize(QString("%1").arg(value));
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
	double value = bondSizeSpinBox->value();
	if(value == bondSizeSpinBox->minimum()) return;

	if(bondSizeSpinBox->specialValueText().size()) {
		bondSizeSpinBox->setSpecialValueText(tr(""));
		bondSizeSpinBox->setValue(DEFAULT_BOND_THICKNESS);
	}else{
		QGraphicsItem *item;
		foreach(item, canvas()->selectedItems()) {
			if(item->type() == Bond::Type) {
				Bond *bond = dynamic_cast<Bond*>(item);
				bond->setThickness(value);
			}
		}
		canvas()->refresh();
	}
	currentTab()->toolBoxState->setBondThickness(QString("%1").arg(value));
}

void MainWindow::changeZoom(int val)
{
	drawingInfo()->setZoom(val);
	canvas()->refresh();
	currentTab()->toolBoxState->setZoom(val);
}

void MainWindow::setUseFogging(bool b)
{
	drawingInfo()->setUseFogging(b);
	currentTab()->toolBoxState->setUseFogging(b);
}

void MainWindow::setFoggingScale(int i)
{
	drawingInfo()->setFoggingScale(i);
	currentTab()->toolBoxState->setFoggingScale(i);
}

void MainWindow::setBackgroundOpacity(int i)
{
	canvas()->setBackgroundOpacity(i);
	currentTab()->toolBoxState->setBackgroundOpacity(i);
}

void MainWindow::setBondLabelPrecision(int i)
{
	canvas()->setBondLabelPrecision(i);
	currentTab()->toolBoxState->setBondPrecision(i);
}

void MainWindow::setAngleLabelPrecision(int i)
{
	canvas()->setAngleLabelPrecision(i);
	currentTab()->toolBoxState->setAnglePrecision(i);
}

void MainWindow::setAtomDrawingStyle(int i)
{
	canvas()->setAtomDrawingStyle(i);
	currentTab()->toolBoxState->setAtomStyle(i);
}

void MainWindow::atomLabelFontChanged(QFont f)
{
	canvas()->atomLabelFontChanged(f);
	currentTab()->toolBoxState->setAtomLabelFont(f.family());
}

void MainWindow::atomLabelFontSizeChanged(QString s)
{
	canvas()->atomLabelFontSizeChanged(s);
	currentTab()->toolBoxState->setAtomLabelSize(s);
}

void MainWindow::setAtomFontSizeStyle(int i)
{
	canvas()->setAtomFontSizeStyle(i);
	currentTab()->toolBoxState->setAtomLabelStyle(i);
}

void MainWindow::resetSignalsOnFileLoad()
{
	connect(canvas(), SIGNAL(selectionChanged()), this, SLOT(updateMenus()));
	connect(canvas(), SIGNAL(mouseModeChanged(int)), this, SLOT(mouseModeButtonGroupClicked(int)));
	connect(canvas(), SIGNAL(updateTextToolbars()), this, SLOT(updateTextLabelToolbar()));

	disconnect(selectAllAction, 0, 0, 0);
	connect(selectAllAction, SIGNAL(triggered()), canvas(), SLOT(selectAll()));

	disconnect(unselectAllAction, 0, 0, 0);
	connect(unselectAllAction, SIGNAL(triggered()), canvas(), SLOT(unselectAll()));

	connect(useFoggingBox, SIGNAL(toggled(bool)), this, SLOT(setUseFogging(bool)));
	connect(useFoggingBox, SIGNAL(toggled(bool)), canvas(), SLOT(refresh()));

	connect(foggingScaleBox, SIGNAL(valueChanged(int)), this, SLOT(setFoggingScale(int)));
	connect(foggingScaleBox, SIGNAL(valueChanged(int)), canvas(), SLOT(refresh()));

	disconnect(backgroundColorButton, 0, 0, 0);
	connect(backgroundColorButton, SIGNAL(clicked()), canvas(), SLOT(setBackgroundColor()));

	disconnect(backgroundOpacitySpinBox, 0, 0, 0);
	connect(backgroundOpacitySpinBox, SIGNAL(valueChanged(int)), this, SLOT(setBackgroundOpacity(int)));

	disconnect(toggleBondLabelsButton, 0, 0, 0);
	connect(toggleBondLabelsButton, SIGNAL(pressed()), canvas(), SLOT(toggleBondLabels()));

	disconnect(bondLabelsPrecisionBox, 0, 0, 0);
	connect(bondLabelsPrecisionBox, SIGNAL(valueChanged(int)), this, SLOT(setBondLabelPrecision(int)));

	disconnect(toggleAngleLabelsButton, 0, 0, 0);
	connect(toggleAngleLabelsButton, SIGNAL(pressed()), canvas(), SLOT(toggleAngleLabels()));

	disconnect(angleLabelsPrecisionBox, 0, 0, 0);
	connect(angleLabelsPrecisionBox, SIGNAL(valueChanged(int)), this, SLOT(setAngleLabelPrecision(int)));

	disconnect(toggleBondDashingButton, 0, 0, 0);
	connect(toggleBondDashingButton, SIGNAL(pressed()), canvas(), SLOT(toggleBondDashing()));

	disconnect(atomColorButton, 0, 0, 0);
	connect(atomColorButton, SIGNAL(clicked()), canvas(), SLOT(setAtomColors()));

	disconnect(atomDrawingStyleButtonGroup, 0, 0, 0);
	connect(atomDrawingStyleButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setAtomDrawingStyle(int)));

	disconnect(toggleAtomNumberSubscriptsButton, 0, 0, 0);
	connect(toggleAtomNumberSubscriptsButton, SIGNAL(pressed()), canvas(), SLOT(toggleAtomNumberSubscripts()));

	disconnect(atomLabelFontCombo, 0, 0, 0);
	connect(atomLabelFontCombo, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(atomLabelFontChanged(const QFont &))); //HPS - this signal never gets fired

	disconnect(atomLabelFontSizeCombo, 0, 0, 0);
	connect(atomLabelFontSizeCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(atomLabelFontSizeChanged(const QString &)));

	disconnect(atomFontSizeButtonGroup, 0, 0, 0);
	connect(atomFontSizeButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(setAtomFontSizeStyle(int)));

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

		this->setToolBarProperties(currentTab()->toolBarState);
		this->resetToolBox(currentTab()->toolBoxState->options());

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

		undoAction->disconnect();
		redoAction->disconnect();

		// Handle undo stack switching
		connect(undoAction, SIGNAL(triggered()), undoStack(), SLOT(undo()));
		connect(redoAction, SIGNAL(triggered()), undoStack(), SLOT(redo()));

		connect(undoStack(), SIGNAL(canRedoChanged(bool)), redoAction, SLOT(setEnabled(bool)));
		connect(undoStack(), SIGNAL(canUndoChanged(bool)), undoAction, SLOT(setEnabled(bool)));

		redoAction->setEnabled(undoStack()->canRedo());
		undoAction->setEnabled(undoStack()->canUndo());
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

	previousTab = tabWidget->currentIndex();
}
