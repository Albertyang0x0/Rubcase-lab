#include "storage/disk_manager.h"

#include <assert.h>    // for assert
#include <string.h>    // for memset
#include <sys/stat.h>  // for stat
#include <unistd.h>    // for lseek

#include "defs.h"

DiskManager::DiskManager() { memset(fd2pageno_, 0, MAX_FD * (sizeof(std::atomic<page_id_t>) / sizeof(char))); }

/**
 * @brief Write the contents of the specified page into disk file
 *
 */
void DiskManager::write_page(int fd, page_id_t page_no, const char *offset, int num_bytes) {
    // Todo:
    // 1.lseek()定位到文件头，通过(fd,page_no)可以定位指定页面及其在磁盘文件中的偏移量
    // 2.调用write()函数
    // 注意处理异常

    //通过 lseek 将文件描述符 fd 的偏移量设置为 page_no * PAGE_SIZE，其中 PAGE_SIZE 是页面大小。这样就定位到了文件中特定页面的位置
    lseek(fd, page_no * PAGE_SIZE, SEEK_SET);  // PAGE_SIZE 是页面大小，需要根据实际情况替换
    ssize_t bytes_written = write(fd, offset, num_bytes);
    if (bytes_written != num_bytes) {
        throw UnixError();
    }

}

/**
 * @brief Read the contents of the specified page into the given memory area
 */
void DiskManager::read_page(int fd, page_id_t page_no, char *offset, int num_bytes) {
    // Todo:
    // 1.lseek()定位到文件头，通过(fd,page_no)可以定位指定页面及其在磁盘文件中的偏移量
    // 2.调用read()函数
    // 注意处理异常
    lseek(fd, page_no * PAGE_SIZE, SEEK_SET);  // PAGE_SIZE 是页面大小，需要根据实际情况替换
    ssize_t bytes_read = read(fd, offset, num_bytes);
    if (bytes_read != num_bytes) {
        throw UnixError();
    }

}

/**
 * @brief Allocate new page (operations like create index/table)
 * For now just keep an increasing counter
 */
//返回当前文件描述符 fd 对应的页面编号，并将该编号加1，以便下一次调用时返回不同的页面编号
page_id_t DiskManager::AllocatePage(int fd) {
    // Todo:
    // 简单的自增分配策略，指定文件的页面编号加1

    return fd2pageno_[fd]++;
}

/**
 * @brief Deallocate page (operations like drop index/table)
 * Need bitmap in header page for tracking pages
 * This does not actually need to do anything for now.
 */
void DiskManager::DeallocatePage(__attribute__((unused)) page_id_t page_id) {}

bool DiskManager::is_dir(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

void DiskManager::create_dir(const std::string &path) {
    // Create a subdirectory
    std::string cmd = "mkdir " + path;
    if (system(cmd.c_str()) < 0) {  // 创建一个名为path的目录
        throw UnixError();
    }
}

void DiskManager::destroy_dir(const std::string &path) {
    std::string cmd = "rm -r " + path;
    if (system(cmd.c_str()) < 0) {
        throw UnixError();
    }
}

/**
 * @brief 用于判断指定路径文件是否存在 
 */
bool DiskManager::is_file(const std::string &path) {
    // Todo:
    // 创建 struct stat 对象 st。
    //使用 stat 函数获取文件信息，将结果存储在 st 中。
    //检查 stat 函数的返回值是否为 0，表示获取文件信息成功。
    //判断 st.st_mode 是否表示一个常规文件（regular file），使用 S_ISREG 宏。
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);

}

/**
 * @brief 用于创建指定路径文件
 */
void DiskManager::create_file(const std::string &path) {
    // Todo:
    // 调用open()函数，使用O_CREAT模式
    // 注意不能重复创建相同文件
    if (!is_file(path)) {
        int fd = open(path.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);//使用 S_IRUSR | S_IWUSR 设置文件的权限，这里表示文件的所有者拥有读和写的权限。
        if (fd < 0) {
            throw UnixError();
        }
        close(fd);
    }else{
        throw FileExistsError(path);}

}

/**
 * @brief 用于删除指定路径文件 
 */
void DiskManager::destroy_file(const std::string &path) {
    // Todo:
    // 调用unlink()函数
    // 注意不能删除未关闭的文件
     if (is_file(path)) {
        if (unlink(path.c_str()) < 0) {
            throw UnixError();
        }
    }else{throw FileNotFoundError(path);}
    
}

/**
 * @brief 用于打开指定路径文件
 */
int DiskManager::open_file(const std::string &path) {
    // Todo:
    // 调用open()函数，使用O_RDWR模式
    // 注意不能重复打开相同文件，并且需要更新文件打开列表
    if (is_file(path) && !path2fd_.count(path)) {//使用 path2fd_.count(path) 检查文件是否已经被打开，如果文件已经被打开则不能重复打开
        int fd = open(path.c_str(), O_RDWR);
        if (fd < 0) {
            throw UnixError();
        }
        path2fd_[path] = fd;
        fd2path_[fd] = path;
        return fd;
    }else{throw FileNotFoundError(path);}

    return -1;
}

/**
 * @brief 用于关闭指定路径文件
 */
void DiskManager::close_file(int fd) {
    // Todo:
    // 调用close()函数
    // 注意不能关闭未打开的文件，并且需要更新文件打开列表
    if (fd >= 0 && fd2path_.count(fd)) {//检查文件描述符 fd 是否大于等于0，以及是否在文件打开列表 fd2path_ 中存在。
        close(fd);
        path2fd_.erase(fd2path_[fd]);
        fd2path_.erase(fd);
    }
    
}

int DiskManager::GetFileSize(const std::string &file_name) {
    struct stat stat_buf;
    int rc = stat(file_name.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

std::string DiskManager::GetFileName(int fd) {
    if (!fd2path_.count(fd)) {
        throw FileNotOpenError(fd);
    }
    return fd2path_[fd];
}

int DiskManager::GetFileFd(const std::string &file_name) {
    if (!path2fd_.count(file_name)) {
        return open_file(file_name);
    }
    return path2fd_[file_name];
}

bool DiskManager::ReadLog(char *log_data, int size, int offset, int prev_log_end) {
    // read log file from the previous end
    if (log_fd_ == -1) {
        log_fd_ = open_file(LOG_FILE_NAME);
    }
    offset += prev_log_end;
    int file_size = GetFileSize(LOG_FILE_NAME);
    if (offset >= file_size) {
        return false;
    }

    size = std::min(size, file_size - offset);
    lseek(log_fd_, offset, SEEK_SET);
    ssize_t bytes_read = read(log_fd_, log_data, size);
    if (bytes_read != size) {
        throw UnixError();
    }
    return true;
}

void DiskManager::WriteLog(char *log_data, int size) {
    if (log_fd_ == -1) {
        log_fd_ = open_file(LOG_FILE_NAME);
    }

    // write from the file_end
    lseek(log_fd_, 0, SEEK_END);
    ssize_t bytes_write = write(log_fd_, log_data, size);
    if (bytes_write != size) {
        throw UnixError();
    }
}
