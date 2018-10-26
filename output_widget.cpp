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
            //Environment env;
            //qDebug() << str;
            // else if(!exp.isHeadList()){
            //     shouldClear = true;
            // }
            // if(shouldClear == true){
            //     scene->clear();
            // }
            scene->clear();
            if(exp.isHeadList()){
                for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
                    //std::cout << "Output: " << (*e).head().asString() << std::endl;
                    scene->addText(QString::fromStdString((*e).makeString()));
                }
                return;
            }
            else if(exp.head().isLambda()){
                //scene->addText(QString::fromStdString(""));
                return;
            }
            scene->addText(QString::fromStdString(exp.makeString()));
        }
        catch(const SemanticError & ex){

            scene->clear();
            scene->addText(QString(ex.what()));
        }
    }
}
