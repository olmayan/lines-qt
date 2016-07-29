#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "field.h"
#include "graphicsview.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QMouseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:
    void mouseMoveEvent();

public slots:
    void test();
    void mousePressed(QMouseEvent *event);
    void newGame();
    void newGameLines();
    void newGameSquares();
    void newGameBlocks();
    void updateScore(int);
    void gameOver();

private:
    void buildMenu();

    QGraphicsScene *scene;
    QGraphicsView *view;
    Field *field;
    GameType m_gameType;
};

#endif // MAINWINDOW_H
