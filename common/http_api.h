#ifndef HTTP_API_H
#define HTTP_API_H

#include "httplib.h"
#include "nljson.h"
#if defined(HTTP_API_USE_QT)
# include <QtCore>
#endif

typedef int (*proto_start)(int port);
typedef void (*proto_stop)(int port);

using HttpApiHandler = std::function<nljson(const nljson &args)>;
#if defined(HTTP_API_USE_QT)
using HttpApiQtHandler = std::function<QVariant(const QVariant &args)>;
#endif

class HttpApiServer : public httplib::Server
{
protected:
    int m_port = -1;
public:
    explicit HttpApiServer(int port, std::size_t nWorker = 1);
    virtual ~HttpApiServer();
    int start();
    Server &HandleJsonCall(const std::string &pattern, HttpApiHandler handler);
#if defined(HTTP_API_USE_QT)
    Server &HandleVariantCall(const QString &pattern, HttpApiQtHandler handler);
#endif
    static void stop_by_port_number(int port);
};

class HttpApiClient
{
protected:
    httplib::Client *m_client = nullptr;
    int m_port = -1;
    proto_start m_start = nullptr;
    proto_stop m_stop = nullptr;
public:
    explicit HttpApiClient(int port);
    explicit HttpApiClient(const std::string &dllPath);
    explicit HttpApiClient(const void *top, std::size_t size);
    virtual ~HttpApiClient();
    nljson JsonCall(const std::string &path, const nljson &args);
#if defined(HTTP_API_USE_QT)
    QVariant VariantCall(const QString &path, const QVariant &args);
#endif
    httplib::Client &Http();
    std::string BuildPath(const std::string &path, const httplib::Params &params);
    std::string ReadFileContent(const std::string &utf8_file_path);
};


#endif // HTTP_API_H
