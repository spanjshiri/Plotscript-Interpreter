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
                if(exp.isPoint()){
                    std::vector<Expression> tail = exp.makeTail();
                    double w = exp.getSize();
                    double h = exp.getSize();
                    double x = tail[0].head().asNumber()-(w/2);
                    double y = tail[1].head().asNumber()-(w/2);
                    const QPen pen = QPen(Qt::black);
                    const QBrush brush = QBrush(Qt::black);
                    scene->QGraphicsScene::addEllipse(x,y,w,h,pen,brush);
                }
                else if(exp.isLine()){
                    std::vector<Expression> tail = exp.makeTail();
                    double thickness = exp.getThickness();
                    std::vector<Expression> p1 = tail[0].makeTail();
                    std::vector<Expression> p2 = tail[1].makeTail();
                    double x1 = p1[0].head().asNumber();
                    double y1 = p1[1].head().asNumber();
                    double x2 = p2[0].head().asNumber();
                    double y2 = p2[1].head().asNumber();
                    QPen pen = QPen(Qt::black);
                    pen.setWidth(thickness);
                    scene->QGraphicsScene::addLine(x1,y1,x2,y2,pen);
                }
                else if(exp.isText()){
                    QGraphicsTextItem::setPos();
                }
                else{
                    for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e){
                        scene->addText(QString::fromStdString((*e).makeString()));
                    }
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
