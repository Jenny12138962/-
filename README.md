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
