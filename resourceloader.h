#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <cstddef>
#include <QSound>
#include <QSvgRenderer>

class ResourceLoader
{
public:
    static ResourceLoader *instance();
    QSvgRenderer *tile() const;
    QSvgRenderer *select() const;
    QSvgRenderer *pathmarker() const;
    QSvgRenderer *ball(int n) const;
    QSound *error() const;
    QSound *selectSound() const;
    QSound *pop() const;

private:
    ResourceLoader();
    static ResourceLoader* m_instance;
    QSvgRenderer *m_tile;
    QSvgRenderer *m_select;
    QSvgRenderer *m_pathmarker;
    QVector<QSvgRenderer*> m_ball;
    QSound *m_error;
    QSound *m_selectSound;
    QSound *m_pop;
};

#endif // RESOURCELOADER_H
