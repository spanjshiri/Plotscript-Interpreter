#include <QTest>
#include "notebook_app.hpp"
/* 
findLines - find lines in a scene contained within a bounding box 
            with a small margin
 */
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin){

  QPainterPath selectPath;

  QMarginsF margins(margin, margin, margin, margin);
  selectPath.addRect(bbox.marginsAdded(margins));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);
  
  int numlines(0);
  foreach(auto item, scene->selectedItems()){
    if(item->type() == QGraphicsLineItem::Type){
      numlines += 1;
    }
  }

  return numlines;
}

/* 
findPoints - find points in a scene contained within a specified rectangle
 */
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius){
  
  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

  int numpoints(0);
  foreach(auto item, scene->selectedItems()){
    if(item->type() == QGraphicsEllipseItem::Type){
      numpoints += 1;
    }
  }

  return numpoints;
}

/* 
findText - find text in a scene centered at a specified point with a given 
           rotation and string contents  
 */
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents){
  
  int numtext(0);
  foreach(auto item, scene->items(center)){
    if(item->type() == QGraphicsTextItem::Type){
      QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
      if((text->toPlainText() == contents) &&
     (text->rotation() == rotation) &&
     (text->pos() + text->boundingRect().center() == center)){
    numtext += 1;
      }
    }
  }

  return numtext;
}

/* 
intersectsLine - find lines in a scene that intersect a specified rectangle
 */
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius){
              
  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius));
  scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

  int numlines(0);
  foreach(auto item, scene->selectedItems()){
    if(item->type() == QGraphicsLineItem::Type){
      numlines += 1;
    }
  }

  return numlines;
}

class NotebookTest : public QObject {
  Q_OBJECT

private slots:
 void initTestCase();
 void testDiscretePlotLayout();
 void testContinuousPlotLayout();

private:
  InputWidget *inputWidget;
  OutputWidget *outputWidget;
  NotebookApp widget;

  // TODO: implement additional tests here
  

};

void NotebookTest::initTestCase(){
  inputWidget = widget.findChild<InputWidget*>("input");
  outputWidget = widget.findChild<OutputWidget*>("output");
}

void NotebookTest::testDiscretePlotLayout() {

  std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") )))";

  inputWidget->setPlainText(QString::fromStdString(program));
  QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

  auto view = outputWidget->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  // first check total number of items
  // 8 lines + 2 points + 7 text = 17
  auto items = scene->items();
  QCOMPARE(items.size(), 17);

  // make them all selectable
  foreach(auto item, items){
    item->setFlag(QGraphicsItem::ItemIsSelectable);
  }

  double scalex = 20.0/2.0;
  double scaley = 20.0/2.0;

  double xmin = scalex*-1;
  double xmax = scalex*1;
  double ymin = scaley*-1;
  double ymax = scaley*1;
  double xmiddle = (xmax+xmin)/2;
  double ymiddle = (ymax+ymin)/2;
    
  // check title
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax+3)), 0, QString("The Title")), 1);
  
  // check abscissa label
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin-3)), 0, QString("X Label")), 1);
  
  // check ordinate label
  QCOMPARE(findText(scene, QPointF(xmin-3, -ymiddle), -90, QString("Y Label")), 1);

  // check abscissa min label
  QCOMPARE(findText(scene, QPointF(xmin, -(ymin-2)), 0, QString("-1")), 1);

  // check abscissa max label
  QCOMPARE(findText(scene, QPointF(xmax, -(ymin-2)), 0, QString("1")), 1);

  // check ordinate min label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymin), 0, QString("-1")), 1);

  // check ordinate max label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymax), 0, QString("1")), 1);

  // check the bounding box bottom
  QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

  // check the bounding box top
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

  // check the bounding box left and (-1, -1) stem
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

  // check the bounding box right and (1, 1) stem
  QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

  // check the abscissa axis
  QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

  // check the ordinate axis 
  QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);
  
  // check the point at (-1,-1)
  QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);
    
  // check the point at (1,1)
  QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1); 
}

void NotebookTest::testContinuousPlotLayout() {

  std::string program = "(begin (define f (lambda (x) (+ (* 2 x) 1))) (continuous-plot f (list -2 2) (list (list \"title\" \"A continuous linear function\") (list \"abscissa-label\" \"x\") (list \"ordinate-label\" \"y\"))))";

  inputWidget->setPlainText(QString::fromStdString(program));
  QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

  auto view = outputWidget->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  // first check total number of items
  // 8 lines + 2 points + 7 text = 17
  auto items = scene->items();
  QCOMPARE(items.size(), 64);

  // make them all selectable
  foreach(auto item, items){
    item->setFlag(QGraphicsItem::ItemIsSelectable);
  }

  double scalex = 20.0/4.0;
  double scaley = 20.0/8.0;

  double xmin = scalex*-2;
  double xmax = scalex*2;
  double ymin = scaley*-3;
  double ymax = scaley*5;
  double xmiddle = (xmax+xmin)/2;
  double ymiddle = (ymax+ymin)/2;
    
  // check title
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax+3)), 0, QString("A continuous linear function")), 1);
  
  // check abscissa label
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin-3)), 0, QString("x")), 1);
  
  // check ordinate label
  QCOMPARE(findText(scene, QPointF(xmin-3, -ymiddle), -90, QString("y")), 1);

  // check abscissa min label
  QCOMPARE(findText(scene, QPointF(xmin, -(ymin-2)), 0, QString("-2")), 1);

  // check abscissa max label
  QCOMPARE(findText(scene, QPointF(xmax, -(ymin-2)), 0, QString("2")), 1);

  // check ordinate min label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymin), 0, QString("-3")), 1);

  // check ordinate max label
  QCOMPARE(findText(scene, QPointF(xmin-2, -ymax), 0, QString("5")), 1);

  // // check the bounding box bottom
  // QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

  // // check the bounding box top
  // QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

  // // check the bounding box left and (-1, -1) stem
  // QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

  // // check the bounding box right and (1, 1) stem
  // QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

  // check the abscissa axis
  QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

  // check the ordinate axis 
  QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);
  
  // // check the point at (-1,-1)
  // QCOMPARE(findPoints(scene, QPointF(-10, 12.5), 0.6), 1);
    
  // // check the point at (1,1)
  // QCOMPARE(findPoints(scene, QPointF(10, -12.5), 0.6), 1); 
}


QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
