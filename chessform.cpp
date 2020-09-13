#include "chessform.h"
#include"ui_chessform.h"
#include"mainwindow.h"
#include"ui_mainwindow.h"
#include<QPainter>
#include <QMouseEvent>
#include <math.h>
#include <QMessageBox>
#include<QDebug>
#include <QTimer>
#include"GameModel.h"
#include <QTcpSocket>
#include<QTcpServer>
#include <QSound>
ChessForm::ChessForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChessForm)
{
    ui->setupUi(this);
    Init();
}

void ChessForm::Init(){
    isDown=false;
//    mySocket=new QUdpSocket(this);


    myChess=new MainWindow();

    ui->gridLayout->addWidget(myChess);
}

void ChessForm::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.drawPixmap(this->rect(),
                       QPixmap("E:/server/liuziqipVSai/pic/fengmian.jpg"));
}

ChessForm::~ChessForm()
{
    delete ui;
}


