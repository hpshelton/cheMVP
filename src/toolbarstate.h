#ifndef TOOLBARSTATE_H
#define TOOLBARSTATE_H

#include <QFont>
#include "defines.h"

class ToolBarState
{
private:
	int _mouseMode;
	bool _textEditingEnabled;
	bool _bold;
	bool _italic;
	bool _underline;
	QFont _font;
	QString _size;

public:
	ToolBarState();
	void setMouseMode(int i) { _mouseMode = i; };
	int getMouseMode() { return _mouseMode; };

	void setTextEditing(bool b) { _textEditingEnabled = b; };
	bool getTextEditing() { return _textEditingEnabled; };

	void setBold(bool b) { _bold = b; };
	bool getBold() { return _bold; };

	void setItalic(bool b) { _italic = b; };
	bool getItalic() { return _italic; };

	void setUnderline(bool b) { _underline = b; };
	bool getUnderline() { return _underline; };

	void setFont(QFont f) { _font = f; };
	QFont getFont() { return _font; };

	void setFontSize(QString s) { _size = s; };
	QString getFontSize() { return _size; };
};

#endif // TOOLBARSTATE_H
