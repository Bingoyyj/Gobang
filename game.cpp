#include "game.h"
#include "ui_game.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDateTime>
#include <QMessageBox>

Game::Game(int _mode,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Game)
{
    ui->setupUi(this);
    this->setFixedSize(800,600);
    mode=_mode;
    if(mode==1)
        this->setWindowTitle("玩家大战");
    else
        this->setWindowTitle("人机大战");

    QString ModeButton=mode==1?"悔棋":"难度";
    ui->Regret->setText(ModeButton);
    QFont f;
    f.setPointSize(20);
    ui->mode->setFont(f);
    QString currentMode=mode==1?"玩家模式":"人机模式";
    ui->mode->setText(currentMode);

    setMouseTracking(true);//不用点击鼠标也一直追踪
    init();
}

Game::~Game()
{
    delete ui;
}

void Game::init(){
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            p[i][j].point.setX(20+40*i);
            p[i][j].point.setY(20+40*j);
            p[i][j].status=0;
            p[i][j].isInVector=false;
        }
    }

    v.clear();//清空元素，但不回收空间
    s.clear();

    cursorRow=-1;//光标位置
    cursorCol=-1;
    canPress=false;

    getBestRow=-1;
    getBestCol=-1;
    flag=0;
}

void Game::paintEvent(QPaintEvent *){

    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    for(int i=0;i<15;i++){
        painter.drawLine(p[0][i].point,p[14][i].point);
        painter.drawLine(p[i][0].point,p[i][14].point);
    }

    //画天元
    painter.setBrush(Qt::black);
    painter.drawEllipse(p[7][7].point,5,5);
    painter.drawEllipse(p[3][3].point,5,5);
    painter.drawEllipse(p[11][3].point,5,5);
    painter.drawEllipse(p[3][11].point,5,5);
    painter.drawEllipse(p[11][11].point,5,5);
    painter.setBrush(Qt::NoBrush);

    //画光标
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(0,0,0,100)));
    if(cursorRow!=-1&&cursorCol!=-1)
        painter.drawEllipse(p[cursorRow][cursorCol].point,15,15);

    //画棋子
    painter.setPen(Qt::black);
    for(int i=0;i<15;i++){
        for(int j=0;j<15;j++){
            if(p[i][j].status==1){
                painter.setBrush(QBrush(Qt::black));
                painter.drawEllipse(p[i][j].point,15,15);
            }
            else if(p[i][j].status==2){
                painter.setBrush(QBrush(Qt::white));
                painter.drawEllipse(p[i][j].point,15,15);
            }
        }
    }

    //十字标记最后棋子
    if(s.size()!=0){
        QPoint pLast=p[s.top().rx()][s.top().ry()].point;
        painter.setPen(Qt::red);
        painter.drawLine(pLast.rx(),pLast.ry()-5,pLast.rx(),pLast.ry()+5);
        painter.drawLine(pLast.rx()-5,pLast.ry(),pLast.rx()+5,pLast.ry());
    }
}

void Game::mouseMoveEvent(QMouseEvent *event){
    if(event->x()>=5&&event->x()<=595&&event->y()>=5&&event->y()<=595){
        cursorRow=event->x()/40;
        cursorCol=event->y()/40;
        if(p[cursorRow][cursorCol].status){
            setCursor(Qt::ForbiddenCursor);
            canPress=false;
        }
        else{
            setCursor(Qt::ArrowCursor);
            canPress=true;
        }
    }
    else{
        cursorRow=-1;
        cursorCol=-1;
        setCursor(Qt::ArrowCursor);
        canPress=false;
    }
    update();
}

void Game::mousePressEvent(QMouseEvent *){
    if(canPress && !flag){
        if(mode==1){
            if(s.size()%2==0){
                p[cursorRow][cursorCol].status=1;
                if(getScore(cursorRow,cursorCol,2)==-500000)flag=1;
            }//设置黑棋状态
            else{
                p[cursorRow][cursorCol].status=2;
                if(getScore(cursorRow,cursorCol,1)==500000)flag=2;
            }//设置白棋状态
            s.push(QPoint(cursorRow,cursorCol));
            repaint();
            markPiece(cursorRow,cursorCol);
        }
        else{
            if(s.size()%2==0){
                playerPlay();
                if(flag==0)
                    aiPlay();
            }
        }
        canPress=false;//防双击
        check();
    }
}

void Game::aiPlay(){
    getBestRow=-1;
    getBestCol=-1;
    if(getMaxScore(level,8000000,0)==500000)flag=2;
    p[getBestRow][getBestCol].status=2;
    s.push(QPoint(getBestRow,getBestCol));
    repaint();
    markPiece(getBestRow,getBestCol);
}

