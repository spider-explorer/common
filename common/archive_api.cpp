#include <winsock2.h>
#include "archive_api.h"
#include "debug_line.h"

//#if defined(__GNUC__)
#include <archive.h>
#include <archive_entry.h>
#include <sys/utime.h>
#include <filesystem>
#include "wstrutil.h"

static std::mutex s_mutex;
static std::vector<std::int64_t> s_progress_list;

static std::int64_t extract_progress(std::uint64_t id)
{
    std::lock_guard lock(s_mutex);
    return s_progress_list[id];
}

static bool extract_archive_entry(archive *a,
                                  archive_entry *entry,
                                  const std::wstring &target,
                                  std::uint64_t id)
{
    std::wstring pathname = archive_entry_pathname_w(entry);
    std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << wide_to_ansi(pathname) << std::endl;
    std::wstring writePath = target + L"/" + pathname;
    bool isDir = strutil::ends_with(pathname, L"/");
    if(isDir) {
        return std::filesystem::create_directories(writePath);
    }
    time_t mtime = archive_entry_mtime(entry);
    std::filesystem::path file = writePath;
    std::filesystem::path dir = file.parent_path();
    std::filesystem::create_directories(dir);
    FILE *fp;;
    if((fp = _wfopen(writePath.c_str(), L"wb"))) {
        int fd = fileno(fp);
        archive_read_data_into_fd(a, fd);
        fclose(fp);
        _utimbuf utbuff;
        utbuff.actime = mtime;
        utbuff.modtime = mtime;
        _wutime(writePath.c_str(), &utbuff);
        {
            std::lock_guard lock(s_mutex);
            s_progress_list[id] = s_progress_list[id] + archive_entry_size(entry);
        }
    }
    //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << wide_to_ansi(pathname) << std::endl;
    return true;
}

static std::uint64_t extract_archive(const std::string &archivePath,
                                     const std::string &outputDir)
{
    //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << archivePath << std::endl;
    //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << outputDir << std::endl;
    std::lock_guard lock(s_mutex);
    std::uint64_t id = s_progress_list.size();
    s_progress_list.push_back(0);
    //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
    std::thread *th = new std::thread([id, archivePath, outputDir]()
    {
        //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << archivePath << std::endl;
        //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << outputDir << std::endl;
        int r;
        struct archive *a = archive_read_new();
        archive_read_support_format_all(a);
        archive_read_support_filter_all(a);
        //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
        r = archive_read_open_filename_w(a, utf8_to_wide(archivePath).c_str(), 10240);
        //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << ":" << r << std::endl;
        if (r)
        {
            return false;
        }
        //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
        for (;;)
        {
            struct archive_entry *entry;
            r = archive_read_next_header(a, &entry);
            if (r == ARCHIVE_EOF)
                break;
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(a));
            if (r < ARCHIVE_WARN)
            {
                {
                    std::lock_guard lock(s_mutex);
                    s_progress_list[id] = -2;
                }
                return false;
            }
            if(!extract_archive_entry(a,
                                      entry,
                                      utf8_to_wide(outputDir),
                                      id))
            {
                //return false;
            }
        }
        {
            std::lock_guard lock(s_mutex);
            s_progress_list[id] = -1;
        }
        //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
        archive_read_close(a);
        archive_read_free(a);
        //std::cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << std::endl;
        return true;
    });
    return id;
}

ArchiveApiServer::ArchiveApiServer(int port, std::size_t nWorker)
    : HttpApiServer(port, nWorker)
{
    this->HandleJsonCall("/extract_archive2", [](const nljson& args) -> nljson {
        std::string archive_path = args["archive_path"].get<std::string>();
        std::string output_dir = args["output_dir"].get<std::string>();
        udebug_line3("/extract_archive2", archive_path, output_dir);
        std::uint64_t id = extract_archive(archive_path, output_dir);
        return id;
    });
    this->HandleJsonCall("/extract_progress2", [](const nljson& args) -> nljson {
        std::uint64_t id = args["archive_id"].get<std::uint64_t>();
        std::int64_t progress = extract_progress(id);
        udebug_line3("/extract_progress2", id, progress);
        return progress;
    });
}

//#endif // defined(__GNUC__)

ArchiveApiClient::ArchiveApiClient(int port)
    : HttpApiClient(port)
{
}

ArchiveApiClient::ArchiveApiClient(const std::string &dllPath)
    : HttpApiClient(dllPath)
{
}

ArchiveApiClient::ArchiveApiClient(const void *top, std::size_t size)
    : HttpApiClient(top, size)
{
}

std::uint64_t ArchiveApiClient::extract_archive(const std::string &archivePath, const std::string &outputDir)
{
    auto res = this->JsonCall("/extract_archive2",
                          nljson { {"archive_path", archivePath},
                                   {"output_dir", outputDir} });
    std::uint64_t archive_id = res.get<std::uint64_t>();
    return archive_id;
}

int64_t ArchiveApiClient::extract_progress(std::uint64_t archive_id)
{
    nljson res = this->JsonCall("/extract_progress2", { {"archive_id", archive_id} });
    std::int64_t progress = res.get<std::int64_t>();
    return progress;
}

#if defined(HTTP_API_USE_QT)
quint64 ArchiveApiClient::extract_archive_qt(const QString &archivePath, const QString &outputDir)
{
    auto res = this->VariantCall("/extract_archive2",
                            QVariantMap { {"archive_path", archivePath},
                                          {"output_dir", outputDir} });
    quint64 archive_id = res.toULongLong();
    return archive_id;
}

qint64 ArchiveApiClient::extract_progress_qt(quint64 archive_id)
{
    auto res = this->VariantCall("/extract_progress2", QVariantMap { {"archive_id", archive_id} });
    qint64 progress = res.toLongLong();
    return progress;
}
#endif
