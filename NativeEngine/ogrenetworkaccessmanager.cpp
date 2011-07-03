
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <OgreResourceGroupManager.h>
#include <OgreDataStream.h>

#include "ogrenetworkaccessmanager.h"

const QString ogreScheme("ogre");

class OgreNetworkReply : public QNetworkReply
{
public:
    OgreNetworkReply(const QNetworkRequest &request);

    void abort();

    bool isFinished() const;
    bool isSequential() const;
    qint64 bytesAvailable() const;
    qint64 size() const;
    bool seek(qint64 pos);

    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char * data, qint64 maxSize);
private:
    Ogre::DataStreamPtr mDataStream;
};

OgreNetworkReply::OgreNetworkReply(const QNetworkRequest &request)
{
    setOperation(QNetworkAccessManager::GetOperation);
    setRequest(request);
    setUrl(request.url());

    QString path = request.url().toString(QUrl::RemoveScheme);

    // Remote slashes at the beginning
    while (path.startsWith('/'))
        path = path.mid(1);

    qDebug() << "Opening" << path << "from ogre resource.";

    Ogre::ResourceGroupManager &resourceManager = Ogre::ResourceGroupManager::getSingleton();

    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");

    /* Is it a directory? */
    Ogre::FileInfoListPtr fileInfo = resourceManager.findResourceFileInfo("General", path.toStdString(), true);
    if (fileInfo->size() > 0) {
        QString msg = QString("Cannot open %1: Path is a directory").arg(path);
        setError(ContentOperationNotPermittedError, msg);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ContentOperationNotPermittedError));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        return;
    }

    try {
        mDataStream = resourceManager.openResource(path.toStdString());
    } catch (Ogre::FileNotFoundException &e) {
        qWarning("Couldn't find %s: %s", qPrintable(path), e.getFullDescription().c_str());
        setError(ContentNotFoundError, "Couldn't find " + path);
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ContentNotFoundError));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        return;
    }

    open(QIODevice::ReadOnly);

    setHeader(QNetworkRequest::ContentLengthHeader, mDataStream->size());

    QMetaObject::invokeMethod(this, "metaDataChanged", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "downloadProgress", Qt::QueuedConnection,
                              Q_ARG(qint64, mDataStream->size()), Q_ARG(qint64, mDataStream->size()));
    QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
}

void OgreNetworkReply::abort()
{
    if (!mDataStream.isNull()) {
        mDataStream->close();
        mDataStream.setNull();
    }
}

bool OgreNetworkReply::isFinished() const
{
    return true;
}

bool OgreNetworkReply::isSequential() const
{
    return false;
}

bool OgreNetworkReply::seek(qint64 pos)
{
    QNetworkReply::seek(pos);
    mDataStream->seek(pos);
    return (mDataStream->tell() == pos);
}

qint64 OgreNetworkReply::bytesAvailable() const
{
    if (mDataStream.isNull())
        return 0;
    return QNetworkReply::bytesAvailable() + mDataStream->size() - mDataStream->tell();
}

qint64 OgreNetworkReply::size() const
{
    return mDataStream->size();
}

qint64 OgreNetworkReply::readData(char *data, qint64 maxSize)
{
    if (mDataStream.isNull()) {
        setErrorString("This network request is closed.");
        return -1;
    }

    if (mDataStream->eof())
        return -1;

    return mDataStream->read(data, maxSize);
}

qint64 OgreNetworkReply::writeData(const char * data, qint64 maxSize)
{
    setErrorString("Unable to write to an Ogre network request.");
    return -1;
}

OgreNetworkAccessManager::OgreNetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

QNetworkReply *OgreNetworkAccessManager::createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0)
{
    /* This network manager only handles "ogre://" urls */
    QString scheme = req.url().scheme();

    if (scheme.toLower() != ogreScheme)
        return QNetworkAccessManager::createRequest(op, req, outgoingData);

    return new OgreNetworkReply(req);
}
