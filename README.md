# -
六子棋代码
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPushButton>
#include <QMainWindow>
#include "GameModel.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 绘制
    void paintEvent(QPaintEvent *);
    void initGame();
    void initAIGame();

private:
    Ui::MainWindow *ui;
    GameModel *game; // 游戏指针
    GameType game_type; // 存储游戏类型
    std::vector<std::vector<int>> gameMapVec;
    bool isDown;
    void Init();
    void InitChess();

    int clickPosRow, clickPosCol; // 存储将点击的位置
    bool selectPos = false; // 是否移动到合适的位置，以选中摸个交叉点

protected:
    void mouseMoveEvent(QMouseEvent *event);
    // 实际落子
    void mouseReleaseEvent(QMouseEvent *event);
    void chessOneByPerson();

private slots:
    void chessOneByAI();
};
#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"GameModel.h"
#include<math.h>
#include <QPainter>
#include <QTimer>
//#include <QSound>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QWidget>
#include <QPushButton>
#include "mypushbutton.h"
#include <QPropertyAnimation>
//#define CHESS_ONE_SOUND ":/res/sound/chessone.wav"
//#define WIN_SOUND ":/res/sound/win.wav"
//#define LOSE_SOUND ":/res/sound/lose.wav"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(320,500);
    setWindowIcon(QIcon("E:/C++_codes/liuziqi/fengmian.jpg"));//(地址)
    setWindowTitle("六子棋");
    connect(ui->actionexit,&QAction::triggered,[=](){
        this->close();
    });
    mypushbutton * startBtn=new mypushbutton("E:/C++_codes/liuziqi/21.png");
    startBtn->setParent(this);
    startBtn->move(this->width()*0.5-startBtn->width()*0.5,this->height()*0.7);

    choosScene = new choosScene;

    connect(choosScene,&ChooseLevelScene::chooseSceneBack,this,[=](){
        choosScene->hide();
        this->show();
    });

    connect(startBtn,&mypushbutton::clicked,[=](){
        qDebug()<<"dianjikaishi";
        startBtn->zoom1();
        startBtn->zoom2();

        //延时进入
        QTimer::singleShot(500,this,[=](){
            this->hide();
            choosScene->show();

            //监听选择关卡的返回按钮的信号

        });
    });


    //jiantingtaojiezi,指定父对象，让其自动回收空间
    tcpServer=new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any,8888);
    connect(tcpServer,&QTcpServer::newConnection,
            [=]()
    {
        //quchu建立好连接的套接字
        tcpSocket=tcpServer->nextPendingConnection();
        //获取对方的IP和端口
        QString ip=tcpSocket->peerAddress().toString();
        qint16 port=tcpSocket->peerPort();
        QString temp=QString("[%1:%2]:成功连接").arg(ip).arg(port);
        ui->read->setText(temp);
        connect(tcpSocket,&QTcpSocket::readyRead,
                [=]()
        {
            //从通信套接字中取出内容
            QByteArray array=tcpSocket->readAll();
            ui->read->append(array);
        }
    };

});

    //设置棋盘大小
    setFixedSize(
                MARCIN*2+BLOCK_SIZE*BOARD_GRAD_SIZE,
                MARCIN*2+BLOCK_SIZE*BOARD_GRAD_SIZE);
    // 添加菜单
    QMenu *gameMenu = menuBar()->addMenu(tr("Game")); // menuBar默认是存在的，直接加菜单就可以了
    QAction *actionPVP = new QAction("Person VS Person", this);
    connect(actionPVP, SIGNAL(triggered()), this, SLOT(initPVPGame()));
    gameMenu->addAction(actionPVP);

    QAction *actionPVE = new QAction("Person VS Computer", this);
    connect(actionPVE, SIGNAL(triggered()), this, SLOT(initPVEGame()));
    gameMenu->addAction(actionPVE);

    initGame();
}


void MainWindow::mySlot(){
   // b1->setText("")
    b1.setText("person VS people √");

}
MainWindow::~MainWindow()
{
    if (game)
        {
            delete game;
            game = nullptr;
        }
}

