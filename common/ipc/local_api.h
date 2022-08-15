#ifndef LOCAL_API_H
#define LOCAL_API_H

#include <QtCore>
#include <QtNetwork>
#include "nljson.h"

using LocalApiHandler = std::function<QVariant(const QString &method, const QVariant &args)>;

class LocalApiServer : public QLocalServer
{
    Q_OBJECT
protected:
    QString m_name;
    QMap<QString, LocalApiHandler> m_handler_map;
    QEventLoop m_loop;
public:
    explicit LocalApiServer(const QString &name, QObject * parent = 0);
    ~LocalApiServer();
    void standby();
    void quit();
    void Handle(const QString &method, LocalApiHandler handler);
private slots:
    void on_newConnection();
};

typedef const char *(*proto_standby)(const char *name);

class LocalApiClient
{
protected:
    QString m_name;
    int m_timeout;
public:
    LocalApiClient(const QString &name, int timeout = 500);
    LocalApiClient(const QFileInfo &dllPath, int timeout = 500);
    LocalApiClient(const QByteArray &dllBytes, int timeout = 500);
    QVariant VariantCall(const QString &method, const QVariant &args);
    nljson JsonCall(const std::string &method, const nljson &args);
};

#endif // LOCAL_API_H
