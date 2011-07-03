#ifndef CONVERTERS_MESHCONVERTER_H
#define CONVERTERS_MESHCONVERTER_H

#include <QtCore/QByteArray>

#include "global.h"

namespace Troika {
    class MeshModel;
}

class IConversionOutput;
class ModelConverterPrivate;
class OgreSystems;

typedef QString (*GetMaterialNameFn)(const QString &);
typedef QString (*GetSkeletonNameFn)(const QString &);

class CONVERTERS_EXPORT ModelConverter
{
public:
    ModelConverter(OgreSystems*);
    ~ModelConverter();

    QByteArray convertMesh(const Troika::MeshModel &model);

    QByteArray convertSkeleton(const Troika::MeshModel &model);

    void setGetMaterialNameFn(GetMaterialNameFn fn);
    void setGetSkeletonNameFn(GetSkeletonNameFn fn);

private:
    ModelConverterPrivate *d;
};

#endif // CONVERTERS_MESHCONVERTER_H
