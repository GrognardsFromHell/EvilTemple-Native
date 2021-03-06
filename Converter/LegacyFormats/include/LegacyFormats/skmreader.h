#ifndef SKMREADER_H
#define SKMREADER_H

#include "troikaformatsglobal.h"

#include <QString>
#include <QSharedPointer>

#include "modelsource.h"

namespace Troika
{

    class VirtualFileSystem;
    class Materials;
    class MeshModel;
    class SkmReaderData;

    class TROIKAFORMATS_EXPORT SkmReader : public ModelSource
    {
    public:
        SkmReader(const QString &filename, const QByteArray &data);
        ~SkmReader();

        MeshModel *get();

    private:
        QScopedPointer<SkmReaderData> d_ptr;
    };

}

#endif // SKMREADER_H
