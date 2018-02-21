#include "gui/cEventFilter.h"


bool cEventFilter::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::KeyPress) {
		//QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		//qDebug("Ate key press %d", keyEvent->key());
		return true;
	} 
	else {
		return QObject::eventFilter(obj, event);
	}
}

