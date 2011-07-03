
#include <QtCore/QCoreApplication>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QStringList>

#include <Converters/clippingconverter.h>
#include <Converters/ogresystems.h>
#include <Converters/Output/conversionoutput.h>

class DirectoryConversionOutput : public IConversionOutput
{
public:
    void writeFile(const QString &path, const QByteArray &data, bool compress)
    {
        QFile f(path);
        f.open(QIODevice::Truncate|QIODevice::WriteOnly);
        f.write(data);
    }
};

int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>", argv[0]);
        return -1;
    }

    QString filename = a.arguments().at(1);

    qDebug("Loading clipping data %s", qPrintable(filename));

    QFile f(filename);

    if (!f.open(QIODevice::ReadOnly))
        qFatal("Unable to open %s.", qPrintable(filename));

    QByteArray data = f.readAll();

    f.close();

    OgreSystems ogreSystems;

    DirectoryConversionOutput output;

    ClippingConverter converter(&ogreSystems, &output);

    QElapsedTimer timer;
    timer.start();
    converter.convert(data, "./");

    qDebug("Conversion took %d milliseconds.", timer.elapsed());

    return 0;

}