void Game::playerPlay(){
    p[cursorRow][cursorCol].status=1;//设置黑棋状态
    s.push(QPoint(cursorRow,cursorCol));
    if(getScore(cursorRow,cursorCol,2)==-500000)flag=1;
    repaint();
    markPiece(cursorRow,cursorCol);
}

//八个方向记录3个棋位
void Game::markPiece(int row,int col){
    //先除掉原有棋位
    for(int i=0;i<v.size();i++){
        if(v[i]==QPoint(row,col))
            v.erase(v.begin()+i);
    }
    //第一棋位
    if(row+1<=14 && p[row+1][col].status==0 && !p[row+1][col].isInVector){v.push_front(QPoint(row+1,col));p[row+1][col].isInVector=true;}
    if(row-1>=0 && p[row-1][col].status==0 && !p[row-1][col].isInVector){v.push_front(QPoint(row-1,col));p[row-1][col].isInVector=true;}

    if(col+1<=14 && p[row][col+1].status==0 && !p[row][col+1].isInVector){v.push_front(QPoint(row,col+1));p[row][col+1].isInVector=true;}
    if(col-1>=0 && p[row][col-1].status==0 && !p[row][col-1].isInVector){v.push_front(QPoint(row,col-1));p[row][col-1].isInVector=true;}

    if(row-1>=0 && col-1>=0 && p[row-1][col-1].status==0 && !p[row-1][col-1].isInVector){v.push_front(QPoint(row-1,col-1));p[row-1][col-1].isInVector=true;}
    if(row+1<=14 && col+1<=14 && p[row+1][col+1].status==0 && !p[row+1][col+1].isInVector){v.push_front(QPoint(row+1,col+1));p[row+1][col+1].isInVector=true;}

    if(row-1>=0 && col+1<=14 && p[row-1][col+1].status==0 && !p[row-1][col+1].isInVector){v.push_front(QPoint(row-1,col+1));p[row-1][col+1].isInVector=true;}
    if(row+1<=14 && col-1>=0 && p[row+1][col-1].status==0 && !p[row+1][col-1].isInVector){v.push_front(QPoint(row+1,col-1));p[row+1][col-1].isInVector=true;}

    //第二棋位
    if(row+2<=14 && p[row+2][col].status==0 && !p[row+2][col].isInVector){v.push_front(QPoint(row+2,col));p[row+2][col].isInVector=true;}
    if(row-2>=0 && p[row-2][col].status==0 && !p[row-2][col].isInVector){v.push_front(QPoint(row-2,col));p[row-2][col].isInVector=true;}

    if(col+2<=14 && p[row][col+2].status==0 && !p[row][col+2].isInVector){v.push_front(QPoint(row,col+2));p[row][col+2].isInVector=true;}
    if(col-2>=0 && p[row][col-2].status==0 && !p[row][col-2].isInVector){v.push_front(QPoint(row,col-2));p[row][col-2].isInVector=true;}

    if(row-2>=0 && col-2>=0 && p[row-2][col-2].status==0 && !p[row-2][col-2].isInVector){v.push_back(QPoint(row-2,col-2));p[row-2][col-2].isInVector=true;}
    if(row+2<=14 && col+2<=14 && p[row+2][col+2].status==0 && !p[row+2][col+2].isInVector){v.push_back(QPoint(row+2,col+2));p[row+2][col+2].isInVector=true;}

    if(row-2>=0 && col+2<=14 && p[row-2][col+2].status==0 && !p[row-2][col+2].isInVector){v.push_back(QPoint(row-2,col+2));p[row-2][col+2].isInVector=true;}
    if(row+2<=14 && col-2>=0 && p[row+2][col-2].status==0 && !p[row+2][col-2].isInVector){v.push_back(QPoint(row+2,col-2));p[row+2][col-2].isInVector=true;}

    //第三个棋位
    if(row+3<=14 && p[row+3][col].status==0 && !p[row+3][col].isInVector){v.push_back(QPoint(row+3,col));p[row+3][col].isInVector=true;}
    if(row-3>=0 && p[row-3][col].status==0 && !p[row-3][col].isInVector){v.push_back(QPoint(row-3,col));p[row-3][col].isInVector=true;}

    if(col-3>=0 && p[row][col-3].status==0 && !p[row][col-3].isInVector){v.push_back(QPoint(row,col-3));p[row][col-3].isInVector=true;}
    if(col+3<=14 &&p[row][col+3].status==0 && !p[row][col+3].isInVector){v.push_back(QPoint(row,col+3));p[row][col+3].isInVector=true;}

    if(row-3>=0 && col-3>=0 && p[row-3][col-3].status==0 && !p[row-3][col-3].isInVector){v.push_back(QPoint(row-3,col-3));p[row-3][col-3].isInVector=true;}
    if(row+3<=14 && col+3<=14 && p[row+3][col+3].status==0 && !p[row+3][col+3].isInVector){v.push_back(QPoint(row+3,col+3));p[row+3][col+3].isInVector=true;}

    if(row-3>=0 && col+3<=14 && p[row-3][col+3].status==0 && !p[row-3][col+3].isInVector){v.push_back(QPoint(row-3,col+3));p[row-3][col+3].isInVector=true;}
    if(row+3<=14 && col-3>=0 && p[row+3][col-3].status==0 && !p[row+3][col-3].isInVector){v.push_back(QPoint(row+3,col-3));p[row+3][col-3].isInVector=true;}
}

