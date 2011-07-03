#ifndef CONVERSIONOUTPUT_H
#define CONVERSIONOUTPUT_H

#include <QtCore/QString>
#include <QtCore/QByteArray>

class IConversionOutput
{
public:

    virtual void writeFile(const QString &path, const QByteArray &data, bool compress = true) = 0;

    virtual ~IConversionOutput() {}

};

#endif // CONVERSIONOUTPUT_H
