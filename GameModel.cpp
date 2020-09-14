#include "GameModel.h"
#include <time.h>
#include <stdlib.h>

GameModel::GameModel()
{
}

void GameModel::start(GameType type)
{
    gameType = type;
    // 初始棋盘
    InitChess();
    // 如果是AI模式，需要初始化评分数组
    if (gameType == AI)
    {
        mapSV.clear();
        for (int i = 0; i < GRADSIZE; i++)
        {
            std::vector<int> lineScores;
            for (int j = 0; j < GRADSIZE; j++)
                lineScores.push_back(0);
            mapSV.push_back(lineScores);
        }
    }

    // 轮到黑方下棋为 true, 白方为false
    player = true;


}
void GameModel::InitChess(){
    mapGV.clear();
    for (int i = 0; i < GRADSIZE; i++)
    {
        std::vector<int> lineBoard;
        for (int j = 0; j < GRADSIZE; j++)
            lineBoard.push_back(0);
        mapGV.push_back(lineBoard);
    }
}

void GameModel::actionMan(int row, int col)
{
    updateMap(row, col);
}

void GameModel::updateMap(int row, int col)
{
    if (player)
        mapGV[row][col] = 1;
    else
        mapGV[row][col] = -1;

    // 换手
    player = !player;
}

bool GameModel::winorlose(int row, int col)
{
    // 横竖斜四种大情况，每种情况都根据当前落子往后遍历5个棋子，有一种符合就算赢
    // 水平方向
    for (int i = 0; i < 6; i++)
    {
        // 往左6个，往右匹配5个子，20种情况
        if (col - i > 0 &&
            col - i + 5 < GRADSIZE &&
            mapGV[row][col - i] == mapGV[row][col - i + 1] &&mapGV[row][col - i] == mapGV[row][col - i + 2] &&mapGV[row][col - i] == mapGV[row][col - i + 3] &&mapGV[row][col - i] == mapGV[row][col - i + 4] &&mapGV[row][col - i] == mapGV[row][col - i + 5])
            return true;
    }

    // 竖直方向(上下延伸5个)
    for (int i = 0; i < 6; i++)
    {
        if (row - i > 0 &&
            row - i + 5 < GRADSIZE &&
            mapGV[row - i][col] == mapGV[row - i + 1][col] &&mapGV[row - i][col] == mapGV[row - i + 2][col] &&mapGV[row - i][col] == mapGV[row - i + 3][col] &&mapGV[row - i][col] == mapGV[row - i + 4][col] &&mapGV[row - i][col] == mapGV[row - i + 5][col])
            return true;
    }

    // 左斜方向
    for (int i = 0; i < 6; i++)
    {
        if (row + i < GRADSIZE &&
            row + i - 5 > 0 &&
            col - i > 0 &&
            col - i +5 < GRADSIZE &&
            mapGV[row + i][col - i] == mapGV[row + i - 1][col - i + 1] &&mapGV[row + i][col - i] == mapGV[row + i - 2][col - i + 2] &&mapGV[row + i][col - i] == mapGV[row + i - 3][col - i + 3] &&mapGV[row + i][col - i] == mapGV[row + i - 4][col - i + 4] &&mapGV[row + i][col - i] == mapGV[row + i - 5][col - i + 5])
            return true;
    }

    // 右斜方向
    for (int i = 0; i < 6; i++)
    {
        if (row - i > 0 &&
            row - i + 5 < GRADSIZE &&
            col - i > 0 &&
            col - i + 5 < GRADSIZE &&
            mapGV[row - i][col - i] == mapGV[row - i + 1][col - i + 1] &&mapGV[row - i][col - i] == mapGV[row - i + 2][col - i + 2] &&mapGV[row - i][col - i] == mapGV[row - i + 3][col - i + 3] &&mapGV[row - i][col - i] == mapGV[row - i + 4][col - i + 4] &&mapGV[row - i][col - i] == mapGV[row - i + 5][col - i + 5])
            return true;
    }

    return false;
}
void GameModel::actionAI(int &row, int &col)
{
    // 计算评分
    cScore();

    // 从评分中找出最大分数的位置
    int maxScore = 0;
    std::vector<std::pair<int, int>> maxPoints;

    for (int row = 1; row < GRADSIZE; row++)
        for (int col = 1; col < GRADSIZE; col++)
        {
            // 前提是这个坐标是空的
            if (mapGV[row][col] == 0)
            {
                if (mapSV[row][col] > maxScore)          // 找最大的数和坐标
                {
                    maxPoints.clear();
                    maxScore = mapSV[row][col];
                    maxPoints.push_back(std::make_pair(row, col));
                }
                else if (mapSV[row][col] == maxScore)     // 如果有多个最大的数，都存起来
                    maxPoints.push_back(std::make_pair(row, col));
            }
        }

    // 随机落子，如果有多个点的话
    srand((unsigned)time(0));
    int index = rand() % maxPoints.size();

    std::pair<int, int> pointPair = maxPoints.at(index);
    row = pointPair.first; // 记录落子点
    col = pointPair.second;
    updateMap(row, col);
}

