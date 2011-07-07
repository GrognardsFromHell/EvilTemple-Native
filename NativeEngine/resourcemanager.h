#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QtCore/QByteArray>

class ResourceManager {
public:

    static void addZipArchive(const char *group, const char *path);
    static void addDirectory(const char *group, const char *path);
    static void remove(const char *group, const char *path);
    static void initializeGroup(const char *group);
    static QByteArray *read(const char *path, const char *group = NULL);

private:
    ResourceManager();
    ~ResourceManager();
};

#endif // RESOURCEMANAGER_H
