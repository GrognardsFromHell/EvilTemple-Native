
#include <QtCore/QByteArray>
#include <QtCore/QScopedPointer>
#include <Ogre.h>

#include "resourcemanager.h"

void ResourceManager::addZipArchive(const char *group, const char *path)
{
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "Zip", group, true);
}

void ResourceManager::addDirectory(const char *group, const char *path)
{
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", group, true);
}

void ResourceManager::remove(const char *group, const char *path)
{
    Ogre::ResourceGroupManager::getSingleton().removeResourceLocation(path);
}

void ResourceManager::initializeGroup(const char *group)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(group);
}

QByteArray *ResourceManager::read(const char *path, const char *group)
{
    Ogre::String groupName = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
    if (group)
        groupName = group;

    Ogre::DataStreamPtr dataStream = Ogre::ResourceGroupManager::getSingleton().openResource(path, groupName);

    int totalSize = dataStream->size();
    QScopedPointer<QByteArray> result(new QByteArray(totalSize, Qt::Uninitialized));

    int bytesRead = 0;
    char *buffer = result->data();
    while (bytesRead < totalSize) {
        int read = dataStream->read(buffer, totalSize - bytesRead);
        buffer += read;
        bytesRead += read;
    }

    return result.take();
}
