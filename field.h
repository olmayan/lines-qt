#ifndef FIELD_H
#define FIELD_H

#include <QAnimationGroup>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QList>

#define W 9
#define H 9
#define CELL_SIZE 40
#define COLORS 6

enum GameType { Lines, Squares, Blocks };

struct SuggestedBall
{
    QPoint pt;
    int color;
    QGraphicsObject *toolbarItem;
    QGraphicsObject *fieldItem;
};

class Field : public QObject
{
    Q_OBJECT
public:
    static Field *initInstance(QGraphicsScene* scene);
    static Field *instance();
    ~Field();
    bool animating();
    void setAnimating(bool value = true);
    bool locked() const;
    GameType gameType() const;
    void throwBalls(int num, bool directly = false);
    void initGame(GameType gameType);
    void clear();
    bool onField(int x, int y);
    bool onField(QPoint point);
    void activate(int x, int y);
    void activate(QPoint point);
    void unselect();
    QPoint selected();
    void moveSelected(QPoint destination);
    bool checkForDeletion();
    int score() const;
    void setScore(int score);
    QAnimationGroup *addAnimGroup(QAnimationGroup *group);

signals:
    void scoreChanged(int);
    void gameOver();

public slots:
    void lock();
    void unlock();
    void deleteAnimGroup();
    void throwBalls();
    void deleteBalls();
    void finishDeletion();
    void growBalls();

private:
    Field(QGraphicsScene* scene);
    static Field *m_instance;

    int at(int x, int y);
    int at(QPoint point);
    void setAt(int x, int y, int value);
    void setAt(QPoint point, int value);

    template<typename T>
    static T vectorAt(QVector<T> vector, int x, int y, int width = W);
    template<typename T>
    static T vectorAt(QVector<T> vector, QPoint point, int width = W);
    template<typename T>
    static void setVectorAt(QVector<T> &vector, int x, int y, T value, int width = W);
    template<typename T>
    static void setVectorAt(QVector<T> &vector, QPoint point, T value, int width = W);

    bool m_animating;
    bool m_locked;
    QGraphicsScene* m_scene;
    QGraphicsObject* m_selItem;
    QVector<int> m_field;
    QVector<QGraphicsObject *> m_fieldItems;
    QVector<QGraphicsObject *> m_pathItems;
    QVector<SuggestedBall> m_next;
    QVector<bool> m_deletionMap;
    QList<QAnimationGroup *> m_activeAnimGroups;
    QPoint m_sel;
    int m_score;
    GameType m_gameType;
};

template<typename T>
T Field::vectorAt(QVector<T> vector, int x, int y, int width)
{
    return vector[width * y + x];
}

template<typename T>
T Field::vectorAt(QVector<T> vector, QPoint point, int width)
{
    return vector[width * point.y() + point.x()];
}

template<typename T>
void Field::setVectorAt(QVector<T> &vector, int x, int y, T value, int width)
{
    vector[width * y + x] = value;
}

template<typename T>
void Field::setVectorAt(QVector<T> &vector, QPoint point, T value, int width)
{
    vector[width * point.y() + point.x()] = value;
}

#endif // FIELD_H
