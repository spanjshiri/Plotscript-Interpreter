#include "output_widget.hpp"

OutputWidget::~OutputWidget(){
    std::string empty;
    inputQueue->push(empty);
    consumer_th1.join();
    if(!inputQueue->empty()){
        inputQueue->wait_and_pop(empty);
    }
}

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
    tempInterp = interp;
    con = Consumer(inputQueue,outputQueue);
    consumer_th1 = std::thread(con,interp);
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(recieveTimerSignal()));
    timer->start(0);
}

void OutputWidget::recieveStartSignal(){
    if(con.threadStarted() == 0){
        consumer_th1 = std::thread(con,interp);
        con.setstartedThread();
    }
    return;
}

void OutputWidget::recieveStopSignal(){
    std::string empty;
      if(con.threadStarted() == 1){
        con.setstoppedThread();
        inputQueue->push(empty);
        if(consumer_th1.joinable()){
            consumer_th1.join();
        }
        if(!inputQueue->empty()){
          inputQueue->wait_and_pop(empty);
        }
      }
      return;
}

void OutputWidget::recieveResetSignal(){
    std::string empty;
    if(consumer_th1.joinable()){
        con.setstoppedThread();
        inputQueue->push(empty);
        consumer_th1.join();
        if(!inputQueue->empty()){
        inputQueue->wait_and_pop(empty);
        }
    }
    if(con.threadStarted() == 0){
        consumer_th1 = std::thread(con,interp);
        con.setstartedThread();
    }
    interp = tempInterp;
    return;
}

void OutputWidget::recieveInterruptSignal(){
    global_status_flag+=1;
}

void OutputWidget::recieveTimerSignal(){
    if(outputQueue->try_pop(tempPair)){
        // std::istringstream expression(str.toStdString());
            std::string errorString = tempPair.first;
            if(errorString.length() > 0){
                scene->clear();
                scene->addText(QString::fromStdString(errorString));
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            }
            else{
                try{
                    Expression exp = tempPair.second;
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
                        //str->setPos(x, y);
                        QRectF strRect = str->sceneBoundingRect();
                        QPointF centerText = QPointF(x-(strRect.width()/2),y-(strRect.height()/2));
                        str->setPos(centerText);
                        // std::cout << "xPos of Single Text: " << x-(strRect.width()/2) << std::endl;
                        // std::cout << "yPos of Single Text: " << y-(strRect.height()/2) << std::endl;
                        QPointF newCenter = str->boundingRect().center();
                        str->setTransformOriginPoint(newCenter);
                        str->setScale(scale);
                        str->setRotation(rotation*(180/M_PI));
                        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        singleTextPrinted = true;
                        return;
                    }
                    if(exp.head().isContinuous()){
                        printList(exp);
                    }
                    else if(exp.isHeadList() || exp.head().isDiscrete()/* || exp.head().isContinuous()*/){
                        if (exp.makeTail().size() >= 10){
                            exp.head().setDiscretePlot();
                        }
                        printList(exp);
                    }
                    else if(exp.head().isLambda()){
                        return;
                    }
                    else {
                        scene->addText(QString::fromStdString(exp.makeString()));
                        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    }
                }
                catch(const SemanticError & ex){
                    scene->clear();
                    scene->addText(QString(ex.what()));
                    view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                }
            }
        }return;
}

