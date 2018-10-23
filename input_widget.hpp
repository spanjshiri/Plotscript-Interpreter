#ifndef INPUT_WIDGET_H
#define INPUT_WIDGET_H

#include <QWidget>
#include <QApplication>
#include <QPlainTextEdit>
#include <QSet>
#include <QLayout>
#include <QDebug>

class InputWidget: public QPlainTextEdit{
Q_OBJECT

public:
    InputWidget(QWidget * parent = nullptr);

signals:
    void sendText(QString str);

private slots:

private:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif
