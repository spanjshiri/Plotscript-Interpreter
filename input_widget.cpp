#include "input_widget.hpp"

InputWidget::InputWidget(QWidget * parent) : QPlainTextEdit(parent){
    setObjectName("input");
    this->installEventFilter(this);
}

bool InputWidget::eventFilter(QObject *object, QEvent *event)
{
    static QSet<int> pressedKeys;

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        pressedKeys += ((QKeyEvent*)event)->key();
        if(pressedKeys.contains(Qt::Key_Shift) && pressedKeys.contains(Qt::Key_Return))
        {
            emit sendText(toPlainText());
        }
        else if(pressedKeys.contains(Qt::Key_Control) && pressedKeys.contains(Qt::Key_C)){
            emit QApplication::quit();
        }
    }
    else if(event->type()==QEvent::KeyRelease)
    {
        pressedKeys -= ((QKeyEvent*)event)->key();
    }
    return object->event(event);
}
