#include "_player_and_resource/resource.h"
#include "_others/utils.h"
#include <qpainter>
#include "_others/global.h"
#include "_others/utils.h"

Resource::Resource(ResourceType type, QPoint grid, QGraphicsItem* parent)
    : QObject(),
    QGraphicsPixmapItem(parent),
    type(type),
    grid(grid)
{
    QPointF pixalPosition = gMap->gridToPixel(grid.x(), grid.y());
    pixal = pixalPosition - QPointF(TILESIZE / 2.0, TILESIZE / 2.0);
    isSelected = false;
    respawnTimer.setSingleShot(true);
    connect(&respawnTimer, &QTimer::timeout, this, &Resource::respawn);
}

void Resource::takeDamage()
{
    if (hp <= 0) return;
    --hp;
    update();
    emit resourceDepleted(this);
}

void Resource::respawn()
{
    hp++;
    if(hp > maxHp) hp = maxHp;
    update();
}

void Resource::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    if(!isSelected)
        p->drawPixmap(0, 0, normalPic[maxHp - hp]);
    else
        p->drawPixmap(0, 0, highlightPic[maxHp - hp]);
}

//-----------------------------------------------------------

Big_rock::Big_rock(ResourceType type, QPoint grid, QGraphicsItem* parent)
    : Resource(type, grid, parent)
{
    maxHp = hp = 3;
    respawnTimeMs = BIG_STONE_TIME;
    initPicture();
    setZValue(RESOURCEZVALUE);
    setPos(getPixal());
    respawnTimer.setInterval(respawnTimeMs * 1000);
}

void Big_rock::initPicture()
{
    normalPic.resize(maxHp + 1);
    highlightPic.resize(maxHp + 1);
    switch(type)
    {
    case ResourceType::SWAMP_STONE:
        for(int i = 0; i < maxHp + 1; i++)
        {
            normalPic[i].load(QString(":/picture/resource/swamp_rock_%1.png").arg(i + 1));
            highlightPic[i].load(QString(":/picture/resource/swamp_rock_highlight_%1.png").arg(i + 1));
        }
        break;
    case ResourceType::SNOW_STONE:
        for(int i = 0; i < maxHp + 1; i++)
        {
            normalPic[i].load(QString(":/picture/resource/snow_rock_%1.png").arg(i + 1));
            highlightPic[i].load(QString(":/picture/resource/snow_rock_highlight_%1.png").arg(i + 1));
        }
        break;
    case ResourceType::GRASS_STONE:
        for(int i = 0; i < maxHp + 1; i++)
        {
            normalPic[i].load(QString(":/picture/resource/grass_rock_%1.png").arg(i + 1));
            highlightPic[i].load(QString(":/picture/resource/grass_rock_highlight_%1.png").arg(i + 1));
        }
        break;
    default:
        break;
    }
}

QRectF Big_rock::boundingRect() const
{
    return QRectF(0, 0, normalPic[0].width(), normalPic[0].height());
}

//-------------------------------------------------------------

Tree::Tree(ResourceType type, QPoint grid, QGraphicsItem* parent)
    : Resource(type, grid, parent)
{
    maxHp = hp = 1;
    respawnTimeMs = TREE_TIME;
    initPicture();
    setZValue(RESOURCEZVALUE);
    setPos(getPixal());
    respawnTimer.setInterval(respawnTimeMs * 1000);
}

void Tree::initPicture()
{
    normalPic.resize(maxHp + 1);
    highlightPic.resize(maxHp + 1);
    switch(type)
    {
    case ResourceType::SWAMP_TREE:
        for(int i = 0; i < maxHp + 1; i++)
        {
            normalPic[i].load(QString(":/picture/resource/swamp_tree_%1.png").arg(i + 1));
            highlightPic[i].load(QString(":/picture/resource/swamp_tree_highlight_%1.png").arg(i + 1));
        }
        break;
    case ResourceType::GRASS_TREE:
        for(int i = 0; i < maxHp + 1; i++)
        {
            normalPic[i].load(QString(":/picture/resource/grass_tree_%1.png").arg(i + 1));
            highlightPic[i].load(QString(":/picture/resource/grass_tree_highlight_%1.png").arg(i + 1));
        }
        break;
    default:
        break;
    }
}

QRectF Tree::boundingRect() const
{
    return QRectF(0, 0, normalPic[0].width(), normalPic[0].height());
}
