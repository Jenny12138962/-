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
