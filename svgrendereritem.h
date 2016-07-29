#ifndef SVGRENDERERITEM_H
#define SVGRENDERERITEM_H

#include <QGraphicsObject>
#include <QPainter>
#include <QSvgRenderer>

class SvgRendererItem : public QGraphicsObject
{
    Q_PROPERTY(QPointF pos READ pos   WRITE setPos)
    Q_PROPERTY(qreal scale READ scale WRITE setscale)
public:
    SvgRendererItem(QSvgRenderer* renderer);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;
    QSvgRenderer *renderer() const;

private:
    QSvgRenderer* m_renderer;
};

#endif // SVGRENDERERITEM_H
