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

class NotebookApp: public QWidget{
Q_OBJECT

public:
    NotebookApp(QWidget * parent = nullptr);

private:
    OutputWidget output;
    InputWidget input;

};

#endif