//计算权重
int Game::add(int isBlock,int count){
    int score=0;
    if(count>=5)return 500000;
    if(isBlock==2)return 0;
    else if(isBlock==1){
        switch (count) {
        case 4:score=1000;break;
        case 3:score=100;break;
        case 2:score=10;break;
        default:break;
        }
    }
    else{
        switch (count) {
        case 4:score=10000;break;
        case 3:score=1000;break;
        case 2:score=100;break;
        case 1:score=10;break;
        default:break;
        }
    }
    return score;
}

//计算某点分值
int Game::getScore(int &i,int &j,int status){
    int score=0;
    //上下
    int isBlock1=0,isBlock2=0,count=0,count1=0,count2=0;

    for(int k=1;k<=4;k++){
        if(j-k<0 || p[i][j-k].status==status){
            isBlock1++;
            break;
        }
        else if(p[i][j-k].status==0){break;}
        else{ count1++;}
    }
    for(int k=1;k<=4;k++){
        if(j+k>14 || p[i][j+k].status==status){
            isBlock2++;
            break;
        }
        else if(p[i][j+k].status==0)break;
        else count2++;
    }
    count=count1+count2+1;
    if(count>=5)return 500000*(status==1?1:-1);
    score+=add(isBlock1+isBlock2,count1+count2+1);
    score-=add(isBlock1,count1);
    score-=add(isBlock2,count2);

    //左右
    isBlock1=0;isBlock2=0;count1=0;count2=0;

    for(int k=1;k<=4;k++){
        if(i-k<0 || p[i-k][j].status==status){
            isBlock1++;
            break;
        }
        else if(p[i-k][j].status==0)break;
        else count1++;
    }
    for(int k=1;k<=4;k++){
        if(i+k>14 || p[i+k][j].status==status){
            isBlock2++;
            break;
        }
        else if(p[i+k][j].status==0)break;
        else count2++;
    }
    count=count1+count2+1;
    if(count>=5)return 500000*(status==1?1:-1);
    score+=add(isBlock1+isBlock2,count1+count2+1);
    score-=add(isBlock1,count1);
    score-=add(isBlock2,count2);


    //左上右下
    isBlock1=0;isBlock2=0;count1=0;count2=0;

    for(int k=1;k<=4;k++){
        if(i-k<0 || j-k<0 || p[i-k][j-k].status==status){
            isBlock1++;
            break;
        }
        else if(p[i-k][j-k].status==0)break;
        else count1++;
    }
    for(int k=1;k<=4;k++){
        if(i+k>14 || j+k>14 || p[i+k][j+k].status==status){
            isBlock2++;
            break;
        }
        else if(p[i+k][j+k].status==0)break;
        else count2++;
    }
    count=count1+count2+1;
    if(count>=5)return 500000*(status==1?1:-1);
    score+=add(isBlock1+isBlock2,count1+count2+1);
    score-=add(isBlock1,count1);
    score-=add(isBlock2,count2);


    //左下右上
    isBlock1=0;isBlock2=0;count1=0;count2=0;

    for(int k=1;k<=4;k++){
        if(i+k>14 || j-k<0 || p[i+k][j-k].status==status){
            isBlock1++;
            break;
        }
        else if(p[i+k][j-k].status==0)break;
        else count1++;
    }
    for(int k=1;k<=4;k++){
        if(i-k<0 || j+k>14 || p[i-k][j+k].status==status){
            isBlock2++;
            break;
        }
        else if(p[i-k][j+k].status==0)break;
        else count2++;
    }
    count=count1+count2+1;
    if(count>=5)return 500000*(status==1?1:-1);
    score+=add(isBlock1+isBlock2,count1+count2+1);
    score-=add(isBlock1,count1);
    score-=add(isBlock2,count2);
    if(status==1)return score;//如果对方是黑棋，即自己是白棋时，返回正
    else return -1*score;//否则返回负
}

