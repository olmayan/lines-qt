#include "resourceloader.h"

ResourceLoader *ResourceLoader::instance()
{
    if (m_instance == NULL)
        m_instance = new ResourceLoader();
    return m_instance;
}

ResourceLoader::ResourceLoader()
{
    m_tile        = new QSvgRenderer(QString(":/images/tile.svg"));
    m_select      = new QSvgRenderer(QString(":/images/select.svg"));
    m_pathmarker  = new QSvgRenderer(QString(":/images/pathmarker.svg"));
    m_ball.resize(7);
    for (int i = 0; i < 7; i++)
    {
        m_ball[i] = new QSvgRenderer(QString(":/images/ball%1.svg").arg(i + 1));
    }
    m_error       = new QSound(":/sounds/error.wav");
    m_selectSound = new QSound(":/sounds/select.wav");
    m_pop         = new QSound(":/sounds/pop.wav");
}

QSvgRenderer *ResourceLoader::tile() const
{
    return m_tile;
}

QSvgRenderer *ResourceLoader::select() const
{
    return m_select;
}

QSvgRenderer *ResourceLoader::pathmarker() const
{
    return m_pathmarker;
}

QSvgRenderer *ResourceLoader::ball(int n) const
{
    return m_ball[n - 1];
}

QSound *ResourceLoader::error() const
{
    return m_error;
}

QSound *ResourceLoader::selectSound() const
{
    return m_selectSound;
}

QSound *ResourceLoader::pop() const
{
    return m_pop;
}

ResourceLoader *ResourceLoader::m_instance = NULL;
