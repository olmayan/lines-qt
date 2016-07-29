#include "field.h"
#include "resourceloader.h"
#include "svgrendereritem.h"

#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>

#ifdef QT_DEBUG
#include <QDateTime>
#endif

#define MOVE_DURATION 50
#define APPEAR_DURATION 100
#define GROW_DURATION 150
#define DISAPPEAR_DURATION 250

Field::Field(QGraphicsScene *scene) //: QObject()
{
    m_scene = scene;
    m_animating = false;

    m_pathItems.fill(NULL, W * H);
    ResourceLoader *loader = ResourceLoader::instance();
    for (int i = 0; i < W; i++)
    {
        for (int j = 0; j < H; j++)
        {
            QGraphicsObject *item = new SvgRendererItem(loader->tile());
            item->setPos(i * CELL_SIZE, j * CELL_SIZE);
#ifdef QT_DEBUG
            //if (j % 2 && i % 2)
            //    item->setScale(0.5);
#endif
            item->setZValue(0);
            m_scene->addItem(item);

            item = new SvgRendererItem(loader->pathmarker());
            item->setPos(i * CELL_SIZE, j * CELL_SIZE);
            item->setZValue(2);
            item->setVisible(false);
            m_scene->addItem(item);
            setVectorAt(m_pathItems, i, j, item);
        }
    }

    m_sel = QPoint(-1, -1);
    m_selItem = new SvgRendererItem(loader->select());
    m_selItem->setZValue(1);
    m_selItem->setVisible(false);
    m_scene->addItem(m_selItem);

    m_field.fill(0, W * H);
    m_fieldItems.fill(NULL, W * H);
    m_next.fill({ QPoint(-1, -1), 0, NULL, NULL }, 3);
}

Field *Field::m_instance = NULL;

Field *Field::initInstance(QGraphicsScene *scene)
{
    m_instance = new Field(scene);
    return m_instance;
}

Field *Field::instance()
{
    return m_instance;
}

Field::~Field()
{

}

bool Field::animating()
{
    return m_animating;
}

void Field::setAnimating(bool value)
{
    m_animating = value;
}

void Field::throwBalls(int num, bool directly)
{
    QVector<QPoint> free;
    free.fill(QPoint(0, 0), W * H);
    int nFree = 0;
    for (int i = 0; i < W; i++)
        for (int j = 0; j < H; j++)
            if (at(i, j) == 0)
            {
                free[nFree] = QPoint(i, j);
                nFree++;
            }

#ifdef QT_DEBUG
    for (int i = 0; i < W * H; i++)
    {
        //qDebug("%d, %d", free[i].x(), free[i].y());
    }
#endif
    //return;

    QList<SuggestedBall> ballsToThrow;
    for (int n = 0; n < num; n++)
    {
        if (nFree > 0)
        {
            int at = qrand() % nFree;
            ballsToThrow.append({ free[at], qrand() % COLORS + 1, NULL, NULL });
            free.removeAt(at);
            nFree--;
        }
        else
            ballsToThrow.append({ QPoint(-1, -1), qrand() % COLORS + 1, NULL, NULL });
    }

    if (directly)
    {
        // This is needed on startup only, therefore do nothing except placing balls
        foreach (auto ball, ballsToThrow)
        {
            setAt(ball.pt, ball.color);
            QGraphicsObject *item = new SvgRendererItem(ResourceLoader::instance()->ball(ball.color));
            item->setPos(ball.pt * CELL_SIZE);
            item->setZValue(3);
            m_scene->addItem(item);
            setVectorAt(m_fieldItems, ball.pt, item);
        }
    }
    else
    {
        for (int i = 0; i < num; i++)
        {
            auto ball = m_next[i];

            if (ball.toolbarItem != NULL)
            {
                m_scene->removeItem(ball.toolbarItem);
                ball.toolbarItem->deleteLater();
            }

            if (ball.fieldItem != NULL)
            {
                m_scene->removeItem(ball.fieldItem);
                ball.fieldItem->deleteLater();
            }
        }

        if (m_animating)
        {
            //lock();
            QAnimationGroup *group = addAnimGroup(new QParallelAnimationGroup);

            for (int i = 0; i < num; i++)
            {
                auto ball = ballsToThrow[i];
                // TODO : add toolbar ball display
                if (ball.pt.x() > -1)
                {
                    QGraphicsObject *item = new SvgRendererItem(ResourceLoader::instance()->ball(ball.color));
                    item->setScale(0.0);
                    item->setPos((QPointF(0.5, 0.5) + ball.pt) * CELL_SIZE);
                    item->setZValue(2);
                    m_scene->addItem(item);
                    ball.fieldItem = item;

                    QPropertyAnimation *anim = new QPropertyAnimation(item, "scale");
                    anim->setEndValue(0.4);
                    anim->setDuration(APPEAR_DURATION);
                    group->addAnimation(anim);

                    anim = new QPropertyAnimation(item, "pos");
                    anim->setEndValue((QPointF(0.3, 0.3) + ball.pt) * CELL_SIZE);
                    anim->setDuration(APPEAR_DURATION);
                    group->addAnimation(anim);
                }
                m_next[i] = ball;
            }
            //if (!m_deleting) // Deletion animation lasts longer,
                             // so suppress unlock if deleting at the same time
                //group->connect(group, SIGNAL(finished()), this, SLOT(unlock()));
            group->start(QAbstractAnimation::DeleteWhenStopped);
        }
        else
        {
            for (int i = 0; i < num; i++)
            {
                auto ball = ballsToThrow[i];
                // TODO : add toolbar ball display
                QGraphicsObject *item = new SvgRendererItem(ResourceLoader::instance()->ball(ball.color));
                item->setScale(0.4);
                item->setPos((QPointF(0.3, 0.3) + ball.pt) * CELL_SIZE);
                item->setZValue(2);
                m_scene->addItem(item);
                ball.fieldItem = item;
                m_next[i] = ball;
            }
        }
    }
}

