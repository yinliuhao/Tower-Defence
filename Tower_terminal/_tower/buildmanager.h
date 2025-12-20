#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include <QObject>
#include "_tower/previewtower.h"
#include "_tower/tower.h"
#include "_others/utils.h"

class BuildManager : public QObject
{
    Q_OBJECT
public:
    explicit BuildManager(QObject* parent = nullptr);

    // 建造流程
    void startBuild(TowerType type);
    void cancelBuild();
    bool isBuilding() const;
    void finalBuild(int gridX, int gridY, Tower * tower) { towers[gridX][gridY] = tower; }
    Tower* getTower(int gridX, int gridY) { return towers[gridX][gridY]; }

    PreviewTower* getPreviewTower() const;
    TowerType getBuildingType() const;

    void setSelectedTower(int gridX, int gridY) { selectedTower = towers[gridX][gridY]; }
    Tower* getSelectedTower() { return selectedTower; }
    void clearSelectedTower();

    void setRangeCircle(QGraphicsItem* circle) { rangeCircle = circle; }
    QGraphicsItem* getRangeCircle() const { return rangeCircle; }

    void setUpgradeText(QGraphicsTextItem* text) { upgradeText = text; }
    QGraphicsTextItem* getUpgradeText() const { return upgradeText; }


signals:
    // 请求外部处理
    void previewCreated(PreviewTower* preview);
    void previewRemoved(PreviewTower* preview);

private:
    PreviewTower* m_previewTower = nullptr;
    TowerType     m_buildingType;
    Tower* towers[MAPHEIGHT / TILESIZE][MAPWIDTH / TILESIZE];
    Tower* selectedTower = nullptr;

    QGraphicsItem* rangeCircle = nullptr;
    QGraphicsTextItem* upgradeText = nullptr;
};

#endif // BUILDMANAGER_H
