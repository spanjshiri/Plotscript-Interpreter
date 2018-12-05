#include "notebook_app.hpp"

NotebookApp::NotebookApp(QWidget * parent) : QWidget(parent) {
    QHBoxLayout *hLayout = new QHBoxLayout;
    QPushButton *startButton = new QPushButton("Start Kernel");
    QPushButton *stopButton = new QPushButton("Stop Kernel");
    QPushButton *resetButton = new QPushButton("Reset Kernel");
    QPushButton *interruptButton = new QPushButton("Interrupt");
    startButton->setObjectName("start");
    stopButton->setObjectName("stop");
    resetButton->setObjectName("reset");
    // interruptButton->setObjectName("interrupt");
    hLayout->addWidget(startButton);
    hLayout->addWidget(stopButton);
    hLayout->addWidget(resetButton);
    hLayout->addWidget(interruptButton);
    QVBoxLayout *vLayout = new QVBoxLayout;
    setObjectName("notebook");
    vLayout->addLayout(hLayout);
    vLayout->addWidget(&input, 1);
    vLayout->addWidget(&output, 1);
    this->setLayout(vLayout);
    QObject::connect(&input, SIGNAL(sendText(QString)), &output, SLOT(recieveText(QString)));
    QObject::connect(startButton, SIGNAL(clicked()), &output, SLOT(recieveStartSignal()));
    QObject::connect(stopButton, SIGNAL(clicked()), &output, SLOT(recieveStopSignal()));
    QObject::connect(resetButton, SIGNAL(clicked()), &output, SLOT(recieveResetSignal()));
    QObject::connect(interruptButton, SIGNAL(clicked()), &output, SLOT(recieveInterruptSignal()));
    // QObject::connect(&output ,SIGNAL(startTimerSignal(int)), timer, SLOT(start(int)));
    // QObject::connect(timer, SIGNAL(timeout()), &output, SLOT(recieveTimerSignal(QString)));
    // QObject::connect(&output ,SIGNAL(stopTimerSignal()), timer, SLOT(stop()));
}