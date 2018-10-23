#include "notebook_app.hpp"
#include <QPlainTextEdit>
#include <QLayout>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout;
    setObjectName("notebook");
    layout->addWidget(&input, 1);
    layout->addWidget(&output, 1);
    this->setLayout(layout);
}