void Field::initGame(GameType gameType)
{
    setAnimating(false);
    clear();
    throwBalls(3, true);
    throwBalls(3);
    m_gameType = gameType;
    setAnimating();
    setScore(0);
    unlock();
}

void Field::clear()
{
    while (!m_activeAnimGroups.empty())
    {
        QAnimationGroup *group = m_activeAnimGroups.first();
        group->disconnect();
        group->stop();
        m_activeAnimGroups.removeFirst();
    }

    for (int i = 0; i < W; i++)
        for (int j = 0; j < H; j++)
            if (at(i, j))
            {
                QGraphicsObject *item = vectorAt(m_fieldItems, i, j);
                m_scene->removeItem(item);
                item->deleteLater();
            }

    for (int k = 0; k < 3; k++)
    {
        auto ball = m_next[k];
        if (ball.toolbarItem != NULL)
        {
            QGraphicsObject *item = ball.toolbarItem;
            m_scene->removeItem(item);
            item->deleteLater();
        }
        if (ball.fieldItem != NULL)
        {
            QGraphicsObject *item = ball.fieldItem;
            m_scene->removeItem(item);
            item->deleteLater();
        }
    }

    m_field.fill(0, W * H);
    m_fieldItems.fill(NULL, W * H);
    m_next.fill({ QPoint(-1, -1), 0, NULL, NULL });
    unselect();
}

bool Field::onField(int x, int y)
{
    return x > -1 && y > -1 && x < W && y < H;
}

bool Field::onField(QPoint point)
{
    return onField(point.x(), point.y());
}

void Field::activate(int x, int y)
{
    activate(QPoint(x, y));
}

void Field::activate(QPoint point)
{
    if (locked() || !onField(point))
        return;

    if (at(point))
    {
        m_sel = point;
        m_selItem->setPos(point * CELL_SIZE);
        m_selItem->setVisible(true);

#ifdef QT_DEBUG
        //for (int i = 0; i < W; i++)
        //    for (int j = 0; j < H; j++)
        //        vectorAt(m_pathItems, i, j)->setVisible(false);
#endif
    }
    else if (onField(m_sel) && at(m_sel))
    {
        moveSelected(point);
    }
}

void Field::unselect()
{
    m_sel = QPoint(-1, -1);
    m_selItem->setVisible(false);;
}

QPoint Field::selected()
{
    return m_sel;
}

