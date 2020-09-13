#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPushButton>
#include <QMainWindow>
#include "GameModel.h"
#include <QNetworkInterface>
#include <QTcpServer>
#include <QHostAddress>
#include <QTcpSocket>
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
    void inGame();
    void inAIGame();

private:
    Ui::MainWindow *ui;
    std::vector<std::vector<int>> gameMapVec;
    void Init();
    void InitChess();
    GameModel *game; // 游戏指针
    GameType game_type; // 存储游戏类型
    int Rowclick, Colclick; // 存储将点击的位置
    bool sPos = false; // 是否移动到合适的位置，以选中摸个交叉点
    QTcpServer *ser;
protected:
    void mouseMoveEvent(QMouseEvent *event);
    // 实际落子
    void mouseReleaseEvent(QMouseEvent *event);
    void chessOneByPerson();

private slots:

    void newCon();
    void dealData();
    void chessOneByAI();
    void on_btn_PVP_clicked();
    void on_btn_PVC_clicked();
    void on_btn_CVC_clicked();
};
#endif // MAINWINDOW_H
