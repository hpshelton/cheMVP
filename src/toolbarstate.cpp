#include "toolbarstate.h"

ToolBarState::ToolBarState()
{
	_mouseMode = 3;
	_textEditingEnabled = false;
	_bold = false;
	_italic = false;
	_underline = false;
	_font = QFont(DEFAULT_LABEL_FONT);
	_size = QString().setNum(DEFAULT_LABEL_FONT_SIZE);
}