void Field::moveSelected(QPoint destination)
{
    QVector<int> vector = m_field;
    int step = 0;
    setVectorAt(vector, m_sel, -1);

    bool bContinue, bHasPath = false;
    QPoint points[4];
    points[0] = QPoint(1, 0);
    points[1] = QPoint(0, 1);
    points[2] = QPoint(-1, 0);
    points[3] = QPoint(0, -1);

    // Looking whether there is a path to destination
    do
    {
        step--;
        bContinue = false;
        for (int i = 0; i < W; i++)
            for (int j = 0; j < H; j++)
                if (!vectorAt(vector, i, j))
                    for (int k = 0; k < 4; k++)
                    {
                        QPoint point = points[k] + QPoint(i, j);
                        if (onField(point) && vectorAt(vector, point) == step)
                        {
                            if (destination == QPoint(i, j))
                                bHasPath = true;

                            setVectorAt(vector, i, j, step - 1);
                            bContinue = !bHasPath;
                            break;
                        }
                    }
    } while (bContinue);

#ifdef QT_DEBUG
    //qDebug("has path: %d", (int)bHasPath);
#endif

    if (!bHasPath)
    {
        // TODO : play sound
        return;
    }

    // Finding shortest
    QList<QPoint> path;
    for (QPoint current = destination; current != m_sel; )
    {
        path.prepend(current);
        for (int k = 0; k < 4; k++)
        {
            QPoint point = points[k] + current;
            if (onField(point) && vectorAt(vector, point) - vectorAt(vector, current) == 1)
            {
                current = point;
                break;
            }
        }
    }

#ifdef QT_DEBUG
    //for (int i = 0; i < W; i++)
    //    for (int j = 0; j < H; j++)
    //        vectorAt(m_pathItems, i, j)->setVisible(path.contains(QPoint(i, j)));
#endif

    path.prepend(m_sel);
    int num = path.count() - 1;
    for (int i = 1; i < num; i++)
    {
        if ((path[i - 1].x() == path[i].x() &&
             path[i + 1].x() == path[i].x()) ||
            (path[i - 1].y() == path[i].y() &&
             path[i + 1].y() == path[i].y()))
        {
            path.removeAt(i);
            num--;
            i--;
        }
    }

    num++;
    QAnimationGroup *group = addAnimGroup(new QSequentialAnimationGroup);

    for (int i = 1; i < num; i++)
    {
        QPropertyAnimation *anim = new QPropertyAnimation(vectorAt(m_fieldItems, m_sel), "pos");
        anim->setEndValue(path[i] * CELL_SIZE);
        anim->setDuration(MOVE_DURATION * (qAbs(path[i].x() - path[i - 1].x()) + qAbs(path[i].y() - path[i - 1].y())));
        group->addAnimation(anim);
    }

    setAt(destination, at(m_sel));
    setAt(m_sel, 0);
    setVectorAt(m_fieldItems, destination, vectorAt(m_fieldItems, m_sel));
    setVectorAt(m_fieldItems, m_sel, (QGraphicsObject *)NULL);
    unselect();
    if (checkForDeletion())
        group->connect(group, SIGNAL(finished()), this, SLOT(deleteBalls()));
    else
        group->connect(group, SIGNAL(finished()), this, SLOT(growBalls()));
    group->start(QAbstractAnimation::DeleteWhenStopped);

    /*foreach (QPoint point, path)
    {
        QGraphicsObject *item = new SvgRendererItem(ResourceLoader::instance()->pathmarker());
        item->setPos(point * CELL_SIZE);
        item->setZValue(1);
        m_scene->addItem(item);
    }*/
}

bool Field::checkForDeletion()
{
    m_deletionMap.fill(false, W * H);
    bool res = false;
    int cur;
    switch (m_gameType)
    {
    case Lines:
    case Squares:
    case Blocks:
        for (int i = 0; i < W - 1; i++)
            for (int j = 0; j < H - 1; j++)
                if ((cur = at(i, j)) &&
                    cur == at(i + 1, j) &&
                    cur == at(i, j + 1) &&
                    cur == at(i + 1, j + 1))
                {
                    setVectorAt(m_deletionMap, i, j, true);
                    setVectorAt(m_deletionMap, i + 1, j, true);
                    setVectorAt(m_deletionMap, i, j + 1, true);
                    setVectorAt(m_deletionMap, i + 1, j + 1, true);
                    res = true;
                }
        break;
    }
    return res;
}

int Field::at(int x, int y)
{
    return m_field[W * y + x];
}

int Field::at(QPoint point)
{
    return m_field[W * point.y() + point.x()];
}

void Field::setAt(int x, int y, int value)
{
    m_field[W * y + x] = value;
}

void Field::setAt(QPoint point, int value)
{
    m_field[W * point.y() + point.x()] = value;
}

int Field::score() const
{
    return m_score;
}

void Field::setScore(int score)
{
    m_score = score;
    emit scoreChanged(score);
}

QAnimationGroup *Field::addAnimGroup(QAnimationGroup *group)
{
    m_activeAnimGroups.append(group);
    group->connect(group, SIGNAL(finished()), this, SLOT(deleteAnimGroup()));
    lock();
    return group;
}

GameType Field::gameType() const
{
    return m_gameType;
}

bool Field::locked() const
{
    return m_locked;
}

void Field::lock()
{
    m_locked = true;
}

void Field::unlock()
{
#ifdef QT_DEBUG
    qDebug("unlock() called: %s", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data());
#endif
    m_locked = false;
}

