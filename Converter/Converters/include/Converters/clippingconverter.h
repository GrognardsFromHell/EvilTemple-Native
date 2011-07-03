#ifndef CLIPPINGGEOMETRYCONVERTER_H
#define CLIPPINGGEOMETRYCONVERTER_H

#include <QtCore/QByteArray>

#include "global.h"

class OgreSystems;
class IConversionOutput;
class ClippingConverterPrivate;

namespace Troika {
    class VirtualFileSystem;
}

class CONVERTERS_EXPORT ClippingConverter
{
public:
    ClippingConverter(OgreSystems *ogreSystems, IConversionOutput *output);
    ~ClippingConverter();

    void convert(const QByteArray &clippingData, const QString &directory);
private:
    ClippingConverterPrivate *d;
};

#endif // CLIPPINGGEOMETRYCONVERTER_H
