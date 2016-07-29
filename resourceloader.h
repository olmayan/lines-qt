#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <cstddef>
#include <QSvgRenderer>

class ResourceLoader
{
public:
    static ResourceLoader *instance();
    QSvgRenderer *tile() const;
    QSvgRenderer *select() const;
    QSvgRenderer *pathmarker() const;
    QSvgRenderer *ball(int n) const;

private:
    ResourceLoader();
    static ResourceLoader* m_instance;
    QSvgRenderer *m_tile;
    QSvgRenderer *m_select;
    QSvgRenderer *m_pathmarker;
    QVector<QSvgRenderer*> m_ball;
};

#endif // RESOURCELOADER_H