void MainWindow::paintEvent(QPaintEvent *event){
    QPainter painter(this);//绘制棋盘
    painter.setRenderHint(QPainter::Antialiasing,true);

    for(int i=0;i<BOARD_GRAD_SIZE+1;i++){
        //从左到右，画竖线（i+1）
        painter.drawLine(MARCIN+BLOCK_SIZE*i,MARCIN,MARCIN+BLOCK_SIZE*i,size().height()-MARCIN);
        //从上到下，画横线（i+1）
        painter.drawLine(MARCIN,MARCIN+BLOCK_SIZE*i,size().width()-MARCIN,MARCIN+BLOCK_SIZE*i);
    }
    //绘制选中点
    QBrush brush;//定义一个刷子
    brush.setStyle(Qt::SolidPattern);//设置刷子的模式
    if(clickPosRow>0&&clickPosRow<BOARD_GRAD_SIZE&&clickPosCol>0&&clickPosCol<BOARD_GRAD_SIZE&&game->gameMapVec[clickPosCol][clickPosCol]==0){
        if(game->playerFlag)
            brush.setColor(Qt::black);
        else
            brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.drawRect(MARCIN+BLOCK_SIZE*clickPosCol-MARK_SIZE/2,MARCIN+BLOCK_SIZE*clickPosRow-MARK_SIZE/2,clickPosRow,clickPosRow);
    }
    //绘制棋子
    for(int i=0;i<BOARD_GRAD_SIZE;i++)
        for(int j=0;j<BOARD_GRAD_SIZE;j++){
            if(game->gameMapVec[i][j]==1){
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(MARCIN+BLOCK_SIZE*j-CHESS_RADIUS,MARCIN+BLOCK_SIZE*i-CHESS_RADIUS,33,33);
            }
            else if(game->gameMapVec[i][j]==-1){
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(MARCIN+BLOCK_SIZE*j-CHESS_RADIUS,MARCIN+BLOCK_SIZE*i-CHESS_RADIUS,33,33);
            }
        }
    // 判断输赢
    if (clickPosRow > 0 && clickPosRow < BOARD_GRAD_SIZE &&
            clickPosCol > 0 && clickPosCol < BOARD_GRAD_SIZE &&
            (game->gameMapVec[clickPosRow][clickPosCol] == 1 ||
             game->gameMapVec[clickPosRow][clickPosCol] == -1))
    {
        if (game->isWin(clickPosRow, clickPosCol) && game->gameStatus == PLAYING)
        {
            qDebug() << "win";
            game->gameStatus = WIN;
           // QSound::play(WIN_SOUND);
            QString str;
            if (game->gameMapVec[clickPosRow][clickPosCol] == 1)
                str = "black player";
            else if (game->gameMapVec[clickPosRow][clickPosCol] == -1)
                str = "white player";
            QMessageBox::StandardButton btnValue = QMessageBox::information(this, "congratulations", str + " win!");

            // 重置游戏状态，否则容易死循环
            if (btnValue == QMessageBox::Ok)
            {
                game->startGame(game_type);
                game->gameStatus = PLAYING;
            }
        }
    }
    // 判断死局
    if (game->isDeadGame())
    {
       // QSound::play(LOSE_SOUND);
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "oops", "和棋!");
        if (btnValue == QMessageBox::Ok)
        {
            game->startGame(game_type);
            game->gameStatus = PLAYING;
        }

    }
}


void MainWindow::initGame(){
    //初始化游戏模型
    game=new GameModel;
    initAIGame();
}
void MainWindow::initPVPGame()
{
    game_type =MAN;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}