//ai
int Game::getMaxScore(int deep,int tempBestScore,int topScore){
    if(s.size()==1){
        int addx[]={1,0,-1,0,1,-1,1,-1};
        int addy[]={0,1,0,-1,1,-1,-1,1};
        QDateTime time = QDateTime::currentDateTime();
        int random=time.toTime_t()%8;
        getBestRow=s.top().rx()+addx[random];
        getBestCol=s.top().ry()+addy[random];//增强随机性
        return 0;
    }
    int maxScore=-1000000,maxRow=-1,maxCol=-1,tempScore=0;
    for(int k=0;k<v.size();k++){
        int i=v[k].rx();
        int j=v[k].ry();
        if(p[i][j].status!=0)continue;
        tempScore=getScore(i,j,1)+topScore;

        if(deep==level && tempScore==500000){//如果5连珠，直接返回
            getBestRow=i;
            getBestCol=j;
            return tempScore;
        }

        if(deep!=1){
            p[i][j].status=2;
            tempScore+=getMinScore(deep-1,maxScore,tempScore);
            p[i][j].status=0;
        }
        if(tempScore>=tempBestScore){return 2000000;}
        if(tempScore>maxScore){
            maxScore=tempScore;
            maxRow=i;
            maxCol=j;
        }
    }
    if(deep==level){
        getBestRow=maxRow;
        getBestCol=maxCol;
    }
    return maxScore;
}

int Game::getMinScore(int deep,int tempBestScore,int topScore){
    int minScore=1000000,tempScore=0;
    for(int k=0;k<v.size();k++){
        int i=v[k].rx();
        int j=v[k].ry();
        if(p[i][j].status!=0)continue;
        tempScore=getScore(i,j,2)+topScore;
        if(deep!=1){
            p[i][j].status=1;
            tempScore+=getMaxScore(deep-1,minScore,tempScore);
            p[i][j].status=0;
        }
        if(tempScore<=tempBestScore)return -2000000;
        if(tempScore<minScore){
            minScore=tempScore;
        }
    }
    return minScore;
}

void Game::check(){
    if(flag==2){
        countWhiteWin++;
        ui->lcdNumber1->display(countWhiteWin);
        if(QMessageBox::Ok==QMessageBox::question(this,"ques","白棋胜利,是否再来一局？",QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)){
            init();
        }
        else{
            ui->restart->setText("再来一局");
        }
    }else if(flag==1){
        countBlackWin++;
        ui->lcdNumber2->display(countBlackWin);
        if(QMessageBox::Ok==QMessageBox::question(this,"ques","黑棋胜利,是否再来一局？",QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)){
            init();
        }
        else{
            ui->restart->setText("再来一局");
        }
    }else if(s.size()==225){
        if(QMessageBox::Ok==QMessageBox::question(this,"ques","平局,是否再来一局？",QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)){
            init();
        }
        else{
            ui->restart->setText("再来一局");
        }
    }
}

void Game::on_restart_clicked()
{
    init();
    ui->restart->setText("重新开始");
}

void Game::on_Regret_clicked()
{
    if(mode==1){
        if(s.size()!=0){
            flag=0;
            QPoint pLast=s.pop();
            p[pLast.rx()][pLast.ry()].status=0;
            update();
        }
        else{
            QMessageBox::information(this,"info","您还没走棋哦！");
        }
    }
    else{
        QString currentLevel=level==2?"简单":"困难";
        QString changeLevel=level==2?"困难":"简单";
        if(QMessageBox::Ok==QMessageBox::question(this,"ques",QString("当前为%1，是否调为%2？").arg(currentLevel).arg(changeLevel),QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)){
            level=level==2?4:2;
        }
    }
}

void Game::on_change_clicked()
{
    if(QMessageBox::Ok==QMessageBox::question(this,"ques","切换模式游戏将重新开始，是否继续？",QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)){
        mode=mode==1?2:1;
        QString ModeButton=mode==1?"悔棋":"难度";
        ui->Regret->setText(ModeButton);
        QString currentMode=mode==1?"玩家模式":"人机模式";
        ui->mode->setText(currentMode);
        if(mode==1)
            this->setWindowTitle("玩家大战");
        else
            this->setWindowTitle("人机大战");
        init();
    }
}

void Game::on_recount_clicked()
{
    countWhiteWin=0;
    countBlackWin=0;
    ui->lcdNumber1->display(countWhiteWin);
    ui->lcdNumber2->display(countBlackWin);
}
