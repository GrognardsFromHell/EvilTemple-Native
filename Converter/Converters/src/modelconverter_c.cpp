
#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <LegacyFormats/skmreader.h>
#include <LegacyFormats/troika_skeleton.h>
#include <LegacyFormats/troika_model.h>

#include "Converters/modelconverter.h"
#include "Converters/ogresystems.h"
#include "Converters/Output/conversionoutput.h"
#include "Converters/clippingconverter.h"

typedef void (__stdcall *FnAddFile)(const wchar_t *path, const char *data, unsigned int dataSize, bool compress);

struct ConversionContext : public IConversionOutput {
    OgreSystems ogre;
    ModelConverter modelConverter;
    ClippingConverter clippingConverter;
    FnAddFile addFile;

    ConversionContext();

    void writeFile(const QString &path, const QByteArray &data, bool compress);
};

void ConversionContext::writeFile(const QString &path, const QByteArray &data, bool compress)
{
    wchar_t *str = new wchar_t[path.size() + 1];
    path.toWCharArray(str);
    str[path.size()] = 0;

    addFile(str, data.constData(), data.size(), compress);
}

ConversionContext::ConversionContext() : modelConverter(&ogre), clippingConverter(&ogre, this)
{
}

static QString getSkeletonName(const QString &originalSkeletonName)
{
    QString skeletonName = originalSkeletonName;
    return skeletonName.replace(".ska", "", Qt::CaseInsensitive);
}

static QString getMaterialName(const QString &originalMaterialName)
{
    QString materialName = originalMaterialName;
    if (materialName.startsWith("art/"))
        materialName = materialName.mid(4);
    return materialName.replace(".mdf", "").toLower();
}

ConversionContext* __stdcall modelconverter_ctor(FnAddFile addFileCallback)
{
    ConversionContext *result = new ConversionContext;
    result->modelConverter.setGetSkeletonNameFn(getSkeletonName);
    result->modelConverter.setGetMaterialNameFn(getMaterialName);
    result->addFile = addFileCallback;
    return result;
}

int __stdcall modelconverter_convert(ConversionContext *context,
                            const char *modelDataIn, int modelDataSize,
                            const char *skeletonDataIn, int skeletonDataSize,
                            const wchar_t *modelFilenameOut, const wchar_t *skeletonFilenameOut)
{
    ModelConverter *converter = &context->modelConverter;

    QString qModelFilenameOut = QString::fromWCharArray(modelFilenameOut);
    QString qSkeletonFilenameOut;
    if (skeletonFilenameOut)
        qSkeletonFilenameOut = QString::fromWCharArray(skeletonFilenameOut);

    QByteArray data = QByteArray::fromRawData(modelDataIn, modelDataSize);

    /* Read the model file */
    Troika::SkmReader reader(qModelFilenameOut, data);
    QScopedPointer<Troika::MeshModel> model(reader.get());

    if (skeletonDataSize > 0) {
        data = QByteArray::fromRawData(skeletonDataIn, skeletonDataSize);

        auto skeleton = new Troika::Skeleton(data, qSkeletonFilenameOut);
        if (skeleton->bones().size() < 256)
            model->setSkeleton(skeleton);
        else
            qWarning("Unable to convert skeleton with %d bones (%s).",
                     skeleton->bones().size(),
                     qPrintable(qSkeletonFilenameOut));
    }

    data = converter->convertMesh(*model);

    context->addFile(qModelFilenameOut.utf16(), data.constData(), data.size(), true);

    if (model->skeleton()) {
        data = converter->convertSkeleton(*model);
        context->addFile(qSkeletonFilenameOut.utf16(), data.constData(), data.size(), true);
    }

    return 0;
}

int __stdcall clippingconverter_convert(ConversionContext *context,
                                        const char *clippingData,
                                        int clippingDataSize,
                                        const wchar_t *directory)
{
    QByteArray clippingDataArray = QByteArray::fromRawData(clippingData, clippingDataSize);
    context->clippingConverter.convert(clippingDataArray, QString::fromWCharArray(directory));
    return 0;
}

void __stdcall modelconverter_dtor(ConversionContext *context)
{
    delete context;
}