// 最关键的计算评分函数
void GameModel::cScore()
{
    // 统计玩家或者电脑连成的子
    int personNum = 0; // 玩家连成子的个数
    int botNum = 0; // AI连成子的个数
    int emptyNum = 0; // 各方向空白位的个数

    // 清空评分数组
    mapSV.clear();
    for (int i = 0; i < GRADSIZE; i++)
    {
        std::vector<int> lineScores;
        for (int j = 0; j < GRADSIZE; j++)
            lineScores.push_back(0);
        mapSV.push_back(lineScores);
    }

    // 计分（此处是完全遍历，其实可以用bfs或者dfs加减枝降低复杂度，通过调整权重值，调整AI智能程度以及攻守风格）
    for (int row = 0; row < GRADSIZE; row++)
        for (int col = 0; col < GRADSIZE; col++)
        {
            // 空白点就算
            if (row > 0 && col > 0 &&
                mapGV[row][col] == 0)
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
                                if (row + i * y > 0 && row + i * y < GRADSIZE &&
                                    col + i * x > 0 && col + i * x < GRADSIZE &&
                                    mapGV[row + i * y][col + i * x] == 1) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < GRADSIZE &&
                                         col + i * x > 0 && col + i * x < GRADSIZE &&
                                         mapGV[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 5; i++)
                            {
                                if (row - i * y > 0 && row - i * y < GRADSIZE &&
                                    col - i * x > 0 && col - i * x < GRADSIZE &&
                                    mapGV[row - i * y][col - i * x] == 1) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < GRADSIZE &&
                                         col - i * x > 0 && col - i * x < GRADSIZE &&
                                         mapGV[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (personNum == 1)                      // 杀二
                                mapSV[row][col] += 10;
                            else if (personNum == 2)                 // 杀三
                            {
                                if (emptyNum == 1)
                                    mapSV[row][col] += 30;
                                else if (emptyNum == 2)
                                    mapSV[row][col] += 40;
//                                else if (emptyNum == 3)
//                                    mapSV[row][col] += 50;
                            }
                            else if (personNum == 3)                 // 杀四
                            {
                                // 量变空位不一样，优先级不一样
                                if (emptyNum == 1)
                                    mapSV[row][col] += 60;
                                else if (emptyNum == 2)
                                    mapSV[row][col] += 110;
//                                else if (emptyNum == 3)
//                                    mapSV[row][col] += 130;
                            }
                            else if (personNum == 4)  {               // 杀五
                                if (emptyNum == 1)
                                    mapSV[row][col] += 80;
                                else if (emptyNum == 2)
                                    mapSV[row][col] += 150;
//                                else if (emptyNum == 3)
//                                    mapSV[row][col] += 190;
                            }
                            else if (personNum == 5)                 // 杀六
                                mapSV[row][col] += 10100;
                            // 进行一次清空
                            emptyNum = 0;

                            // 对AI黑子评分
                            for (int i = 1; i <= 5; i++)
                            {
                                if (row + i * y > 0 && row + i * y < GRADSIZE &&
                                    col + i * x > 0 && col + i * x < GRADSIZE &&
                                    mapGV[row + i * y][col + i * x] == 1) // 玩家的子
                                {
                                    botNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < GRADSIZE &&
                                         col + i * x > 0 && col + i * x < GRADSIZE &&
                                         mapGV[row +i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 5; i++)
                            {
                                if (row - i * y > 0 && row - i * y < GRADSIZE &&
                                    col - i * x > 0 && col - i * x < GRADSIZE &&
                                    mapGV[row - i * y][col - i * x] == -1) // AI的子
                                {
                                    botNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < GRADSIZE &&
                                         col - i * x > 0 && col - i * x < GRADSIZE &&
                                         mapGV[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (botNum == 0)                      // 普通下子
                                mapSV[row][col] += 5;
                            else if (botNum == 1)                 // 活二
                                mapSV[row][col] += 10;
                            else if (botNum == 2)
                            {
                                if (emptyNum == 1)                // 死三
                                    mapSV[row][col] += 25;
                                else if (emptyNum == 2)
                                    mapSV[row][col] += 50;  // 活三
                            }
                            else if (botNum == 3)
                            {
                                if (emptyNum == 1)                // 死四
                                    mapSV[row][col] += 55;
                                else if (emptyNum == 2)
                                    mapSV[row][col] += 100; // 活四
                            }
                            else if (botNum == 4)
                            {
                                if (emptyNum == 1)                // 死四
                                    mapSV[row][col] += 105;
                                else if (emptyNum == 2)
                                    mapSV[row][col] += 210; // 活四
                            }
                            else if (botNum >= 5)
                                mapSV[row][col] += 10000;   // 活五

                        }
                    }

            }
        }
}
bool GameModel::isDeadGame()//判断和棋

{
    // 所有空格全部填满
    for (int i = 1; i < GRADSIZE; i++)
        for (int j = 1; j < GRADSIZE; j++)
        {
            if (!(mapGV[i][j] == 1 || mapGV[i][j] == -1))
                return false;
        }
    return true;
}
