#ifndef UTILS_H
#define UTILS_H

#include <OgreResourceManager.h>
#include <OgreDataStream.h>
#include <QtCore/QByteArray>
#include <QtCore/QString>

inline QByteArray loadFile(const QString &name)
{
    using namespace Ogre;
    DataStreamPtr data = ResourceGroupManager::getSingleton().openResource(name.toStdString(), "General", false);
    int fileSize = data->size();
    QByteArray buffer(fileSize, Qt::Uninitialized);
    int totalRead = 0;
    while (totalRead < buffer.size())
        totalRead += data->read(buffer.data() + totalRead, buffer.size() - totalRead);
    return buffer;
}

#endif // UTILS_H
