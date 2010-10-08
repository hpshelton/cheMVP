#include "toolboxstate.h"

ToolBoxState::ToolBoxState()
{
}

ToolBoxState* ToolBoxState::defaultToolBoxState()
{
	QMap<QString, QString>* options = new QMap<QString, QString>();

	// Appearance
	options->insert("FOGGING_ON", QString("%1").arg(false));
	options->insert("FOGGING_SCALE", QString("%1").arg(DEFAULT_FOGGING_SCALE));
	options->insert("X_ROTATION", "0");
	options->insert("Y_ROTATION", "0");
	options->insert("Z_ROTATION", "0");
	options->insert("BACKGROUND_OPACITY", QString("%1").arg(DEFAULT_BACKGROUND_OPACITY));
	options->insert("ZOOM", "100");

	// Bonds and Angles
	options->insert("BOND_LABEL_PRECISION", QString("%1").arg(DEFAULT_BOND_LABEL_PRECISION));
	options->insert("ANGLE_LABEL_PRECISION", QString("%1").arg(DEFAULT_ANGLE_LABEL_PRECISION));

	// Atoms
	options->insert("ATOM_DRAWING_STYLE", QString("%1").arg(DrawingInfo::Gradient));
	options->insert("ATOM_LABEL_STYLE", QString("%1").arg(Atom::SmallLabel));

	return new ToolBoxState(options);
}

void ToolBoxState::setUseFogging(bool b)
{
	_options->insert("FOGGING_ON", QString("%1").arg(b));
}

void ToolBoxState::setFoggingScale(int i)
{
	_options->insert("FOGGING_SCALE", QString("%1").arg(i));
}

void ToolBoxState::setBackgroundOpacity(int i)
{
	_options->insert("BACKGROUND_OPACITY", QString("%1").arg(i));
}

void ToolBoxState::setXRotation(int i)
{
	_options->insert("X_ROTATION", QString("%1").arg(i));
}

void ToolBoxState::setYRotation(int i)
{
	_options->insert("Y_ROTATION", QString("%1").arg(i));
}

void ToolBoxState::setZRotation(int i)
{
	_options->insert("Z_ROTATION", QString("%1").arg(i));
}

void ToolBoxState::setZoom(int i)
{
	_options->insert("ZOOM", QString("%1").arg(i));
}

void ToolBoxState::setBondPrecision(int i)
{
	_options->insert("BOND_LABEL_PRECISION", QString("%1").arg(i));
}

void ToolBoxState::setAnglePrecision(int i)
{
	_options->insert("ANGLE_LABEL_PRECISION", QString("%1").arg(i));
}

void ToolBoxState::setAtomSize(QString s)
{
	_options->insert("ATOM_SIZE", s);
}

void ToolBoxState::setAtomStyle(int i)
{
	_options->insert("ATOM_DRAWING_STYLE", QString("%1").arg(i));
}

void ToolBoxState::setAtomLabelText(QString s)
{
	_options->insert("ATOM_LABEL_TEXT", s);
}

void ToolBoxState::setAtomLabelFont(QString s)
{
	_options->insert("ATOM_LABEL_FONT", s);
}

void ToolBoxState::setAtomLabelSize(QString s)
{
	_options->insert("ATOM_LABEL_SIZE", s);
}

void ToolBoxState::setAtomLabelStyle(int i)
{
	_options->insert("ATOM_LABEL_STYLE", QString("%1").arg(i));
}

void ToolBoxState::setBondThickness(QString s)
{
	_options->insert("BOND_THICKNESS", s);
}
