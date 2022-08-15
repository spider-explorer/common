#if defined(HTTP_API_USE_QT)
# include <QtCore>
#endif
#include "http_api.h"
#include "debug_line.h"
#include "url_encode.h"
#include "wstrutil.h"
#include "MemoryModule.h"

static std::mutex s_mutex;
static std::map<int, HttpApiServer *> s_server_map;

HttpApiServer::HttpApiServer(int port, std::size_t nWorker)
    : httplib::Server(), m_port(port)
{
    this->new_task_queue =
            [nWorker] {
        return new httplib::ThreadPool(nWorker);
    };
}

HttpApiServer::~HttpApiServer()
{
    debug_line2("Destructing...", m_port);
}

int HttpApiServer::start()
{
    if (m_port <= 0)
    {
        m_port = this->bind_to_any_port("127.0.0.1");
    }
    else
    {
        if (!this->bind_to_port("127.0.0.1", m_port))
        {
            m_port = -1;
        }
    }
    if (m_port <= 0) return -1;
    new std::thread([this]{
        this->listen_after_bind();
    });
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_server_map.emplace(m_port, this);
    }
    return m_port;
}

httplib::Server &HttpApiServer::HandleJsonCall(const std::string &pattern, HttpApiHandler handler)
{
    this->Post(pattern, [handler](const httplib::Request& req, httplib::Response& res)
    {
        nljson args = convert_msgpack_to_json(req.body);
        nljson result = handler(args);
        std::string mpack = convert_json_to_msgpack(result);
        res.set_content(mpack, "application/octet-stream");
    });
    return *this;
}

#if defined(HTTP_API_USE_QT)
httplib::Server &HttpApiServer::HandleVariantCall(const QString &pattern, HttpApiQtHandler handler)
{
    this->Post(pattern.toStdString(), [handler](const httplib::Request& req, httplib::Response& res)
    {
        QVariant args = convert_msgpack_to_variant(req.body);
        QVariant result = handler(args);
        std::string mpack = convert_variant_to_msgpack(result);
        res.set_content(mpack, "application/octet-stream");
    });
    return *this;
}
#endif

void HttpApiServer::stop_by_port_number(int port)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    auto it = s_server_map.find(port);
    if (it != s_server_map.end())
    {
      HttpApiServer *svr = it->second;
      svr->stop();
      debug_line2("Stopped...", port);
      s_server_map.erase(it);
      delete svr;
      debug_line2("Deleted...", port);
    }
}

HttpApiClient::HttpApiClient(int port)
{
    m_port = port;
    m_client = new httplib::Client("127.0.0.1", m_port);
}

HttpApiClient::HttpApiClient(const std::string &dllPath)
{
    HMODULE h = ::LoadLibraryW(utf8_to_wide(dllPath).c_str());
    if (!h)
    {
        throw std::invalid_argument("Could not load: " + dllPath);
    }
    m_start = (proto_start)::GetProcAddress(h, "start");
    m_stop = (proto_stop)::GetProcAddress(h, "stop");
    if (!m_start)
    {
        throw std::invalid_argument("Could not find start() from: " + dllPath);
    }
    m_port = m_start(0);
    m_client = new httplib::Client("127.0.0.1", m_port);
}

HttpApiClient::HttpApiClient(const void *top, std::size_t size)
{
    HMEMORYMODULE h = ::MemoryLoadLibrary(top, size);
    if (!h)
    {
        throw std::invalid_argument("Could not load memory module");
    }
    m_start = (proto_start)::MemoryGetProcAddress(h, "start");
    m_stop = (proto_stop)::MemoryGetProcAddress(h, "stop");
    if (!m_start)
    {
        throw std::invalid_argument("Could not find start() from memory module");
    }
    m_port = m_start(0);
    m_client = new httplib::Client("127.0.0.1", m_port);
}


HttpApiClient::~HttpApiClient()
{
    if (m_stop) m_stop(m_port);
}

nljson HttpApiClient::JsonCall(const std::string &path, const nljson &args)
{
    std::string mpack = convert_json_to_msgpack(args);
    httplib::Result result = m_client->Post(path.c_str(), mpack, "application/octet-stream");
    if (result->status != 200) return nullptr;
    nljson json = convert_msgpack_to_json(result->body);
    return json;
}

QVariant HttpApiClient::VariantCall(const QString &path, const QVariant &args)
{
    std::string mpack = convert_variant_to_msgpack(args);
    httplib::Result result = m_client->Post(path.toStdString().c_str(), mpack, "application/octet-stream");
    if (result->status != 200) return QVariant();
    QVariant variant = convert_msgpack_to_variant(result->body);
    return variant;
}

httplib::Client &HttpApiClient::Http()
{
    return *(this->m_client);
}

std::string HttpApiClient::BuildPath(const std::string &path, const httplib::Params &params)
{
    std::vector<std::string> paramsList;
    for(auto it = params.begin(); it != params.end(); ++it)
    {
        paramsList.push_back(it->first + "=" + ::url_encode(it->second));
    }
    if(paramsList.size() == 0) return path;
    return path + "?" + strutil::join(paramsList, "&");
}

std::string HttpApiClient::ReadFileContent(const std::string &utf8_file_path)
{
    std::wstring wide_path = ::utf8_to_wide(utf8_file_path);
    std::ifstream fin(std::filesystem::path(wide_path), std::ios::binary);
    std::stringstream buffer;
    buffer << fin.rdbuf();
    return buffer.str();
}
