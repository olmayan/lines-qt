#include "graphicsview.h"

GraphicsView::GraphicsView(QGraphicsScene *scene)
    : QGraphicsView(scene)
{

}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    emit mouseReleased(event);
}
