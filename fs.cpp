#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <sys/stat.h>
using namespace std;

const char* DISK_NAME = "disk.sim";
const int DISK_SIZE = 1024 * 1024;
const int METADATA_SIZE = 4096;
const int MAX_FILES = 85;
const int FILENAME_SIZE = 32;
const int BLOCK_SIZE = 512;

struct FileEntry {
    char name[FILENAME_SIZE];
    int size;
    int start;
    long long created;
};

FileEntry entries[MAX_FILES];

int get_file_count(int fd) {
    int count;
    lseek(fd, 0, SEEK_SET);
    read(fd, &count, sizeof(int));
    return count;
}

void set_file_count(int fd, int count) {
    lseek(fd, 0, SEEK_SET);
    write(fd, &count, sizeof(int));
}

void load_entries(int fd) {
    lseek(fd, sizeof(int), SEEK_SET);
    read(fd, entries, sizeof(entries));
}

void save_entries(int fd) {
    lseek(fd, sizeof(int), SEEK_SET);
    write(fd, entries, sizeof(entries));
}

int find_file(const char* filename, int file_count) {
    for (int i = 0; i < file_count; ++i) {
        if (strcmp(entries[i].name, filename) == 0) {
            return i;
        }
    }
    return -1;
}

void fs_format() {
    int fd = open(DISK_NAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
    char buffer[BLOCK_SIZE] = {0};
    for (int i = 0; i < DISK_SIZE / BLOCK_SIZE; ++i) {
        write(fd, buffer, BLOCK_SIZE);
    }
    close(fd);
}

int fs_create(const char* filename) {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    if (file_count >= MAX_FILES) {
        close(fd);
        return -2;  
    }
    if (find_file(filename, file_count) != -1) {
        close(fd);
        return -1;  
    }
    FileEntry& e = entries[file_count];
    memset(&e, 0, sizeof(FileEntry));
    strncpy(e.name, filename, FILENAME_SIZE - 1);
    e.size = 0;
    e.start = METADATA_SIZE + file_count * BLOCK_SIZE;
    e.created = time(nullptr);
    set_file_count(fd, file_count + 1);
    save_entries(fd);
    close(fd);
    return 0; 
}

void fs_delete(const char* filename) {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(filename, file_count);
    if (index == -1) {
        close(fd);
        return;
    }
    for (int i = index; i < file_count - 1; ++i) {
        entries[i] = entries[i + 1];
    }
    memset(&entries[file_count - 1], 0, sizeof(FileEntry));
    set_file_count(fd, file_count - 1);
    save_entries(fd);
    close(fd);
}

void fs_write(const char* filename, const char* data, int size) {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(filename, file_count);
    if (index == -1) {
        close(fd);
        return;
    }
    if (size > BLOCK_SIZE) size = BLOCK_SIZE;  
    lseek(fd, entries[index].start, SEEK_SET);
    write(fd, data, size);
    entries[index].size = size;
    save_entries(fd);
    close(fd);
}

int fs_read(const char* filename, int offset, int size, char* buffer) {
    int fd = open(DISK_NAME, O_RDONLY);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(filename, file_count);
    if (index == -1 || offset + size > entries[index].size) {
        close(fd);
        return -1;  
    }
    lseek(fd, entries[index].start + offset, SEEK_SET);
    int bytes_read = read(fd, buffer, size);
    close(fd);
    return bytes_read;  
}

void fs_ls() {
    int fd = open(DISK_NAME, O_RDONLY);
    int file_count = get_file_count(fd);
    load_entries(fd);
    for (int i = 0; i < file_count; ++i) {
        cout << entries[i].name << " - " << entries[i].size << " bytes" << endl;
    }
    close(fd);
}

void fs_rename(const char* old_name, const char* new_name) {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(old_name, file_count);
    if (index != -1) {
        strncpy(entries[index].name, new_name, FILENAME_SIZE - 1);
        save_entries(fd);
    }
    close(fd);
}

bool fs_exists(const char* filename) {
    int fd = open(DISK_NAME, O_RDONLY);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(filename, file_count);
    close(fd);
    return index != -1;
}

int fs_size(const char* filename) {
    int fd = open(DISK_NAME, O_RDONLY);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(filename, file_count);
    close(fd);
    return index != -1 ? entries[index].size : -1;
}

void fs_append(const char* filename, const char* data, int size) {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(filename, file_count);
    if (index != -1) {
        int new_size = entries[index].size + size;
        if (new_size > BLOCK_SIZE) size = BLOCK_SIZE - entries[index].size;
        if (size <= 0) {
            close(fd);
            return;
        }
        int pos = entries[index].start + entries[index].size;
        lseek(fd, pos, SEEK_SET);
        write(fd, data, size);
        entries[index].size += size;
        save_entries(fd);
    }
    close(fd);
}

void fs_truncate(const char* filename, int new_size) {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int index = find_file(filename, file_count);
    if (index != -1 && new_size < entries[index].size) {
        entries[index].size = new_size;
        save_entries(fd);
    }
    close(fd);
}

void fs_copy(const char* src_filename, const char* dest_filename) {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int src_index = find_file(src_filename, file_count);
    if (src_index == -1 || file_count >= MAX_FILES || find_file(dest_filename, file_count) != -1) {
        close(fd);
        return;
    }
    FileEntry& src = entries[src_index];
    FileEntry& dest = entries[file_count];
    memset(&dest, 0, sizeof(FileEntry));
    strncpy(dest.name, dest_filename, FILENAME_SIZE - 1);
    dest.size = src.size;
    dest.start = METADATA_SIZE + file_count * BLOCK_SIZE;
    dest.created = time(nullptr);
    char* buffer = new char[src.size];
    lseek(fd, src.start, SEEK_SET);
    read(fd, buffer, src.size);
    lseek(fd, dest.start, SEEK_SET);
    write(fd, buffer, src.size);
    delete[] buffer;
    set_file_count(fd, file_count + 1);
    save_entries(fd);
    close(fd);
}

void fs_mv(const char* old_path, const char* new_path) {
    fs_rename(old_path, new_path);
}

void fs_defragment() {
    int fd = open(DISK_NAME, O_RDWR);
    int file_count = get_file_count(fd);
    load_entries(fd);
    int offset = METADATA_SIZE;
    char buffer[BLOCK_SIZE];
    for (int i = 0; i < file_count; ++i) {
        if (entries[i].start != offset) {
            lseek(fd, entries[i].start, SEEK_SET);
            read(fd, buffer, entries[i].size);
            lseek(fd, offset, SEEK_SET);
            write(fd, buffer, entries[i].size);
            entries[i].start = offset;
        }
        offset += ((entries[i].size + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
    }
    save_entries(fd);
    close(fd);
}

void fs_check_integrity() {
    int fd = open(DISK_NAME, O_RDONLY);
    int file_count = get_file_count(fd);
    load_entries(fd);
    for (int i = 0; i < file_count; ++i) {
        if (entries[i].size < 0 || entries[i].start < METADATA_SIZE) {
            cout << "Corrupted file: " << entries[i].name << endl;
        }
    }
    close(fd);
}

void fs_backup(const char* backup_filename) {
    int fd_src = open(DISK_NAME, O_RDONLY);
    if (fd_src == -1) {
        std::cerr << "Failed to open source file" << std::endl;
        return;
    }
    int fd_dst = open(backup_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_dst == -1) {
        close(fd_src);
        std::cerr << "Failed to open backup file" << std::endl;
        return;
    }
    char buffer[4096];
    ssize_t bytes;
    while ((bytes = read(fd_src, buffer, sizeof(buffer))) > 0) {
        write(fd_dst, buffer, bytes);
    }
    close(fd_src);
    close(fd_dst);
}

void fs_restore(const char* backup_filename) {
    int fd_src = open(backup_filename, O_RDONLY);
    if (fd_src == -1) {
        std::cerr << "Failed to open backup file" << std::endl;
        return;
    }
    int fd_dst = open(DISK_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_dst == -1) {
        close(fd_src);
        std::cerr << "Failed to open disk file" << std::endl;
        return;
    }
    char buffer[4096];
    ssize_t bytes;
    while ((bytes = read(fd_src, buffer, sizeof(buffer))) > 0) {
        write(fd_dst, buffer, bytes);
    }
    close(fd_src);
    close(fd_dst);
}

void fs_cat(const char* filename) {
    char buffer[1024];
    int size = fs_size(filename);
    if (size <= 0) return;
    int bytes_read = fs_read(filename, 0, size, buffer);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        cout << buffer << endl;
    }
}

void fs_diff(const char* file1, const char* file2) {
    int size1 = fs_size(file1);
    int size2 = fs_size(file2);
    if (size1 != size2) {
        cout << "Files are different in size." << endl;
        return;
    }
    char* buf1 = new char[size1];
    char* buf2 = new char[size2];
    int bytes_read1 = fs_read(file1, 0, size1, buf1);
    int bytes_read2 = fs_read(file2, 0, size2, buf2);
    if (bytes_read1 > 0 && bytes_read2 > 0 && memcmp(buf1, buf2, size1) == 0)
        cout << "Files are identical." << endl;
    else
        cout << "Files differ." << endl;
    delete[] buf1;
    delete[] buf2;
}

void fs_log(const char* message) {
    int fd = open("fs.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (fd == -1) {
        std::cerr << "Failed to open log file" << std::endl;
        return;
    }
    time_t now = time(nullptr);
    char* time_str = ctime(&now);
    write(fd, time_str, strlen(time_str) - 1);
    write(fd, ": ", 2);
    write(fd, message, strlen(message));
    write(fd, "\n", 1);
    close(fd);
}