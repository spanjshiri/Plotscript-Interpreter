#include "output_widget.hpp"
#include <QLayout>
#include <QDebug>

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(this);
    setObjectName("output");
    layout->addWidget(view);
    view->setScene(scene);
}

void OutputWidget::recieveText(QString str){
    
}