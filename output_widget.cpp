#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(this);
    setObjectName("output");
    layout->addWidget(view);
    view->setScene(scene);
    std::ifstream ifs(STARTUP_FILE);
    if(!interp.parseStream(ifs)){
        scene->clear();
        scene->addText("Error: Invalid Program. Could not parse.");
    }
    else{
        try{
        Expression exp = interp.evaluate();
        }
        catch(const SemanticError & ex){
            scene->clear();
            scene->addText(QString(ex.what()));
        }
    }
}

void OutputWidget::recieveText(QString str){
    std::istringstream expression(str.toStdString());
    if(!interp.parseStream(expression)){
      scene->clear();
      scene->addText("Error: Invalid Expression. Could not parse.");
    }
    else{
        try{
            Expression exp = interp.evaluate();
            if(exp.isHeadList()){
                shouldClear = false;
            }
            if(shouldClear == true){
                recieveText(str);
                scene->clear();
            }
            scene->addText(QString::fromStdString(exp.makeString()));
        }
        catch(const SemanticError & ex){

            scene->clear();
            scene->addText(QString(ex.what()));
        }
    }
}
