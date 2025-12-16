#include "previewtower.h"
#include "QPainter"
#include "utils.h"

PreviewTower::PreviewTower(TowerType type):
    type(type),
    isValid(false)
{
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);

    setZValue(PREVIEWTOWERZVALUE);

    switch(type)
    {
    case TowerType::TOWER1:
        attackRadius = TOWER1_ATTACKRANGE;
        picture.load(QString(":/picture/tower/preview_archer.png"));
        break;
    case TowerType::TOWER2:
        attackRadius = TOWER2_ATTACKRANGE;
        picture.load(QString(":/picture/tower/preview_cannon.png"));
        break;
    case TowerType::TOWER3:
        attackRadius = TOWER3_ATTACKRANGE;
        picture.load(QString(":/picture/tower/preview_mortar.png"));
        break;
    default:
        break;
    }
}

void PreviewTower::setGridPos(const QPoint grid)
{
    gridPos = grid;

    // 对齐到格子中心
    qreal y = grid.x() * TILESIZE + TILESIZE / 2;
    qreal x = grid.y() * TILESIZE + TILESIZE / 2;

    setPos(x, y);
    update();
}

void PreviewTower::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem*,
                         QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    // ===== 攻击范围 =====
    QColor rangeColor = isValid
                            ? QColor(0, 200, 0, 80)
                            : QColor(200, 0, 0, 80);

    if (type != TowerType::TOWER3)
    {
        // 普通塔：实心圆
        painter->setBrush(rangeColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(0, 0),
                             attackRadius,
                             attackRadius);
    }
    else
    {
        // 迫击炮：圆环
        qreal outerR = attackRadius;
        qreal innerR = attackRadius / 3;

        QPainterPath path;
        path.setFillRule(Qt::OddEvenFill);   // 关键！！
        path.addEllipse(QPointF(0, 0), outerR, outerR);
        path.addEllipse(QPointF(0, 0), innerR, innerR);

        painter->setBrush(rangeColor);
        painter->setPen(Qt::NoPen);
        painter->drawPath(path);
    }

    // ===== 炮塔本体 =====
    painter->setOpacity(0.6);
    painter->drawPixmap(
        -picture.width() / 2,
        -picture.height() / 2,
        picture
        );
}

QRectF PreviewTower::boundingRect() const
{
    qreal half = attackRadius;
    return QRectF(-half, -half, half * 2, half * 2);
}