void MainWindow::initAIGame(){
    game_type=AI;
    game->gameStatus=PLAYING;
    game->startGame(game_type);
    update();
}
void MainWindow::mouseMoveEvent(QMouseEvent *event){
    int x=event->x();
    int y=event->y();
    //棋盘边缘不能落子
    if(x>=MARCIN+BLOCK_SIZE/2&&x<size().width()-MARCIN-BLOCK_SIZE/2&&y>=MARCIN+BLOCK_SIZE/2&&y<size().height()-MARCIN-BLOCK_SIZE/2){
        //获取最近左上角的点
        int col=(x-MARCIN)/BLOCK_SIZE;
        int row=(y-MARCIN)/BLOCK_SIZE;

        int leftTopPosX=MARCIN+BLOCK_SIZE*col;
        int leftTopPosY=MARCIN+BLOCK_SIZE*row;
        //根据半径距离选择最近的点
        clickPosRow=-1;
        clickPosCol=-1;//初始化最终的值
        int len=0;
        selectPos=false;

        len=sqrt((x-leftTopPosX)*(x-leftTopPosX)+(y-leftTopPosY)*(y-leftTopPosY));
        if(len<POS_OFFSET){
            clickPosRow=row;
            clickPosCol=col;
            if(game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
        len=sqrt((x-leftTopPosX-BLOCK_SIZE)*(x-leftTopPosX-BLOCK_SIZE)+(y-leftTopPosY)*(y-leftTopPosY));
        if(len<POS_OFFSET){
            clickPosRow=row;
            clickPosCol=col+1;
            if(game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
        len=sqrt((x-leftTopPosX)*(x-leftTopPosX)+(y-leftTopPosY-BLOCK_SIZE)*(y-leftTopPosY-BLOCK_SIZE));
        if(len<POS_OFFSET){
            clickPosRow=row+1;
            clickPosCol=col;
            if(game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
        len=sqrt((x-leftTopPosX-BLOCK_SIZE)*(x-leftTopPosX-BLOCK_SIZE)+(y-leftTopPosY-BLOCK_SIZE)*(y-leftTopPosY-BLOCK_SIZE));
        if(len<POS_OFFSET){
            clickPosRow=row+1;
            clickPosCol=col+1;
            if(game->gameMapVec[clickPosRow][clickPosCol]==0){
                selectPos=true;
            }
        }
    }
    update();
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if(selectPos==false){
        return;
    }
    else{//在落子前，把落子标记再设置为false
        selectPos=false;
    }
   if(game_type==MAN)
        on_chessbyPerson_clicked();//人来下棋
    if(game_type==AI){
        //AI下棋
   QTimer::singleShot(AI_THINK_TIME, this, SLOT(chessOneByAI()));
    }
}


void MainWindow::on_chessbyPerson_clicked()
{
    //根据当前存储的来下棋子
    while (game->isWin(clickPosRow, clickPosCol)!=0 && game->gameStatus != PLAYING) {
        if(clickPosRow!=-1&&clickPosCol!=-1&&game->gameMapVec[clickPosRow][clickPosCol]==0){
            game->actionByPerson(clickPosRow,clickPosCol);//在游戏的数据模型中落子
            //播放落子音效，待实现；
            //   QSound::play(CHESS_ONE_SOUND);
        }
        update();
    }
}

void MainWindow::on_chessbyAI_clicked()
{
    while (game->isWin(clickPosRow, clickPosCol)!=0 && game->gameStatus != PLAYING){
        game->actionByAT(clickPosRow, clickPosCol);
        // QSound::play(":sound/chessone.wav");
        update();
    }
}

void MainWindow::on_AIbyAI_clicked()
{
    game->actionByAT(clickPosRow, clickPosCol);
    update();
}
#ifndef GAMEMODEL_H
#define GAMEMODEL_H

//六子棋游戏模型类//
#include<vector>
enum GameType{
    MAN,
    AI  //AI下棋
};

enum GameStatus{
    PLAYING,
    WIN,
    DEAD
};
//棋盘尺寸
const int BOARD_GRAD_SIZE=20;

const int MARCIN=30;//棋盘边缘空隙
const int CHESS_RADIUS=15;//棋子半径
const int MARK_SIZE=6;//落子标记边长
const int BLOCK_SIZE=45;//格子大小
const int POS_OFFSET=BLOCK_SIZE*0.4;//鼠标点击的模糊距离上限

const int AI_THINK_TIME=700;//AI下棋思考时间

class GameModel{
public:
    GameModel(){};
public:
    //存储当前游戏和棋子的情况，空白为0，黑子1，白子-1
    std::vector<std::vector<int>>gameMapVec;
    //各位评分情况（作为AI下棋依据）
    std::vector<std::vector<int>>scoreMapVec;
    //标示下棋方，true：黑  false：白
    bool playerFlag;
    GameType gameType;//游戏模式：人机还是双人
    GameStatus gameStatus;//游戏状态

    void startGame(GameType type);//开始游戏
    void calculateScore();//计算评分
    void actionByPerson(int row,int col);//人下棋
    void actionByAT(int &clickRow,int &clickCol);//机器下棋
    void updateGameMap(int row,int col);//更新棋盘
    bool isWin(int row,int col);//判断胜利
    bool isDeadGame();//判断是否和棋
};
#endif // GAMEMODEL_H
#include"GameModel.h"
#include <utility>
#include <stdlib.h>
#include <time.h>


void GameModel::startGame(GameType type){
    gameType=type;//初始棋盘

    gameMapVec.clear();
    for(int i=0;i<BOARD_GRAD_SIZE;i++){
        std::vector<int>lineBoard;
        for(int j=0;j<BOARD_GRAD_SIZE;j++)
            lineBoard.push_back(0);
        gameMapVec.push_back(lineBoard);
    }
    //如果是AI模式，需要初始化评分数组
    if(gameType==AI){
        scoreMapVec.clear();
        for(int i=0;i<BOARD_GRAD_SIZE;i++){
            std::vector<int>lineScores;
            for(int j=0;j<BOARD_GRAD_SIZE;j++)
                lineScores.push_back(0);
            scoreMapVec.push_back(lineScores);
        }
    }
    //黑棋为true,白棋为false
    playerFlag=true;

}
void GameModel::actionByPerson(int row,int col){
    updateGameMap(row,col);
}
void GameModel::updateGameMap(int row,int col){
    if(playerFlag)
        gameMapVec[row][col]=1;
    else
        gameMapVec[row][col]=-1;
    //换队友
    playerFlag=!playerFlag;
}

void GameModel::actionByAT(int &clickRow, int &clickCol)
{
    // 计算评分
    calculateScore();

    // 从评分中找出最大分数的位置
    int maxScore = 0;
    std::vector<std::pair<int, int>> maxPoints;

    for (int row = 1; row < BOARD_GRAD_SIZE; row++)
        for (int col = 1; col < BOARD_GRAD_SIZE; col++)
        {
            // 前提是这个坐标是空的
            if (gameMapVec[row][col] == 0)
            {
                if (scoreMapVec[row][col] > maxScore)          // 找最大的数和坐标
                {
                    maxPoints.clear();
                    maxScore = scoreMapVec[row][col];
                    maxPoints.push_back(std::make_pair(row, col));
                }
                else if (scoreMapVec[row][col] == maxScore)     // 如果有多个最大的数，都存起来
                    maxPoints.push_back(std::make_pair(row, col));
            }
        }

    // 随机落子，如果有多个点的话
    srand((unsigned)time(0));
    int index = rand() % maxPoints.size();

    std::pair<int, int> pointPair = maxPoints.at(index);
    clickRow = pointPair.first; // 记录落子点
    clickCol = pointPair.second;
    updateGameMap(clickRow, clickCol);
}

// 最关键的计算评分函数
void GameModel::calculateScore()
{
    // 统计玩家或者电脑连成的子
    int personNum = 0; // 玩家连成子的个数
    int botNum = 0; // AI连成子的个数
    int emptyNum = 0; // 各方向空白位的个数

    // 清空评分数组
    scoreMapVec.clear();
    for (int i = 0; i < BOARD_GRAD_SIZE; i++)
    {
        std::vector<int> lineScores;
        for (int j = 0; j < BOARD_GRAD_SIZE; j++)
            lineScores.push_back(0);
        scoreMapVec.push_back(lineScores);
    }

    // 计分（此处是完全遍历，其实可以用bfs或者dfs加减枝降低复杂度，通过调整权重值，调整AI智能程度以及攻守风格）
    for (int row = 0; row < BOARD_GRAD_SIZE; row++)
        for (int col = 0; col < BOARD_GRAD_SIZE; col++)
        {
            // 空白点就算
            if (row > 0 && col > 0 &&
                gameMapVec[row][col] == 0)
            {
                // 遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                    for (int x = -1; x <= 1; x++)
                    {
                        // 重置
                        personNum = 0;
                        botNum = 0;
                        emptyNum = 0;

                        // 原坐标不算
                        if (!(y == 0 && x == 0))
                        {
                            // 每个方向延伸4个子

                            // 对玩家白子评分（正反两个方向）
                            for (int i = 1; i <= 5; i++)
                            {
                                if (row + i * y > 0 && row + i * y < BOARD_GRAD_SIZE &&
                                    col + i * x > 0 && col + i * x < BOARD_GRAD_SIZE &&
                                    gameMapVec[row + i * y][col + i * x] == 1) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < BOARD_GRAD_SIZE &&
                                         col + i * x > 0 && col + i * x < BOARD_GRAD_SIZE &&
                                         gameMapVec[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 5; i++)
                            {
                                if (row - i * y > 0 && row - i * y < BOARD_GRAD_SIZE &&
                                    col - i * x > 0 && col - i * x < BOARD_GRAD_SIZE &&
                                    gameMapVec[row - i * y][col - i * x] == 1) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < BOARD_GRAD_SIZE &&
                                         col - i * x > 0 && col - i * x < BOARD_GRAD_SIZE &&
                                         gameMapVec[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (personNum == 1)                      // 杀二
                                scoreMapVec[row][col] += 10;
                            else if (personNum == 2)                 // 杀三
                            {
                                if (emptyNum == 1)
                                    scoreMapVec[row][col] += 30;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 40;
//                                else if (emptyNum == 3)
//                                    scoreMapVec[row][col] += 50;
                            }
                            else if (personNum == 3)                 // 杀四
                            {
                                // 量变空位不一样，优先级不一样
                                if (emptyNum == 1)
                                    scoreMapVec[row][col] += 60;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 110;
//                                else if (emptyNum == 3)
//                                    scoreMapVec[row][col] += 130;
                            }
                            else if (personNum == 4)  {               // 杀五
                                if (emptyNum == 1)
                                    scoreMapVec[row][col] += 80;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 150;
//                                else if (emptyNum == 3)
//                                    scoreMapVec[row][col] += 190;
                            }
                            else if (personNum == 5)                 // 杀六
                                scoreMapVec[row][col] += 10100;
                            // 进行一次清空
                            emptyNum = 0;

                            // 对AI黑子评分
                            for (int i = 1; i <= 5; i++)
                            {
                                if (row + i * y > 0 && row + i * y < BOARD_GRAD_SIZE &&
                                    col + i * x > 0 && col + i * x < BOARD_GRAD_SIZE &&
                                    gameMapVec[row + i * y][col + i * x] == 1) // 玩家的子
                                {
                                    botNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < BOARD_GRAD_SIZE &&
                                         col + i * x > 0 && col + i * x < BOARD_GRAD_SIZE &&
                                         gameMapVec[row +i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 5; i++)
                            {
                                if (row - i * y > 0 && row - i * y < BOARD_GRAD_SIZE &&
                                    col - i * x > 0 && col - i * x < BOARD_GRAD_SIZE &&
                                    gameMapVec[row - i * y][col - i * x] == -1) // AI的子
                                {
                                    botNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < BOARD_GRAD_SIZE &&
                                         col - i * x > 0 && col - i * x < BOARD_GRAD_SIZE &&
                                         gameMapVec[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (botNum == 0)                      // 普通下子
                                scoreMapVec[row][col] += 5;
                            else if (botNum == 1)                 // 活二
                                scoreMapVec[row][col] += 10;
                            else if (botNum == 2)
                            {
                                if (emptyNum == 1)                // 死三
                                    scoreMapVec[row][col] += 25;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 50;  // 活三
                            }
                            else if (botNum == 3)
                            {
                                if (emptyNum == 1)                // 死四
                                    scoreMapVec[row][col] += 55;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 100; // 活四
                            }
                            else if (botNum == 4)
                            {
                                if (emptyNum == 1)                // 死四
                                    scoreMapVec[row][col] += 105;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 210; // 活四
                            }
                            else if (botNum >= 5)
                                scoreMapVec[row][col] += 10000;   // 活五

                        }
                    }

            }
        }
}

bool GameModel::isWin(int row, int col)
{
    // 横竖斜四种大情况，每种情况都根据当前落子往后遍历5个棋子，有一种符合就算赢
    // 水平方向
    for (int i = 0; i < 6; i++)
    {
        // 往左6个，往右匹配5个子，20种情况
        if (col - i > 0 &&
            col - i + 5 < BOARD_GRAD_SIZE &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 1] &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 2] &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 3] &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 4] &&
            gameMapVec[row][col - i] == gameMapVec[row][col - i + 5])
            return true;
    }

    // 竖直方向(上下延伸5个)
    for (int i = 0; i < 6; i++)
    {
        if (row - i > 0 &&
            row - i + 5 < BOARD_GRAD_SIZE &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 1][col] &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 2][col] &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 3][col] &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 4][col] &&
            gameMapVec[row - i][col] == gameMapVec[row - i + 5][col])
            return true;
    }

    // 左斜方向
    for (int i = 0; i < 6; i++)
    {
        if (row + i < BOARD_GRAD_SIZE &&
            row + i - 5 > 0 &&
            col - i > 0 &&
            col - i +5 < BOARD_GRAD_SIZE &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 1][col - i + 1] &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 2][col - i + 2] &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 3][col - i + 3] &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 4][col - i + 4] &&
            gameMapVec[row + i][col - i] == gameMapVec[row + i - 5][col - i + 5])
            return true;
    }

    // 右斜方向
    for (int i = 0; i < 6; i++)
    {
        if (row - i > 0 &&
            row - i + 5 < BOARD_GRAD_SIZE &&
            col - i > 0 &&
            col - i + 5 < BOARD_GRAD_SIZE &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 1][col - i + 1] &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 2][col - i + 2] &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 3][col - i + 3] &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 4][col - i + 4] &&
            gameMapVec[row - i][col - i] == gameMapVec[row - i + 5][col - i + 5])
            return true;
    }

    return false;
}

bool GameModel::isDeadGame()//判断和棋

{
    // 所有空格全部填满
    for (int i = 1; i < BOARD_GRAD_SIZE; i++)
        for (int j = 1; j < BOARD_GRAD_SIZE; j++)
        {
            if (!(gameMapVec[i][j] == 1 || gameMapVec[i][j] == -1))
                return false;
        }
    return true;
}


