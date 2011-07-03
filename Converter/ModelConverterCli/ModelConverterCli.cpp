
#include <QtCore/QCoreApplication>
#include <QtCore/QElapsedTimer>

#include <Converters/modelconverter.h>
#include <Converters/ogresystems.h>
#include <LegacyFormats/troika_model.h>
#include <LegacyFormats/skmreader.h>
#include <LegacyFormats/virtualfilesystem.h>
#include <LegacyFormats/troika_materials.h>

void convertMesh(ModelConverter *converter, const Troika::MeshModel &model);
void convertSkeleton(ModelConverter *converter, const Troika::MeshModel &model);

static QString getSkeletonFilename(const QString &filename) {
    return "output.skeleton";
}

static QString getMaterialFilename(const QString &material) {
    QString result = material;
    result.replace(".mdf", "");
    return result;
}

int main(int argc, char *argv[]) {

    QCoreApplication a(argc, argv);

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <toee-path> <filename.skm>", argv[0]);
        return -1;
    }

    QString toeePath = a.arguments().at(1);
    QString filename = a.arguments().at(2);

    if (!toeePath.endsWith('/') && !toeePath.endsWith('\\'))
        toeePath.append('/');

    qDebug("Loading model %s", qPrintable(filename));

    Troika::VirtualFileSystem vfs;
    vfs.loadDefaultArchives(toeePath);
    Troika::Materials materials(&vfs);
	QByteArray data = vfs.openFile(filename);
    Troika::SkmReader reader(filename, data);

	
    QScopedPointer<Troika::MeshModel> troikaModel(reader.get());

    if (!troikaModel) {
        qCritical("Unable to load model %s", qPrintable(filename));
        return -2;
    }

	QString skeletonFilename = filename.replace(".skm", ".ska", Qt::CaseInsensitive);
	if (vfs.exists(skeletonFilename)) {
		Troika::Skeleton *skeleton = new Troika::Skeleton(vfs.openFile(skeletonFilename), filename);
		troikaModel->setSkeleton(skeleton);
	}
	
	OgreSystems ogre;

    ModelConverter converter(&ogre);
    converter.setGetMaterialNameFn(getMaterialFilename);
    converter.setGetSkeletonNameFn(getSkeletonFilename);

    QElapsedTimer timer;
    timer.start();
    convertMesh(&converter, *troikaModel.data());

    if (troikaModel->skeleton())
        convertSkeleton(&converter, *troikaModel.data());
    qDebug("Conversion took %d milliseconds.", timer.elapsed());

    return 0;

}

void convertMesh(ModelConverter *converter, const Troika::MeshModel &model)
{
    QByteArray meshData = converter->convertMesh(model);

    if (meshData.isEmpty()) {
        qCritical("Unable to serialize Ogre mesh.");
        return;
    }

    QString outputFilename = "output.mesh";

    /* Write the converted ogre mesh to a file */
    QFile out(outputFilename);
    if (!out.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        qCritical("Cannot open %s for writing.", qPrintable(outputFilename));
    }

    out.write(meshData);
    out.close();
}

void convertSkeleton(ModelConverter *converter, const Troika::MeshModel &model)
{
    QByteArray skeletonData = converter->convertSkeleton(model);

    if (skeletonData.isEmpty()) {
        qCritical("Unable to serialize Ogre skeleton.");
        return;
    }

    QString outputFilename = "output.skeleton";

    /* Write the converted ogre skeleton to a file */
    QFile out(outputFilename);
    if (!out.open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        qCritical("Cannot open %s for writing.", qPrintable(outputFilename));
    }

    out.write(skeletonData);
    out.close();
}
