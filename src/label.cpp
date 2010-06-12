#include <QtGui>
#include <QtDebug>

#include "label.h"
#include "drawingcanvas.h"

Label::Label(LabelType type, double value, DrawingInfo *info, QGraphicsItem *parent, QGraphicsScene *scene)
		:QGraphicsTextItem(parent, scene),
		myType(type),
		myFontSize(DEFAULT_LABEL_FONT_SIZE),
		myDX(0.0),
		myDY(0.0),
		myValue(value),
		_info(info)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setTextInteractionFlags(Qt::NoTextInteraction);
	setZValue(1000.0);
	if(myType != TextLabelType){
		updateLabel();
	}
	setFont(QFont(DEFAULT_LABEL_FONT));
	updateFontSize();
	// If the zoom factor changes or the window size changes, we need to adjust
	// the size of the font for the labels accordingly
	connect(_info, SIGNAL(scaleFactorChanged()), this, SLOT(updateFontSize()));
	setToolTip(tr("Double click to edit"));
}

void Label::keyPressEvent(QKeyEvent *event)
{
	std::cout<<"key press event "<<event->key()<<"  "<<Qt::Key_Tab<<std::endl;
	if(event->key()==Qt::Key_Tab){
		QTextCursor t = textCursor();
		t.insertText("\t");
		setTextCursor(t);
		std::cout<<"inserting tab"<<std::endl;
		setTextInteractionFlags(Qt::TextEditorInteraction);
	} else {
		QGraphicsTextItem::keyPressEvent(event);
	}
}

void Label::focusOutEvent(QFocusEvent *event)
{
	if(event->reason()==Qt::TabFocusReason) {
		QKeyEvent *newEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
		QGraphicsTextItem::keyPressEvent(newEvent);
		event->accept();
	} else {
		QGraphicsTextItem::focusOutEvent(event);
		if(this->toPlainText().length() == 0)
			this->scene()->removeItem(this);
	}
}

void Label::updateFontSize()
{
	QFont myFont(font());
	// The denominator is completely arbitrary and is chosen to make the font size
	// appear on a reasonable scale
	myFont.setPointSizeF(double(myFontSize) * _info->scaleFactor() / 80.0);
	setFont(myFont);
}

void Label::updateLabel()
{
	if(myType == BondLabelType) {
		myString.setNum(myValue, 'f', _info->getBondPrecision());
	} else if(myType == AngleLabelType) {
		myString.setNum(myValue, 'f', _info->getAnglePrecision());
	}
	setPlainText(myString);
}

void Label::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if(textInteractionFlags() == Qt::NoTextInteraction) {
		setTextInteractionFlags(Qt::TextEditorInteraction);
	}
	QGraphicsTextItem::mouseDoubleClickEvent(event);
}

void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void Label::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	// A null event to prevent unwanted deselection
	Q_UNUSED(event);
}

void Label::serialize(QXmlStreamWriter* writer)
{
	writer->writeStartElement("Label");
	writer->writeAttribute("type", QString("%1").arg(myType));
	writer->writeAttribute("string", myString);
	writer->writeAttribute("text", toPlainText());
	writer->writeAttribute("fontSize", QString("%1").arg(myFontSize));
	writer->writeAttribute("dx", QString("%1").arg(myDX));
	writer->writeAttribute("dy", QString("%1").arg(myDY));
	writer->writeAttribute("value", QString("%1").arg(myValue));

	// Determine and write all the font formats used throughout the label
	QTextCursor* cursor = new QTextCursor(this->document());
	QMap<QString, QList<FontFormatTuple*>* > fontMap;
	int start = cursor->position();
	QString prev = "";
	QString format = "";
	QList<FontFormatTuple*>* values;
	int i = 0;
	for(i = start; !cursor->atEnd(); i++)
	{
		cursor->setPosition(cursor->position()+1);
		format = cursor->charFormat().font().toString();
		if(fontMap.contains(format))
		{
			values = fontMap[format];
			if(format == prev)
				((*values)[values->size() - 1])->end = i;
			else
			{
				FontFormatTuple* temp = new FontFormatTuple(format, i, i);
				values->append(temp);
			}
		}
		else
		{
			values = new QList<FontFormatTuple*>();
			FontFormatTuple* temp = new FontFormatTuple(format, i, i);
			values->append(temp);
		}
		fontMap.insert(format, values);
		prev = format;
	}

	if(fontMap.contains(format))
	{
		values = fontMap[format];
		((*values)[values->size() - 1])->end = i-1;
	}

	std::vector<FontFormatTuple*> list;
	foreach(QList<FontFormatTuple*>* ql, fontMap.values())
	{
		foreach(FontFormatTuple* v, *ql)
			list.push_back(v);
	}
	sort(list.begin(), list.end(), FontFormatTuple::compareTo);

	writer->writeAttribute("formats", QString("%1").arg(list.size()));
	foreach(FontFormatTuple* t, list)
		t->serialize(writer);

	writer->writeEndElement();

	delete cursor;
	// Delete lots of other stuff here, too
}

Label* Label::deserialize(QXmlStreamReader* reader, DrawingInfo* drawingInfo, QGraphicsScene* scene)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "Label");
	QXmlStreamAttributes attr = reader->attributes();
	LabelType type = Label::LabelType(attr.value("type").toString().toInt());
	Label* l = new Label(type, attr.value("value").toString().toDouble(), drawingInfo, NULL, scene);
	QString text = attr.value("text").toString();
	l->myString = attr.value("string").toString();
	l->myFontSize = attr.value("fontSize").toString().toInt();
	l->myDX = attr.value("dx").toString().toInt();
	l->myDY = attr.value("dy").toString().toInt();
	l->myValue = attr.value("value").toString().toDouble();

	int formats = attr.value("formats").toString().toInt();
	QTextCursor cursor(l->document());
	QTextCharFormat textFormat;
	QFont textFont;
	for(int i = 0; i < formats; i++) {
		FontFormatTuple* s = FontFormatTuple::deserialize(reader);
		QString t = text.left(s->end+1).right(s->end - s->start + 1);
		textFont.fromString(s->format);
		textFormat.setFont(textFont);
		cursor.insertText(t, textFormat);
	}
	return l;
}
