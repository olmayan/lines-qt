#include "svgrendereritem.h"

SvgRendererItem::SvgRendererItem(QSvgRenderer *renderer)
{
    m_renderer = renderer;
}

void SvgRendererItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    m_renderer->render(painter, boundingRect());
}

QRectF SvgRendererItem::boundingRect() const
{
    //QSizeF size = m_renderer->viewBoxF().size();
    //return QRectF(pos(), size.scaled(size.width() * scale(), size.height() * scale(), Qt::KeepAspectRatio));
    return m_renderer->viewBoxF();
}

QSvgRenderer *SvgRendererItem::renderer() const
{
    return m_renderer;
}
