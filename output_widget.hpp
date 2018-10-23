#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLayout>

class OutputWidget : public QWidget{
Q_OBJECT

public:
    OutputWidget(QWidget * parent = nullptr);

private slots:
    void recieveText(QString str);

private:
QGraphicsScene * scene;
QGraphicsView * view;
};

#endif