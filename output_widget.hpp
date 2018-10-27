#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLayout>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "expression.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"

class OutputWidget : public QWidget{
Q_OBJECT

public:
    OutputWidget(QWidget * parent = nullptr);
    void printList(Expression exp);
    
private slots:
    void recieveText(QString str);

private:
bool shouldClear = false;
bool singleTextPrinted = false;
QGraphicsScene * scene;
QGraphicsView * view;
Interpreter interp;
};

#endif