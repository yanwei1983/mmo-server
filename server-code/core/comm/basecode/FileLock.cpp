#include "FileLock.h"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "LoggingMgr.h"

file_lock::file_lock(const std::string& programname)
{
    m_pid_file = "./.pid/" + programname + ".pid";
    m_fd       = -1;
}

file_lock::~file_lock() {}

bool file_lock::kill(int32_t sig)
{
    int32_t fd = open(m_pid_file.c_str(), O_TRUNC | O_RDWR);
    if(fd < 0)
    {
        //BaseCode::PrintfError("file_lock::kill open fd = {}, {}, {}", fd, sig, m_pid_file.c_str());
        return false;
    }

    struct flock lock;
    lock.l_type   = F_WRLCK;
    lock.l_start  = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len    = 0;

    if(fcntl(fd, F_GETLK, reinterpret_cast<long>(&lock)) == -1)
    {
        BaseCode::PrintfError("file_lock::kill fcntl fd = {}, {}, {}", fd, sig, m_pid_file.c_str());
        return false;
    }

    if(lock.l_type == F_UNLCK)
    {
        // BaseCode::PrintfError("file_lock::kill lock.l_type == F_UNLCK, {}", m_pid_file.c_str());
        return true;
    }

    // if (::kill(lock.l_pid, sig) == -1)
    // 这里暂时写死，Kill, unblockable (POSIX).
    if(::kill(lock.l_pid, sig) == -1)
    {
        BaseCode::PrintfError("file_lock::kill ::kill(lock.l_pid, sig) == -1, pid = {}, sig = {}, errno = {}, {}, {}",
                              lock.l_pid,
                              sig,
                              strerror(errno),
                              errno,
                              m_pid_file.c_str());
        return false;
    }
    else
    {
        return true;
    }
}

bool file_lock::lock()
{
    m_fd = open(m_pid_file.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if(m_fd < 0)
    {
        BaseCode::PrintfError("file_lock::lock fail m_fd = {},{}({}), {}", m_fd, strerror(errno), errno, m_pid_file.c_str());
        return false;
    }
    int32_t lock_result = lockf(m_fd, F_TLOCK, 0);
    if(lock_result < 0)
    {
        // lockf F_LOCK failure.
        // BaseCode::PrintfError("file_lock::lock fail F_LOCK lock_result = {}, {}", lock_result, m_pid_file.c_str());
        return false;
    }
    std::string sid = std::to_string(getpid());
    write(m_fd, sid.c_str(), sid.size());
    fsync(m_fd);
    return true;
}

void file_lock::unlock()
{
    if(m_fd < 0)
    {
        return;
    }
    lockf(m_fd, F_ULOCK, 0);
}