#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QVector>
#include <QStack>

namespace Ui {
class Game;
}

struct Piece{
    QPoint point;//像素坐标
    int status;//0无子、1黑子、2白子
    bool isInVector;
};

class Game : public QWidget
{
    Q_OBJECT

public:
    explicit Game(int _mode,QWidget *parent = nullptr);
    ~Game();

    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    int add(int isBlock,int count);
    int getScore(int &i,int &j,int status);
    void markPiece(int row,int col);

    int getMaxScore(int deep,int tempBestScore,int topScore);
    int getMinScore(int deep,int tempBestScore,int topScore);
    void check();
    void aiPlay();
    void playerPlay();

    void init();

private slots:
    void on_Regret_clicked();

    void on_restart_clicked();

    void on_change_clicked();

    void on_recount_clicked();

private:
    Ui::Game *ui;

    int mode;//1为玩家模式、2为人机模式
    int countWhiteWin=0;
    int countBlackWin=0;
    int cursorRow=-1;//光标位置
    int cursorCol=-1;
    bool canPress=false;
    Piece p[15][15];
    QVector<QPoint> v;//记录受影响的空棋位
    QStack<QPoint> s;//记录下过棋的顺序

    int level=4;
    int getBestRow=-1;
    int getBestCol=-1;
    int flag=0;//记录是否胜利，1为黑棋胜，2为白棋胜
};

#endif // GAME_H
