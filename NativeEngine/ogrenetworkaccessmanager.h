#ifndef OGRENETWORKACCESSMANAGER_H
#define OGRENETWORKACCESSMANAGER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtDeclarative/QDeclarativeNetworkAccessManagerFactory>

class OgreNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit OgreNetworkAccessManager(QObject *parent = 0);

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData);

};

class OgreNetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
{
public:
    QNetworkAccessManager *create(QObject *parent)
    {
        return new OgreNetworkAccessManager(parent);
    }
};

#endif // OGRENETWORKACCESSMANAGER_H
