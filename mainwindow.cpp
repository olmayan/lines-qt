#include "field.h"
#include "mainwindow.h"
#include <QGraphicsSvgItem>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>

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
    m_gameType = Lines;
    field->initGame(m_gameType);

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
    field->initGame(m_gameType);
}

void MainWindow::newGameLines()
{
    m_gameType = Lines;
    field->initGame(m_gameType);
}

void MainWindow::newGameSquares()
{
    m_gameType = Squares;
    field->initGame(m_gameType);
}

void MainWindow::newGameBlocks()
{
    m_gameType = Blocks;
    field->initGame(m_gameType);
}

void MainWindow::updateScore(int score)
{
    setWindowTitle(QString("lines-qt (Score: %1)").arg(score));
}

void MainWindow::gameOver()
{
    QMessageBox::information(this, tr("Game Over!"), tr("Your score is %1!").arg(field->score()), QMessageBox::Ok);
    field->initGame(Lines);
}

void MainWindow::buildMenu()
{
    QMenuBar *mb = menuBar();

    QMenu *gameMenu = new QMenu(tr("&Game"));
    mb->addMenu(gameMenu);

    QAction *aNewGame = new QAction(tr("&New Game"), this);
    aNewGame->setShortcut(QKeySequence("F2"));
    connect(aNewGame, SIGNAL(triggered(bool)), this, SLOT(newGame()));
    gameMenu->addAction(aNewGame);

    gameMenu->addSeparator();

    QAction *aNewGameLines = new QAction(tr("New &Lines Game"), this);
    aNewGameLines->setShortcut(QKeySequence("F3"));
    connect(aNewGameLines, SIGNAL(triggered(bool)), this, SLOT(newGameLines()));
    gameMenu->addAction(aNewGameLines);

    QAction *aNewGameSquares = new QAction(tr("New &Squares Game"), this);
    aNewGameSquares->setShortcut(QKeySequence("F4"));
    connect(aNewGameSquares, SIGNAL(triggered(bool)), this, SLOT(newGameSquares()));
    gameMenu->addAction(aNewGameSquares);

    QAction *aNewGameBlocks = new QAction(tr("New &Blocks Game"), this);
    aNewGameBlocks->setShortcut(QKeySequence("F5"));
    connect(aNewGameBlocks, SIGNAL(triggered(bool)), this, SLOT(newGameBlocks()));
    gameMenu->addAction(aNewGameBlocks);

    gameMenu->addSeparator();

    QAction *aQuit = new QAction(tr("&Quit"), this);
    connect(aQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
    gameMenu->addAction(aQuit);

    QMenu *helpMenu = new QMenu(tr("&Help"));
    mb->addMenu(helpMenu);

    QAction *aAboutQt = new QAction(tr("About &Qt"), this);
    connect(aAboutQt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt()));
    helpMenu->addAction(aAboutQt);

}
