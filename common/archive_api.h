#ifndef ARCHIVE_API_H
#define ARCHIVE_API_H

#include "http_api.h"
#if defined(HTTP_API_USE_QT)
# include <QtCore>
#endif

class ArchiveApiServer : public HttpApiServer
{
public:
    explicit ArchiveApiServer(int port, std::size_t nWorker = 1);
};

class ArchiveApiClient : public HttpApiClient
{
public:
    explicit ArchiveApiClient(int port);
    explicit ArchiveApiClient(const std::string &dllPath);
    explicit ArchiveApiClient(const void *top, std::size_t size);
    std::uint64_t extract_archive(const std::string &archivePath,
                                  const std::string &outputDir);
    std::int64_t extract_progress(std::uint64_t archive_id);
#if defined(HTTP_API_USE_QT)
    quint64 extract_archive_qt(const QString &archivePath,
                               const QString &outputDir);
    qint64 extract_progress_qt(quint64 archive_id);
#endif
};

#endif // ARCHIVE_API_H
