#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include <QObject>
#include "previewtower.h"

class BuildManager : public QObject
{
    Q_OBJECT
public:
    explicit BuildManager(QObject* parent = nullptr);

    // 建造流程
    void startBuild(TowerType type);
    void cancelBuild();
    bool isBuilding() const;

    PreviewTower* getPreviewTower() const;
    TowerType getBuildingType() const;

signals:
    // 请求外部处理
    void previewCreated(PreviewTower* preview);
    void previewRemoved(PreviewTower* preview);

private:
    PreviewTower* m_previewTower = nullptr;
    TowerType     m_buildingType;
};

#endif // BUILDMANAGER_H
