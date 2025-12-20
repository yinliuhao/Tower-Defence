#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include "_player_and_resource/resource.h"

class ResourceManager : public QObject
{
    Q_OBJECT
public:
    explicit ResourceManager(QObject* parent = nullptr);

    void addResource(ResourceType type, QPoint grid);

    void initPicture();

    Resource* checkResource(QPoint checkPoint);

    Resource* getResources(int i);
    int getResourcesNum() { return (int)allResources.size(); }

private slots:
    void onResourceDepleted(Resource* r);

signals:
    void resourceAdded(Resource * r);

private:
    std::vector<Resource*> allResources;
};

#endif // RESOURCEMANAGER_H
