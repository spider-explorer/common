#include <winsock2.h>
#include "pcloudapi.h"

#include "varianttojson.h"
#include "httplib.h"
#include "nljson.h"
#include "debug_line.h"
#include "strconv.h"
#include "url_encode.h"
#include "wstrutil.h"
#include <filesystem>

class HttpApiClientUtil {
public:
    static std::string BuildPath(const std::string &path, const httplib::Params &params)
    {
        std::vector<std::string> paramsList;
        for(auto it = params.begin(); it != params.end(); ++it)
        {
            paramsList.push_back(it->first + "=" + ::url_encode(it->second));
        }
        if(paramsList.size() == 0) return path;
        return path + "?" + strutil::join(paramsList, "&");
    }
    static std::string ReadFileContent(const std::string &utf8_file_path)
    {
        std::wstring wide_path = ::utf8_to_wide(utf8_file_path);
        std::ifstream fin(std::filesystem::path(wide_path), std::ios::binary);
        std::stringstream buffer;
        buffer << fin.rdbuf();
        return buffer.str();
    }
};

pCloudAPI::pCloudAPI()
{
#if 0x0
    QUrl url("https://api.pcloud.com/login");
    QUrlQuery query;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    query.addQueryItem("username", env.value("PCLOUD_USERNAME"));
    query.addQueryItem("password", env.value("PCLOUD_PASSWORD"));
    url.setQuery(query.query());
    QNetworkRequest request(url);
    QVariant v = m_nm.getBatchAsJson(request);
    //qDebug().noquote() << variantToJson(v, true);
    qDebug() << "auth:" << v.toMap()["auth"];
    m_auth = v.toMap()["auth"].toString();
#else
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    std::string username = env.value("PCLOUD_USERNAME").toStdString();
    std::string password = env.value("PCLOUD_PASSWORD").toStdString();
    httplib::SSLClient cli("api.pcloud.com", 443);
    httplib::Params params = { {"username", username}, {"password", password} };
    auto res = cli.Get("/login", params, httplib::Headers());
    debug_line1(res->body);
    if (!res) exit(1);
    //debug_line1(res->body);
    nljson json = nljson::parse(res->body);
    debug_line1(json.dump(2));
    debug_line1(json["auth"].get<std::string>());
    m_auth = QString::fromStdString(json["auth"].get<std::string>());
    //exit(0);
#endif
}

pCloudAPI::~pCloudAPI()
{
#if 0x0
    QUrl url("https://api.pcloud.com/logout");
    QUrlQuery query;
    query.addQueryItem("auth", m_auth);
    url.setQuery(query.query());
    QNetworkRequest request(url);
    QVariant v = m_nm.getBatchAsJson(request);
    qDebug().noquote() << variantToJson(v, true);
#else
    httplib::SSLClient cli("api.pcloud.com", 443);
    httplib::Params params = { {"auth", m_auth.toStdString()} };
    auto res = cli.Get("/logout", params, httplib::Headers());
    debug_line1(res->body);
#endif
}

void pCloudAPI::listFolder()
{
#if 0x0
    // https://api.pcloud.com/listfolder
    QUrl url("https://api.pcloud.com/listfolder");
    QUrlQuery query;
    query.addQueryItem("auth", m_auth);
    query.addQueryItem("path", "/");
    url.setQuery(query.query());
    QNetworkRequest request(url);
    QVariant v = m_nm.getBatchAsJson(request);
    qDebug() << m_nm.batchResult();
    qDebug().noquote() << variantToJson(v, true);
    QVariantList list = v.toMap()["metadata"].toMap()["contents"].toList();
    qDebug() << list.size();
    foreach(QVariant e, list)
    {
        QVariantMap entry = e.toMap();
        qDebug() << entry["path"].toString();
    }
#else
    // https://api.pcloud.com/listfolder
    QUrlQuery query;
    query.addQueryItem("auth", m_auth);
    query.addQueryItem("path", "/");
    httplib::SSLClient cli("api.pcloud.com", 443);
    httplib::Params params = { {"auth", m_auth.toStdString()}, {"path", "/"} };
    auto res = cli.Get("/listfolder", params, httplib::Headers());
    //debug_line1(res->body);
    nljson json = nljson::parse(res->body);
    debug_line1(json.dump(2));
#endif
}

void pCloudAPI::uploadFile()
{
#if 0x0
    // https://api.pcloud.com/uploadfile
    QUrl url("https://api.pcloud.com/uploadfile");
    QUrlQuery query;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    query.addQueryItem("auth", m_auth);
    query.addQueryItem("path", "/");
    query.addQueryItem("filename", "pcloud.cpp");
    url.setQuery(query.query());
    QNetworkRequest request(url);
    QFile file("pcloud.cpp");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open pcloud.cpp";
        return;
    }
    QByteArray data = file.readAll();
    qDebug().noquote() << data;
    JNetworkManager nm;
    QVariantMap result = nm.postBatch(request, "application/octet-stream", data);
    qDebug() << result;
    qDebug().noquote() << result["body"].toString();
#else
    // https://api.pcloud.com/uploadfile
    std::string content = HttpApiClientUtil::ReadFileContent("pcloud.cpp");
    httplib::SSLClient cli("api.pcloud.com", 443);
    std::string built_path = HttpApiClientUtil::BuildPath("/uploadfile", { {"auth", m_auth.toStdString()}, {"path", "/"}, {"filename", "pcloud.cpp"} });
    debug_line2("built_path=", built_path);
    auto res = cli.Post(built_path.c_str(), content, "application/octet-stream");
    nljson json = nljson::parse(res->body);
    debug_line1(json.dump(2));
#endif
}
