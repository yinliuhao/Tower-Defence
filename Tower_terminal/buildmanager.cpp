#include "buildmanager.h"
#include <QGraphicsScene>

BuildManager::BuildManager(QObject* parent)
    : QObject(parent)
{
}

void BuildManager::startBuild(TowerType type)
{
    // 已在建造 → 先取消
    cancelBuild();

    m_buildingType = type;

    m_previewTower = new PreviewTower(type);

    // 不碰 scene，只通知外部
    emit previewCreated(m_previewTower);
}

void BuildManager::cancelBuild()
{
    if (!m_previewTower)
        return;

    // 通知外部移除
    emit previewRemoved(m_previewTower);

    delete m_previewTower;
    m_previewTower = nullptr;
}

bool BuildManager::isBuilding() const
{
    return m_previewTower != nullptr;
}

PreviewTower* BuildManager::getPreviewTower() const
{
    return m_previewTower;
}

TowerType BuildManager::getBuildingType() const
{
    return m_buildingType;
}