void Field::deleteAnimGroup()
{
#ifdef QT_DEBUG
    qDebug("deleteAnimGroup() called: %s", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toLocal8Bit().data());
#endif
    QAnimationGroup *group = qobject_cast<QAnimationGroup *>(QObject::sender());
    if (group == NULL)
        return;

    m_activeAnimGroups.removeOne(group);
    if (m_activeAnimGroups.empty())
        unlock();
}

void Field::throwBalls()
{
    throwBalls(3);
}

void Field::deleteBalls()
{
    QAnimationGroup *group = addAnimGroup(new QParallelAnimationGroup);

    int count = 0;
    for (int i = 0; i < W; i++)
        for (int j = 0; j < H; j++)
            if (vectorAt(m_deletionMap, i, j))
            {
                count++;

                setAt(i, j, 0);
                QGraphicsObject *item = vectorAt(m_fieldItems, i, j);

                QPropertyAnimation *anim = new QPropertyAnimation(item, "scale");
                anim->setEndValue(0.0);
                anim->setDuration(DISAPPEAR_DURATION);
                group->addAnimation(anim);

                anim = new QPropertyAnimation(item, "pos");
                anim->setEndValue((QPointF(0.5, 0.5) + QPoint(i, j)) * CELL_SIZE);
                anim->setDuration(DISAPPEAR_DURATION);
                group->addAnimation(anim);
            }
    int nBalls = (m_gameType == Lines) ? 5 : (m_gameType == Squares) ? 4 : 7;
#ifdef QT_DEBUG
    nBalls = 4;
#endif
    setScore(score() + count * (count - (nBalls - 1)));
    group->connect(group, SIGNAL(finished()), this, SLOT(finishDeletion()));
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void Field::finishDeletion()
{
    for (int i = 0; i < W; i++)
        for (int j = 0; j < H; j++)
            if (vectorAt(m_deletionMap, i, j))
            {
                QGraphicsObject *item = vectorAt(m_fieldItems, i, j);
                m_scene->removeItem(item);
                item->deleteLater();
                setVectorAt(m_fieldItems, i, j, (QGraphicsObject *)NULL);
            }
}

void Field::growBalls()
{
    QAnimationGroup *group = addAnimGroup(new QParallelAnimationGroup);
    bool gameOver = false;

    // Moving balls whose positions were occupied and
    // Those whose not were positioned to new positions
    for (int i = 0; i < 3; i++)
    {
        SuggestedBall ball = m_next[i];
        if (!onField(ball.pt) || at(ball.pt))
        {
            QVector<QPoint> free;
            int nFree = 0;
            free.resize(W * H);
            QList<QPoint> points;
            foreach (SuggestedBall b, m_next)
                points.append(b.pt);
            for (int i = 0; i < W; i++)
                for (int j = 0; j < H; j++)
                {
                    if (at(i, j) == 0 && !points.contains(QPoint(i, j)))
                    {
                        free[nFree] = QPoint(i, j);
                        nFree++;
                    }
                }

            if (nFree)
            {
                ball.pt = free[qrand() % nFree];
                if (ball.fieldItem == NULL)
                {
                    QGraphicsObject *item = new SvgRendererItem(ResourceLoader::instance()->ball(ball.color));
                    item->setScale(0.4);
                    item->setZValue(2);
                    m_scene->addItem(item);
                    ball.fieldItem = item;
                }
                ball.fieldItem->setPos((QPointF(0.3, 0.3) + ball.pt) * CELL_SIZE);
            }
            else
            {
                gameOver = true;
            }
        }

        if (onField(ball.pt))
        {
            setAt(ball.pt, ball.color);
            setVectorAt(m_fieldItems, ball.pt, ball.fieldItem);

            QPropertyAnimation *anim = new QPropertyAnimation(ball.fieldItem, "scale");
            anim->setEndValue(1.0);
            anim->setDuration(GROW_DURATION);
            group->addAnimation(anim);

            anim = new QPropertyAnimation(ball.fieldItem, "pos");
            anim->setEndValue(ball.pt * CELL_SIZE);
            anim->setDuration(GROW_DURATION);
            group->addAnimation(anim);

            ball.fieldItem = NULL;
        }
        m_next[i] = ball;
    }

    if (!gameOver)
    {
        if (checkForDeletion())
        {
            group->connect(group, SIGNAL(finished()), this, SLOT(deleteBalls()));
            group->connect(group, SIGNAL(finished()), this, SLOT(throwBalls()));
        }
        else if (m_field.contains(0))
            group->connect(group, SIGNAL(finished()), this, SLOT(throwBalls()));
        else
            gameOver = true;
    }

    if (gameOver)
        group->connect(group, SIGNAL(finished()), this, SIGNAL(gameOver())/*, Qt::QueuedConnection*/);

    group->start(QAbstractAnimation::DeleteWhenStopped);
}
