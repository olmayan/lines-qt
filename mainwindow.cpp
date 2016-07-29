#include "field.h"
#include "mainwindow.h"
#include <QGraphicsSvgItem>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    scene = new QGraphicsScene(0, 0, CELL_SIZE * W, CELL_SIZE * H);
    view = new GraphicsView(scene);
    setCentralWidget(view);
    setMinimumSize(sizeHint());

    buildMenu();

    field = Field::initInstance(scene);
    connect(field, SIGNAL(scoreChanged(int)), this, SLOT(updateScore(int)));
    connect(field, SIGNAL(gameOver()), this, SLOT(gameOver()));
    field->initGame(Lines);

    connect(view, SIGNAL(mousePressed(QMouseEvent *)), this, SLOT(mousePressed(QMouseEvent *)));
}

void MainWindow::test()
{

}

void MainWindow::mousePressed(QMouseEvent *event)
{
    QPoint pt(event->x() / CELL_SIZE, event->y() / CELL_SIZE);
    field->activate(pt);
}

void MainWindow::newGame()
{
    field->initGame(Lines);
}

void MainWindow::updateScore(int score)
{
    setWindowTitle(QString("lines-qt (Score: %1").arg(score));
}

void MainWindow::gameOver()
{
    QMessageBox::information(this, tr("Game Over!"), tr("Your score is %1!").arg(field->score()), QMessageBox::Ok);
    field->initGame(Lines);
}

void MainWindow::buildMenu()
{
    QMenuBar *mb = menuBar();

    QMenu *gameMenu = new QMenu;
    gameMenu->setTitle(tr("&Game"));
    mb->addMenu(gameMenu);

    QAction *aNewGame = new QAction(tr("&New Game"), this);
    aNewGame->setShortcut(QKeySequence("F2"));
    connect(aNewGame, SIGNAL(triggered(bool)), this, SLOT(newGame()));
    gameMenu->addAction(aNewGame);
}
