#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("五子棋大战");
    this->setFixedSize(800,600);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    game = new Game(1);
    this->hide();
    game->show();
}

void Widget::on_pushButton_2_clicked()
{
    game = new Game(2);
    this->hide();
    game->show();
}
