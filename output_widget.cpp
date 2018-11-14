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
            scene->clear();
            if(exp.isText()){
                std::vector<Expression> tail = exp.makeTail();
                Expression newExp = exp.getPosition();
                double scale = exp.getTextScale();
                double rotation = exp.getTextRotation();
                std::vector<Expression> tail2 = newExp.makeTail();
                double x = tail2[0].head().asNumber();
                double y = tail2[1].head().asNumber();
                std::string text = exp.head().asString();
                std::string subText = text.substr(1,text.length()-2);
                QGraphicsTextItem *str = scene->addText(QString::fromStdString(subText));
                auto font = QFont("Monospace");
                font.setStyleHint(QFont::TypeWriter);
                font.setPointSize(1);
                str->setFont(font);
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                str->setPos(x, y);
                str->setScale(scale);
                str->setRotation(rotation*(180/M_PI));
                QRectF strRect = str->sceneBoundingRect();
                x-=strRect.width()/2;
                y-=strRect.height()/2;
                singleTextPrinted = true;
                return;
            }
            if(exp.isHeadList() || exp.head().isDiscrete()){
                printList(exp);
            }
            else if(exp.head().isLambda()){
                return;
            }
            else {
                scene->addText(QString::fromStdString(exp.makeString()));
            }
        }
        catch(const SemanticError & ex){
            scene->clear();
            scene->addText(QString(ex.what()));
        }
    }
}