void OutputWidget::recieveText(QString str){
    global_status_flag = 0;
    if(con.threadStarted() == 0){
        scene->clear();
        scene->addText("Error: interpreter kernel not running");
        view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        return;
    }
    inputQueue->push(str.toStdString());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void OutputWidget::printList(Expression exp){
    if(exp.isPoint()){
        std::vector<Expression> tail = exp.makeTail();
        double w = exp.getSize();
        double h = exp.getSize();
        double x = tail[0].head().asNumber();
        double y = tail[1].head().asNumber();
        if(exp.head().isDiscrete()){
            w = .5;
            h = .5;
        }
        QRectF values = QRectF(x,y,w,h);
        values.moveCenter(QPointF(x,y));
        const QPen pen = QPen(Qt::NoPen);
        const QBrush brush = QBrush(Qt::black);
        scene->QGraphicsScene::addEllipse(values,pen,brush);
        // std::cout << "X Value of Single Point: " << x << std::endl;
        // std::cout << "Y Value of Single Point: " << y << std::endl;
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
        if(exp.head().isDiscrete() || exp.head().isContinuous()){
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
                double x = tail[0].head().asNumber();
                double y = tail[1].head().asNumber();
                if(exp.head().isDiscrete()){
                    w = .5;
                    h = .5;
                }
                QRectF values = QRectF(x,y,w,h);
                values.moveCenter(QPointF(x,y));
                const QPen pen = QPen(Qt::NoPen);
                const QBrush brush = QBrush(Qt::black);
                scene->QGraphicsScene::addEllipse(values,pen,brush);
                // std::cout << "X Value of Multi Point: " << x << std::endl;
                // std::cout << "Y Value of Multi Point: " << y << std::endl;
                // std::cout << "Size Value of Multi Point: " << w << std::endl;
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
                if(exp.head().isDiscrete() || exp.head().isContinuous()){
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
                // double scale = exp.getTextScale();
                // double rotation = exp.getTextRotation();
                std::string text = (*e).head().asString();
                std::string subText = text.substr(1,text.length()-2);
                QGraphicsTextItem *str = scene->addText(QString::fromStdString(subText));
                //str->setPos(x, y);
                auto font = QFont("Monospace");
                font.setStyleHint(QFont::TypeWriter);
                font.setPointSize(1);
                str->setFont(font);
                QRectF strRect = str->sceneBoundingRect();
                QPointF centerText = QPointF(x-(strRect.width()/2),y-(strRect.height()/2));
                str->setPos(centerText);
                QPointF newCenter = str->boundingRect().center();
                str->setTransformOriginPoint(newCenter);
                // str->setScale(scale);
                // str->setRotation(rotation*(180/M_PI));
                view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
                view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
                    QGraphicsTextItem *str0 = scene->addText(QString::fromStdString(stringYMin));
                    QGraphicsTextItem *str1 = scene->addText(QString::fromStdString(stringYMax));
                    QGraphicsTextItem *str2 = scene->addText(QString::fromStdString(stringXMin));
                    QGraphicsTextItem *str3 = scene->addText(QString::fromStdString(stringXMax));
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
                    double yScale = (N/(yMax-yMin));
                    double scaledXMin = xMin*xScale;
                    double scaledXMax = xMax*xScale;
                    double scaledYMin = -1*yMin*yScale;
                    double scaledYMax = -1*yMax*yScale;
                    double ouXPos = (scaledXMin-C) - (str0->boundingRect().width()/2);
                    double ouYPos = (scaledYMin) - (str2->boundingRect().height()/2);
                    double olXPos = (scaledXMin-C) - (str1->boundingRect().width()/2);
                    double olYPos = (scaledYMax) - (str3->boundingRect().height()/2);
                    double alXPos = (scaledXMin) - (str2->boundingRect().width()/2);
                    double alYPos = (scaledYMin+C) - (str3->boundingRect().height()/2);
                    double auXPos = (scaledXMax) - (str1->boundingRect().width()/2);
                    double auYPos = (scaledYMin+C) - (str2->boundingRect().height()/2);
                    double titleXPos = ((scaledXMin+scaledXMax)/2) - (str4->boundingRect().width()/2);
                    double titleYPos = (scaledYMax-A) - (str4->boundingRect().height()/2);
                    double xLabelXPos = ((scaledXMin+scaledXMax)/2) - (str5->boundingRect().width()/2);
                    double xLabelYPos = (scaledYMin+A) - (str5->boundingRect().height()/2);
                    double yLabelXPos = (scaledXMin-B) - (str6->boundingRect().width()/2);
                    double yLabelYPos = ((scaledYMin+scaledYMax)/2) - (str6->boundingRect().height()/2);
                    // std::cout << "str.width/2: " << (str2->boundingRect().width()/2) << std::endl;
                    // std::cout << "xScale: " << xScale << std::endl;
                    // std::cout << "yScale: " << yScale << std::endl;
                    // std::cout << "scaledXMin: " << scaledXMin << std::endl;
                    // std::cout << "scaledXMax: " << scaledXMax << std::endl;
                    // std::cout << "scaledYMin: " << scaledYMin << std::endl;
                    // std::cout << "scaledYMax: " << scaledYMax << std::endl;
                    // std::cout << "titleXPos: " << titleXPos << std::endl;
                    // std::cout << "titleYPos: " << titleYPos << std::endl;
                    // std::cout << "xLabelXPos:" << xLabelXPos << std::endl;
                    // std::cout << "xLabelYPos:" << xLabelYPos << std::endl;
                    // std::cout << "yLabelXPos:" << yLabelXPos << std::endl;
                    // std::cout << "yLabelYPos:" << yLabelYPos << std::endl;
                    // std::cout << "ouXPos: " << olXPos << std::endl;
                    // std::cout << "ouYPos:" << olYPos << std::endl;
                    // std::cout << "olXPos: " << ouXPos << std::endl;
                    // std::cout << "olYPos:" << ouYPos << std::endl;
                    // std::cout << "auXPos: " << auXPos << std::endl;
                    // std::cout << "auYPos:" << auYPos << std::endl;
                    // std::cout << "alXPos: " << alXPos << std::endl;
                    // std::cout << "alYPos:" << alYPos << std::endl;
                    str0->setPos(ouXPos,ouYPos);
                    str1->setPos(olXPos,olYPos);
                    str2->setPos(alXPos,alYPos);
                    str3->setPos(auXPos,auYPos);
                    str4->setPos(titleXPos,titleYPos);
                    str5->setPos(xLabelXPos,xLabelYPos);
                    // QRectF strRect = str6->sceneBoundingRect();
                    QPointF centerText = QPointF(yLabelXPos,yLabelYPos);
                    str6->setPos(centerText);
                    QPointF newCenter = str6->boundingRect().center();
                    str6->setTransformOriginPoint(newCenter);
                    // str6->setPos(yLabelXPos,yLabelYPos);
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
            else if(exp.head().isContinuous()){
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
                    // std::cout << "stringXMin: " << stringXMin << std::endl;
                    // std::cout << "xMin: " << xMin << std::endl;
                }
                else if(count ==  1){
                    stringXMax = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    xMax = std::stod(stringXMax);
                    // std::cout << "stringXMax: " << stringXMax << std::endl;
                    // std::cout << "xMax: " << xMax << std::endl;
                    
                }
                else if(count == 2){
                    stringYMin = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    yMin = std::stod(stringYMin);
                    // std::cout << "stringYMin: " << stringYMin << std::endl;
                    // std::cout << "yMin: " << yMin << std::endl;
                }
                else if(count == 3){
                    stringYMax = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    yMax = std::stod(stringYMax);
                    // std::cout << "stringYMax: " << stringYMax << std::endl;
                    // std::cout << "yMax: " << yMax << std::endl;
                    if((e+1) == exp.tailConstEnd()){
                        QGraphicsTextItem *str0 = scene->addText(QString::fromStdString(stringYMin));
                        QGraphicsTextItem *str1 = scene->addText(QString::fromStdString(stringYMax));
                        QGraphicsTextItem *str2 = scene->addText(QString::fromStdString(stringXMin));
                        QGraphicsTextItem *str3 = scene->addText(QString::fromStdString(stringXMax));
                        auto font = QFont("Monospace");
                        font.setStyleHint(QFont::TypeWriter);
                        font.setPointSize(1);
                        str0->setFont(font);
                        str1->setFont(font);
                        str2->setFont(font);
                        str3->setFont(font);
                        double xScale = N/(xMax-xMin);
                        double yScale = (N/(yMax-yMin));
                        double scaledXMin = xMin*xScale;
                        double scaledXMax = xMax*xScale;
                        double scaledYMin = -1*yMin*yScale;
                        double scaledYMax = -1*yMax*yScale;
                        double ouXPos = (scaledXMin-C) - (str0->boundingRect().width()/2);
                        double ouYPos = (scaledYMin) - (str2->boundingRect().height()/2);
                        double olXPos = (scaledXMin-C) - (str1->boundingRect().width()/2);
                        double olYPos = (scaledYMax) - (str3->boundingRect().height()/2);
                        double alXPos = (scaledXMin) - (str2->boundingRect().width()/2);
                        double alYPos = (scaledYMin+C) - (str3->boundingRect().height()/2);
                        double auXPos = (scaledXMax) - (str1->boundingRect().width()/2);
                        double auYPos = (scaledYMin+C) - (str2->boundingRect().height()/2);
                        str0->setPos(ouXPos,ouYPos);
                        str1->setPos(olXPos,olYPos);
                        str2->setPos(alXPos,alYPos);
                        str3->setPos(auXPos,auYPos);
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
                }
                else if(count == 4){
                    stringTitle = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                }
                else if(count == 5){
                    stringXLabel = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                }
                else if(count == 6){
                    stringYLabel = (*e).head().asString().substr(1,(*e).head().asString().length()-2);
                    QGraphicsTextItem *str0 = scene->addText(QString::fromStdString(stringYMin));
                    QGraphicsTextItem *str1 = scene->addText(QString::fromStdString(stringYMax));
                    QGraphicsTextItem *str2 = scene->addText(QString::fromStdString(stringXMin));
                    QGraphicsTextItem *str3 = scene->addText(QString::fromStdString(stringXMax));
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
                    double yScale = (N/(yMax-yMin));
                    double scaledXMin = xMin*xScale;
                    double scaledXMax = xMax*xScale;
                    double scaledYMin = -1*yMin*yScale;
                    double scaledYMax = -1*yMax*yScale;
                    double ouXPos = (scaledXMin-C) - (str0->boundingRect().width()/2);
                    double ouYPos = (scaledYMin) - (str2->boundingRect().height()/2);
                    double olXPos = (scaledXMin-C) - (str1->boundingRect().width()/2);
                    double olYPos = (scaledYMax) - (str3->boundingRect().height()/2);
                    double alXPos = (scaledXMin) - (str2->boundingRect().width()/2);
                    double alYPos = (scaledYMin+C) - (str3->boundingRect().height()/2);
                    double auXPos = (scaledXMax) - (str1->boundingRect().width()/2);
                    double auYPos = (scaledYMin+C) - (str2->boundingRect().height()/2);
                    double titleXPos = ((scaledXMin+scaledXMax)/2) - (str4->boundingRect().width()/2);
                    double titleYPos = (scaledYMax-A) - (str4->boundingRect().height()/2);
                    double xLabelXPos = ((scaledXMin+scaledXMax)/2) - (str5->boundingRect().width()/2);
                    double xLabelYPos = (scaledYMin+A) - (str5->boundingRect().height()/2);
                    double yLabelXPos = (scaledXMin-B) - (str6->boundingRect().width()/2);
                    double yLabelYPos = ((scaledYMin+scaledYMax)/2) - (str6->boundingRect().height()/2);
                    // std::cout << "str.width/2: " << (str2->boundingRect().width()/2) << std::endl;
                    // std::cout << "xScale: " << xScale << std::endl;
                    // std::cout << "yScale: " << yScale << std::endl;
                    // std::cout << "scaledXMin: " << scaledXMin << std::endl;
                    // std::cout << "scaledXMax: " << scaledXMax << std::endl;
                    // std::cout << "scaledYMin: " << scaledYMin << std::endl;
                    // std::cout << "scaledYMax: " << scaledYMax << std::endl;
                    // std::cout << "titleXPos: " << titleXPos << std::endl;
                    // std::cout << "titleYPos: " << titleYPos << std::endl;
                    // std::cout << "xLabelXPos:" << xLabelXPos << std::endl;
                    // std::cout << "xLabelYPos:" << xLabelYPos << std::endl;
                    // std::cout << "yLabelXPos:" << yLabelXPos << std::endl;
                    // std::cout << "yLabelYPos:" << yLabelYPos << std::endl;
                    // std::cout << "ouXPos: " << olXPos << std::endl;
                    // std::cout << "ouYPos:" << olYPos << std::endl;
                    // std::cout << "olXPos: " << ouXPos << std::endl;
                    // std::cout << "olYPos:" << ouYPos << std::endl;
                    // std::cout << "auXPos: " << auXPos << std::endl;
                    // std::cout << "auYPos:" << auYPos << std::endl;
                    // std::cout << "alXPos: " << alXPos << std::endl;
                    // std::cout << "alYPos:" << alYPos << std::endl;
                    str0->setPos(ouXPos,ouYPos);
                    str1->setPos(olXPos,olYPos);
                    str2->setPos(alXPos,alYPos);
                    str3->setPos(auXPos,auYPos);
                    str4->setPos(titleXPos,titleYPos);
                    str5->setPos(xLabelXPos,xLabelYPos);
                    // QRectF strRect = str6->sceneBoundingRect();
                    QPointF centerText = QPointF(yLabelXPos,yLabelYPos);
                    str6->setPos(centerText);
                    QPointF newCenter = str6->boundingRect().center();
                    str6->setTransformOriginPoint(newCenter);
                    // str6->setPos(yLabelXPos,yLabelYPos);
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