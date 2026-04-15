#include "widget.h"
#include "ui_widget.h"

#include <QDebug>

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


void Widget::on_pushButton_clicked()
{
    if(ui->pushButton->text() == "开灯")
    {
        myled.on(fsmpLeds::LED1);

        ui->pushButton->setText("关灯");
    }
    else
    {
        myled.off(fsmpLeds::LED1);
        ui->pushButton->setText("开灯");
    }
}


void Widget::on_pushButton_2_clicked()
{
    qDebug() << "真的假的" << endl;
}

