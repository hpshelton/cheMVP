#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QtSvg>
#include <QSvgGenerator>
#include <QToolBox>
#include <QToolButton>
#include <QSlider>
#include <QLabel>
#include <QUndoCommand>
#include <QDebug>
#include <QSettings>
#include <QMap>
#include <QTabWidget>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "splashscreen.h"
#include "drawingcanvas.h"
#include "drawingdisplay.h"
#include "drawinginfo.h"
#include "fileparser.h"
#include "undo_delete.h"
#include "preferences.h"
#include "tab.h"
#include "toolbarstate.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(FileParser *parser);
	~MainWindow();
	enum FileType {TIFF, PNG, PDF, PostScript, SVG, CHMVP, Unknown};

public slots:
	void setGeometryStep(int);
	void setCurrentSaveFile(QString file) {currentSaveFile = file;}
	void openProject(QString filename, Tab* tab, bool onNewMainWindow = false);
	void saveAndExit();

private slots:
	void insertTextAtCursor(QAction *action);
	void updateTextLabelToolbar();
	void updateMenus();
	void setAtomLabels();
	void changeAtomSize();
	void changeBondSize();
	void setLabelBoldness(bool b);
	void setLabelItalics(bool b);
	void setLabelUnderline(bool b);
	void setLabelFont(QFont f);
	void setLabelFontSize(QString s);
	void deleteItem();
	void mouseModeButtonGroupClicked(int);
	void rotateFromInitialCoordinates();
	void openFile();
	void save();
	void saveAs();
	void saveProject(QString filename);
	void changeZoom(int);
	void setAddArrowMode();
	void aboutCheMVP();
	void showPreferences();
	void openRecentFile();
	void tabSelected();
	void closeCurrentTab();
	void tabClosed(int i);

private:
	void focusOutEvent(QFocusEvent *event);
	void createToolBox(int width = DEFAULT_TOOLBOX_WIDTH, int height = DEFAULT_SCENE_SIZE_Y);
	QMap<QString, QString>* defaultToolBoxOptions();
	void createActions();
	void createMenus();
	void createToolbars();
	void updateRecentFiles();
	FileType determineFileType(const QString &fileName);
	void saveImage(const QString &fileName);
	void foggingToggled(int useFogging);
	void loadFile(FileParser* p, Tab* tab);
	void resetSignalsOnFileLoad();
	void resetButtonsOnFileLoad(bool project);
	QIcon textToIcon(const QString &string);
	void disableLabelSignals();
	void enableLabelSignals();
	void resetToolBox(QMap<QString, QString>* options);
	void activateToolBar();
	void deactivateToolBar();
	Tab* currentTab() { return static_cast<Tab*>(tabWidget->currentWidget()); };
	void setToolBarProperties(ToolBarState* state);

	// Tab properties
	DrawingInfo* drawingInfo() { return currentTab()->drawingInfo; };
	DrawingCanvas* canvas() { return currentTab()->canvas; };
	FileParser* parser() { return currentTab()->parser; };
	QGraphicsView* view() { return currentTab()->view; };
	QUndoStack* undoStack() { return currentTab()->undoStack; };

	QWidget *createAppearanceWidget(QMap<QString, QString>* options);
	QWidget *createBondsAndAnglesWidget(QMap<QString, QString>* options);
	QWidget *createAtomsWidget(QMap<QString, QString>* options);
	QWidget *createAnnotationWidget(QMap<QString, QString>* options);
	QWidget *createAnimationWidget(QMap<QString, QString>* options);
	QToolButton *makeAtomButton(const char *);
	QSlider *createSlider(int max);
	QCheckBox *useFoggingBox;
	QLabel* foggingLabel;

	QWidget *animationWidget;

	QButtonGroup *mouseModeButtonGroup;
	QButtonGroup *atomDrawingStyleButtonGroup;
	QButtonGroup *atomFontSizeButtonGroup;

	QLineEdit *atomLabelInput;

	QToolBar *mouseModeToolBar;
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QToolBar *editSelectedTextToolBar;

	QSplitter* splitter;

	QComboBox *atomLabelFontSizeCombo;
	QComboBox *textFontSizeCombo;

	QFontComboBox *atomLabelFontCombo;
	QFontComboBox *textFontCombo;

	QActionGroup *insertTextActionGroup;
	QAction *deleteAction;
	QAction *openAction;
	QAction *closeAction;
	QAction *exitAction;
	QAction *saveAction;
	QAction *saveAsAction;
	QAction *insertAngstromAction;
	QAction *insertDegreeAction;
	QAction *insertPlusMinusAction;
	QAction *addArrowAction;
	QAction *unselectAllAction;
	QAction *selectAllAction;
	QAction *undoAction;
	QAction *redoAction;
	QAction *aboutAction;
	QAction *showPreferencesAction;

	QPushButton *toggleBondLabelsButton;
	QPushButton *toggleBondDashingButton;
	QPushButton *toggleAngleLabelsButton;
	QPushButton *toggleAtomNumberSubscriptsButton;
	QPushButton *backgroundColorButton;
	QPushButton *atomColorButton;

	QToolButton *boldTextButton;
	QToolButton *italicTextButton;
	QToolButton *underlineTextButton;

	QRadioButton *simpleAtomDrawingButton;
	QRadioButton *houkMolAtomDrawingButton;
	QRadioButton *simpleColoredAtomDrawingButton;
	QRadioButton *gradientColoredAtomDrawingButton;
	QRadioButton *largeLabelAtomDrawingButton;
	QRadioButton *smallLabelAtomDrawingButton;

	QSpinBox	   *backgroundOpacitySpinBox;
	QSpinBox       *zoomSpinBox;
	QSpinBox       *bondLabelsPrecisionBox;
	QSpinBox       *angleLabelsPrecisionBox;
	QSpinBox       *foggingScaleBox;
	QDoubleSpinBox *atomSizeSpinBox;
	QDoubleSpinBox *bondSizeSpinBox;

	QLineEdit *xRotationBox;
	QLineEdit *yRotationBox;
	QLineEdit *zRotationBox;

	QLabel *xLabel;
	QLabel *yLabel;
	QLabel *zLabel;

	QSlider *animationSlider;

	QString currentSaveFile;
	QMenu *fileMenu;
	QMenu *itemMenu;
	QMenu *editMenu;
	QMenu *insertMenu;
	QMenu *insertSymbolMenu;

	QToolBox *toolBox;

	QList<QString> recentlyOpenedFiles;
	QList<QAction*> recentFileActions;
	QAction* separatorAction;

	QTabWidget* tabWidget;
	QHBoxLayout* layout;
	int previousTab;
};

#endif
