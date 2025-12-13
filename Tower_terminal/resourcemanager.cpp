#include "resource.h"
#include "resourcemanager.h"
#include "global.h"
#include <vector>

ResourceManager::ResourceManager(QObject *):
    QObject()
{
}

void ResourceManager::initPicture()
{
    for(int i = 0; i < int(gMap->grassStone.size()); i++)
    {
        addResource(ResourceType::GRASS_STONE, gMap->grassStone[i]);
    }

    for(int i = 0; i < int(gMap->swampStone.size()); i++)
    {
        addResource(ResourceType::SWAMP_STONE, gMap->swampStone[i]);
    }

    for(int i = 0; i < int(gMap->snowStone.size()); i++)
    {
        addResource(ResourceType::SNOW_STONE, gMap->snowStone[i]);
    }

    for(int i = 0; i < int(gMap->grassTree.size()); i++)
    {
        addResource(ResourceType::GRASS_TREE, gMap->grassTree[i]);
    }

    for(int i = 0; i < int(gMap->swampTree.size()); i++)
    {
        addResource(ResourceType::SWAMP_TREE, gMap->swampTree[i]);
    }
}

void ResourceManager::addResource(ResourceType type, QPoint grid)
{
    Resource* r;
    if(type == ResourceType::GRASS_TREE || type == ResourceType::SWAMP_TREE)
    {
        r = new Tree(type, grid);
        allResources.push_back(r);
    }
    else
    {
        r = new Big_rock(type, grid);
        allResources.push_back(r);
    }
    connect(r, &Resource::resourceDepleted,
            this, &ResourceManager::onResourceDepleted);
    emit resourceAdded(r);
}

void ResourceManager::onResourceDepleted(Resource* r)
{
    r->setTimerStart();
}

Resource * ResourceManager::checkResource(QPoint checkPoint)
{
    for(int i = 0; i < int(allResources.size()); i++)
    {
        if(allResources[i]->isDead())  continue;

        if(allResources[i]->getType() == ResourceType::GRASS_TREE ||
           allResources[i]->getType() == ResourceType::SWAMP_TREE)
        {
            QPoint point = checkPoint + QPoint(-1, 0);
            if(point == allResources[i]->getGrid()) return allResources[i];
        }
        else
        {
            static const QPoint dirs[] = {
                                          {0, 0}, {0, -1}, {-1, 0}, {-1, -1}
            };
            for (const QPoint& d : dirs)
            {
                QPoint point = checkPoint + d;
                if(point == allResources[i]->getGrid()) return allResources[i];
            }
        }
    }
    return nullptr;
}

Resource* ResourceManager::getResources(int i)
{
    if(i < (int)allResources.size())
        return allResources[i];
    else
        return nullptr;
}
