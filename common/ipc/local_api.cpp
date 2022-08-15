//#undef DEBUG_LINE
#include "local_api.h"
#include <QtCore>
#include <QtNetwork>
#include "MemoryModule.h"

#include "debug_line.h"

LocalApiServer::LocalApiServer(const QString &name, QObject *parent)
    : QLocalServer(parent), m_name(name)
{
    //qdebug_line();
    connect(this, SIGNAL(newConnection()), this, SLOT(on_newConnection()));
    //qdebug_line();
}

LocalApiServer::~LocalApiServer() {}

void LocalApiServer::standby()
{
    this->listen(m_name);
    m_loop.exec();
}

void LocalApiServer::quit()
{
    m_loop.quit();
}

void LocalApiServer::Handle(const QString &method, LocalApiHandler handler)
{
    m_handler_map.insert(method, handler);
}

void LocalApiServer::on_newConnection()
{
    QLocalSocket * clientSocket = nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()),
            clientSocket, SLOT(deleteLater()));
    clientSocket->waitForReadyRead();
    QDataStream in(clientSocket);
    qint64 blockSize;
    in.setVersion(QDataStream::Qt_5_1);
    if(clientSocket->bytesAvailable() < sizeof(qint64))
        return;
    in >> blockSize;
    QString method;
    in >> method;
    qdebug_line2("method=", method);
    QVariant args;
    in >> args;
    qdebug_line2("args=", args);
    bool quit_flag = false;
    QVariant result = QVariant();
    if (method == ":quit" || method == ":stop")
    {
        quit_flag = true;
        result = true;
    }
    else if (m_handler_map.contains(method))
    {
        result = m_handler_map[method](method, args);
    }
    qdebug_line2("result=", result);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out << (qint64)0;
    out << result;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    clientSocket->write(block);
    clientSocket->flush();
    clientSocket->disconnectFromServer();
    if (quit_flag)
    {
        m_loop.quit();
    }
}

LocalApiClient::LocalApiClient(const QString &name, int timeout)
    : m_name(name), m_timeout(timeout)
{
}

LocalApiClient::LocalApiClient(const QFileInfo &dllPath, int timeout)
    : m_timeout(timeout)
{
    HMODULE h = ::LoadLibraryW(dllPath.absoluteFilePath().toStdWString().c_str());
    if (!h)
    {
        throw std::invalid_argument("Could not load: " + dllPath.absoluteFilePath().toStdString());
    }
    proto_standby standby = (proto_standby)::GetProcAddress(h, "standby");
    if (!standby)
    {
        throw std::invalid_argument("Could not find standby() from: " + dllPath.absoluteFilePath().toStdString());
    }
    QUuid id = QUuid::createUuid();
    m_name = id.toString().replace("{", "").replace("}", "");
    standby(m_name.toLatin1().toStdString().c_str());
}

LocalApiClient::LocalApiClient(const QByteArray &dllBytes, int timeout)
    : m_timeout(timeout)
{
    HMEMORYMODULE h = ::MemoryLoadLibrary(dllBytes.data(), dllBytes.size());
    if (!h)
    {
        throw std::invalid_argument("Could not load memory module");
    }
    proto_standby standby = (proto_standby)::MemoryGetProcAddress(h, "standby");
    if (!standby)
    {
        throw std::invalid_argument("Could not find standby() from dllBytes");
    }
    QUuid id = QUuid::createUuid();
    m_name = id.toString().replace("{", "").replace("}", "");
    standby(m_name.toLatin1().toStdString().c_str());
}


QVariant LocalApiClient::VariantCall(const QString &method, const QVariant &args)
{
    QEventLoop loop;
    QLocalSocket *socket = new QLocalSocket();
    QDateTime dt0 = QDateTime::currentDateTime();
    while(true)
    {
        //qdebug_line();
        if (dt0.msecsTo(QDateTime::currentDateTime()) >= m_timeout) return QVariant();
        loop.processEvents();
        socket->abort();
        socket->connectToServer(m_name);
        if (socket->waitForConnected()) break;
    }
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out << (qint64)0;
    out << method;
    out << args;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    socket->write(block);
    dt0 = QDateTime::currentDateTime();
    if (!socket->waitForReadyRead(m_timeout)) return QVariant();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_1);
    qint64 blockSize;
    in >> blockSize;
    QVariant result;
    in >> result;
    return result;
}

nljson LocalApiClient::JsonCall(const std::string &method, const nljson &args)
{
    return ::convert_variant_to_json(this->VariantCall(QString::fromStdString(method), ::convert_json_to_variant(args)));
}
