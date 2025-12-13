#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include "resource.h"

class ResourceManager : public QObject
{
    Q_OBJECT
public:
    explicit ResourceManager(QObject* parent = nullptr);

    void addResource(ResourceType type, QPoint grid);

    void initPicture();

    Resource * checkResource(QPoint checkPoint);

private slots:
    void onResourceDepleted(Resource* r);

signals:
    void resourceAdded(Resource * r);

private:
    std::vector<Resource*> allResources;
};

#endif // RESOURCEMANAGER_H