void OutputWidget::printList(Expression exp){
    if(exp.isPoint()){
        std::vector<Expression> tail = exp.makeTail();
        double w = exp.getSize();
        double h = exp.getSize();
        double x = tail[0].head().asNumber()-(w/2);
        double y = tail[1].head().asNumber()-(w/2);
        const QPen pen = QPen(Qt::black);
        const QBrush brush = QBrush(Qt::black);
        if(exp.head().isDiscrete()){
            w = .5;
            h = .5;
        }
        scene->QGraphicsScene::addEllipse(x,y,w,h,pen,brush);
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
        if(exp.head().isDiscrete()){
            pen.setWidth(0);
        }
        scene->QGraphicsScene::addLine(x1,y1,x2,y2,pen);
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    else{
        for(auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
            if((*e).isPoint()){
                std::vector<Expression> tail = (*e).makeTail();
                double w = (*e).getSize();
                double h = (*e).getSize();
                double x = tail[0].head().asNumber()-(w/2);
                double y = tail[1].head().asNumber()-(w/2);
                const QPen pen = QPen(Qt::black);
                const QBrush brush = QBrush(Qt::black);
                if(exp.head().isDiscrete()){
                    w = .5;
                    h = .5;
                }
                scene->QGraphicsScene::addEllipse(x,y,w,h,pen,brush);
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            }
            else if((*e).isLine()){
                std::vector<Expression> tail = (*e).makeTail();
                double thickness = (*e).getThickness();
                std::vector<Expression> p1 = tail[0].makeTail();
                std::vector<Expression> p2 = tail[1].makeTail();
                double x1 = p1[0].head().asNumber();
                double y1 = p1[1].head().asNumber();
                double x2 = p2[0].head().asNumber();
                double y2 = p2[1].head().asNumber();
                QPen pen = QPen(Qt::black);
                pen.setWidth(thickness);
                if(exp.head().isDiscrete()){
                    pen.setWidth(0);
                }
                scene->QGraphicsScene::addLine(x1,y1,x2,y2,pen);
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            }
            else if((*e).isText() && !singleTextPrinted){
                std::vector<Expression> tail = (*e).makeTail();
                Expression newExp = (*e).getPosition();
                std::vector<Expression> tail2 = newExp.makeTail();
                double x = tail2[0].head().asNumber();
                double y = tail2[1].head().asNumber();
                double scale = exp.getTextScale();
                double rotation = exp.getTextRotation();
                std::string text = (*e).head().asString();
                std::string subText = text.substr(1,text.length()-2);
                QGraphicsTextItem *str = scene->addText(QString::fromStdString(subText));
                str->setPos(x, y);
                str->setScale(scale);
                str->setRotation(rotation*(180/M_PI));
                auto font = QFont("Monospace");
                font.setStyleHint(QFont::TypeWriter);
                font.setPointSize(1);
                str->setFont(font);
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                QRectF strRect = str->sceneBoundingRect();
                x-=strRect.width()/2;
                y-=strRect.height()/2;
            }
            else if(exp.head().isDiscrete()){
                static int count = 0;
                static std::string stringXMin = "";
                static std::string stringXMax = "";
                static std::string stringYMin = "";
                static std::string stringYMax = "";
                static std::string stringTitle = "";
                static std::string stringXLabel = "";
                static std::string stringYLabel = "";
                static double xMin = 0;
                static double xMax = 0;
                static double yMin = 0;
                static double yMax = 0;

                if(count == 0){
                    stringXMin = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    xMin = std::stod(stringXMin);
                }
                else if(count ==  1){
                    stringXMax = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    xMax = std::stod(stringXMax);
                }
                else if(count == 2){
                    stringYMin = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    yMin = std::stod(stringYMin);
                }
                else if(count == 3){
                    stringYMax = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    yMax = std::stod(stringYMax);
                }
                else if(count == 4){
                    stringTitle = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                }
                else if(count == 5){
                    stringXLabel = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                }
                else if(count == 6){
                    stringYLabel = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    QGraphicsTextItem *str0 = scene->addText(QString::fromStdString(stringXMin));
                    QGraphicsTextItem *str1 = scene->addText(QString::fromStdString(stringXMax));
                    QGraphicsTextItem *str2 = scene->addText(QString::fromStdString(stringYMin));
                    QGraphicsTextItem *str3 = scene->addText(QString::fromStdString(stringYMax));
                    QGraphicsTextItem *str4 = scene->addText(QString::fromStdString(stringTitle));
                    QGraphicsTextItem *str5 = scene->addText(QString::fromStdString(stringXLabel));
                    QGraphicsTextItem *str6 = scene->addText(QString::fromStdString(stringYLabel));
                    auto font = QFont("Monospace");
                    font.setStyleHint(QFont::TypeWriter);
                    font.setPointSize(1);
                    str0->setFont(font);
                    str1->setFont(font);
                    str2->setFont(font);
                    str3->setFont(font);
                    str4->setFont(font);
                    str5->setFont(font);
                    str6->setFont(font);
                    double xScale = N/(xMax-xMin);
                    double yScale = N/(yMax-yMin);
                    double scaledXMin = xMin*xScale;
                    double scaledXMax = xMax*xScale;
                    double scaledYMin = yMin*yScale;
                    double scaledYMax = yMax*yScale;
                    double ouXPos = (scaledXMin-C) - (str0->boundingRect().width()/2);
                    double ouYPos = (scaledYMin) - (str2->boundingRect().height()/2);
                    double olXPos = (scaledXMin-C) - (str0->boundingRect().width()/2);
                    double olYPos = (scaledYMax) - (str3->boundingRect().height()/2);
                    double alXPos = (scaledXMin) - (str0->boundingRect().width()/2);
                    double alYPos = (scaledYMax+C) - (str3->boundingRect().height()/2);
                    double auXPos = (scaledXMax) - (str1->boundingRect().width()/2);
                    double auYPos = (scaledYMax+C) - (str2->boundingRect().height()/2);
                    double titleXPos = ((scaledXMin+scaledXMax)/2) - (str4->boundingRect().width()/2);
                    double titleYPos = (scaledYMin-A) - (str4->boundingRect().height()/2);
                    double xLabelXPos = ((scaledXMin+scaledXMax)/2) - (str5->boundingRect().width()/2);
                    double xLabelYPos = (scaledYMax+A) - (str5->boundingRect().height()/2);
                    double yLabelXPos = (scaledXMin-B) - (str6->boundingRect().height()/2);
                    double yLabelYPos = ((scaledYMin+scaledYMax)/2) + (str6->boundingRect().width()/2);
                    std::cout << "xScale: " << xScale << std::endl;
                    std::cout << "yScale: " << yScale << std::endl;
                    std::cout << "scaledXMin: " << scaledXMin << std::endl;
                    std::cout << "scaledXMax: " << scaledXMax << std::endl;
                    std::cout << "scaledYMin: " << scaledYMin << std::endl;
                    std::cout << "scaledYMax: " << scaledYMax << std::endl;
                    std::cout << "titleXPos: " << titleXPos << std::endl;
                    std::cout << "titleYPos: " << titleYPos << std::endl;
                    std::cout << "xLabelXPos:" << xLabelXPos << std::endl;
                    std::cout << "xLabelYPos:" << xLabelYPos << std::endl;
                    std::cout << "yLabelXPos:" << yLabelXPos << std::endl;
                    std::cout << "yLabelYPos:" << yLabelYPos << std::endl;
                    str0->setPos(ouXPos,ouYPos);
                    str1->setPos(olXPos,olYPos);
                    str2->setPos(alXPos,alYPos);
                    str3->setPos(auXPos,auYPos);
                    str4->setPos(titleXPos,titleYPos);
                    str5->setPos(xLabelXPos,xLabelYPos);
                    str6->setPos(yLabelXPos,yLabelYPos);
                    str6->setRotation(-90);
                    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    count = 0;
                    stringXMin = "";
                    stringXMax = "";
                    stringYMin = "";
                    stringYMax = "";
                    stringTitle = "";
                    stringXLabel = "";
                    stringYLabel = "";
                    xMin = 0;
                    xMax = 0;
                    yMin = 0;
                    yMax = 0;
                    return;
                }
                count++;
            }
            else{
                scene->addText(QString::fromStdString((*e).makeString()));
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            }
        }
    }
}