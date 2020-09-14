#ifndef GAMEMODEL_H
#define GAMEMODEL_H

// ---- 五子棋游戏模型类 ---- //
#include <vector>

// 游戏类型，双人还是AI（目前固定让AI下黑子）
enum GameType
{
    PERSON,
    AI  //人机对弈模式
};

// 游戏状态
enum GameStatus
{
    PLAYING,
    WIN,
    DEAD
};
const int AITHITIME =20; // AI下棋的思考时间
// 棋盘尺寸
const int GRADSIZE = 20;
const int CHESSR = 15; // 棋子半径
const int MSIZE = 6; // 落子标记边长
const int BLOCK_SIZE = 35; // 格子的大小
const int OFFSET = BLOCK_SIZE * 0.4; // 20 鼠标点击的模糊距离上限
const int M = 100; // 棋盘边缘空隙
class GameModel
{
public:
    GameModel();

public:
    // 标示下棋方, true:黑棋方  false: AI 白棋方（AI方）
    bool player;

    // 游戏模式：人机对弈，还是双人
    GameType gameType;

    // 存储当前游戏棋盘和棋子的情况,空白为0，黑子1，白子-1
    std::vector<std::vector<int>> mapGV;

    // 存储各个点位的评分情况，作为AI下棋依据
    std::vector<std::vector<int>> mapSV;

    // 游戏状态
    GameStatus gameStatus;

    void InitChess();

    void start(GameType type); // 开始游戏

    void actionMan(int row, int col);

    void updateMap(int row, int col);

    bool winorlose(int row, int col); //判断输赢
    void cScore();
    void actionAI(int &row, int &col); // 机器执行下棋
    bool isDeadGame();//判断是否和棋
};

#endif // GAMEMODEL_H



