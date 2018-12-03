#ifndef NOTEBOOK_APP_H
#define NOTEBOOK_APP_H

#include <QWidget>
#include <QLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "input_widget.hpp"
#include "output_widget.hpp"
#include "message_queue.hpp"

typedef MessageQueue<std::string> imq;
typedef MessageQueue<std::pair<std::string,Expression>> omq;

class NotebookApp: public QWidget{
Q_OBJECT

public:
    NotebookApp(QWidget * parent = nullptr);

signals:
    void sendText(QString str);

private:
    OutputWidget output;
    InputWidget input;
};

#endif
