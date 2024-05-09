#include "widget.h"
#include "ui_widget.h"

#include <QMouseEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);    
}
Widget::~Widget()
{
    delete ui;
}


void Widget::mousePressEvent(QMouseEvent *event)
{
    diff_ = event->globalPos() - this->pos();
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    diff_ = QPoint();
}
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (!diff_.isNull()) {
        QPoint newPos = event->globalPos() - diff_;
        this->move(newPos);
    }
}
