#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLayout>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QtMath>
#include <QTextBlockFormat>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include <utility>
#include "expression.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "startup_config.hpp"
#include "message_queue.hpp"

typedef MessageQueue<std::string> imq;
typedef MessageQueue<std::pair<std::string,Expression>> omq;

class Consumer {
public:
  Consumer(){
    id = 1;
  }
  Consumer(imq *inputQueuePtr, omq *outputQueuePtr, int identifier = 1)
  {
    inputQueue = inputQueuePtr;
    outputQueue = outputQueuePtr;
    id = identifier;
  }
  bool runStatus(bool status){
    return status;
  }
  void changeRunStatus(){
    status = false;
  }
  void operator()(Interpreter i)
  {
    while(status == true){
      Expression tempExp;
      std::string tempStr;
      std::string errStr;
      inputQueue->wait_and_pop(tempStr);
      std::istringstream expression(tempStr);
      if(tempStr == ""){
        changeRunStatus();
        return;
      }
      if(!i.parseStream(expression)){
        tempStr = "Invalid Program. Could not parse.";
      }
      else{
        try{
          Expression exp = i.evaluate();
          tempExp = exp;
        }
        catch(const SemanticError & ex){
          errStr = ex.what();
        }	
      }
      std::pair<std::string,Expression> tempPair = {errStr, tempExp};

      outputQueue->push(tempPair);
    }
  }
  int threadStarted(){
    return id;
  }
  void setstartedThread(){
    id = 1;
  }
  void setstoppedThread(){
    id = 0;
  }
private:
  imq *inputQueue;
  omq *outputQueue;
  int id;
  bool status = true;
};

class OutputWidget : public QWidget{
Q_OBJECT

public:
    OutputWidget(QWidget * parent = nullptr);
    ~OutputWidget();
    void printList(Expression exp);
    
private slots:
    void recieveText(QString str);
    void recieveStartSignal();
    void recieveStopSignal();
    void recieveResetSignal();
    void recieveInterruptSignal();

private:
bool shouldClear = false;
bool singleTextPrinted = false;
QGraphicsScene * scene;
QGraphicsView * view;
Interpreter interp;
imq *inputQueue = new imq;
omq *outputQueue = new omq;
Consumer con;
std::thread consumer_th1;
};

#endif