#include "notebook_app.hpp"

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout;
    setObjectName("notebook");
    layout->addWidget(&input, 1);
    layout->addWidget(&output, 1);
    this->setLayout(layout);
    QObject::connect(&input, SIGNAL(sendText(QString)), &output, SLOT(recieveText(QString)));
}