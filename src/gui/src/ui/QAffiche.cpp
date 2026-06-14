#include "ui/QAffiche.h"
#include <QGestureEvent>
#include <QGesture>
#include <QMouseEvent>
#include <QMovie>
#include <QPointingDevice>
#include <QResizeEvent>
#include <QTabletEvent>


QAffiche::QAffiche(const QVariant &id, int border, QColor color, QWidget *parent)
	: QLabel(parent), m_lastPressed(Qt::NoButton)
{
	m_pressed = false;
	m_id = id;
	m_border = border;
	m_color = color;
	setText(QString());

	setAttribute(Qt::WA_AcceptTouchEvents, true);
	grabGesture(Qt::TapAndHoldGesture);
}

bool QAffiche::event(QEvent *event)
{
	if (event->type() == QEvent::Gesture) {
		auto *ge = static_cast<QGestureEvent *>(event);
		QGesture *hold = ge->gesture(Qt::TapAndHoldGesture);
		if (hold && hold->state() == Qt::GestureFinished) {
			m_lastPressed = Qt::RightButton;
			emit rightClicked();
			emit rightClicked(m_id.toInt());
			emit rightClicked(m_id.toString());
		}
		return true;
	}
	return QLabel::event(event);
}

// Handles stylus/pen input: barrel button or eraser end → right-click, tip → normal mouse synthesis.
void QAffiche::tabletEvent(QTabletEvent *event)
{
	const bool isRightAction = event->buttons().testFlag(Qt::RightButton)
		|| event->pointingDevice()->pointerType() == QPointingDevice::PointerType::Eraser;

	if (isRightAction) {
		if (event->type() == QEvent::TabletPress) {
			m_lastPressed = Qt::RightButton;
			m_pressed = true;
		} else if (event->type() == QEvent::TabletRelease && m_pressed) {
			m_pressed = false;
			if (hitLabel(event->position().toPoint())) {
				emit rightClicked();
				emit rightClicked(m_id.toInt());
				emit rightClicked(m_id.toString());
			}
		}
		event->accept();
	} else {
		// Pen tip: let Qt synthesize the standard mouse press/release
		event->ignore();
	}
}

void QAffiche::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		emit doubleClicked();
		emit doubleClicked(m_id.toInt());
	}
	QLabel::mouseDoubleClickEvent(event);
}

void QAffiche::mousePressEvent(QMouseEvent *event)
{
	m_lastPressed = event->button();
	m_pressed = event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton || event->button() == Qt::RightButton;
	emit pressed();
	emit pressed(m_id.toInt());
	QLabel::mousePressEvent(event);
}

void QAffiche::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_pressed && event->button() == Qt::LeftButton && hitLabel(event->pos())) {
		emit clicked();
		emit clicked(m_id.toInt());
		emit clicked(m_id.toString());
	} else if (m_pressed && event->button() == Qt::MiddleButton && hitLabel(event->pos())) {
		emit middleClicked();
		emit middleClicked(m_id.toInt());
		emit middleClicked(m_id.toString());
	} else if (m_pressed && event->button() == Qt::RightButton && hitLabel(event->pos())) {
		emit rightClicked();
		emit rightClicked(m_id.toInt());
		emit rightClicked(m_id.toString());
	}
	m_pressed = false;
	emit released();
	emit released(m_id.toInt());
	QLabel::mouseReleaseEvent(event);
}

void QAffiche::enterEvent(QEnterEvent *event)
{
	emit mouseOver();
	emit mouseOver(m_id.toInt());
	emit mouseOver(m_id.toString());
	QLabel::enterEvent(event);
}

void QAffiche::leaveEvent(QEvent *event)
{
	emit mouseOut();
	emit mouseOut(m_id.toInt());
	emit mouseOut(m_id.toString());
	QLabel::leaveEvent(event);
}

void QAffiche::resizeEvent(QResizeEvent *event)
{
	QMovie *mov = movie();
	if (mov != nullptr) {
		const QSize &movieSize = mov->currentPixmap().size();
		const QSize &newSize = event->size();
		if (newSize.width() < movieSize.width() || newSize.height() < movieSize.height()) {
			mov->setScaledSize(movieSize.scaled(newSize, Qt::KeepAspectRatio));
		}
	}
}

bool QAffiche::hitLabel(QPoint point)
{ return rect().contains(point); }
Qt::MouseButton QAffiche::lastPressed()
{ return m_lastPressed; }

void QAffiche::setImage(const QImage &image)
{ this->setPixmap(QPixmap::fromImage(image)); }
void QAffiche::setImage(const QPixmap &image)
{ this->setPixmap(image); }
