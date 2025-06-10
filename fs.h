#ifndef FS_H
#define FS_H

int fs_create(const char* filename);  
void fs_delete(const char* filename);
void fs_write(const char* filename, const char* data, int size);
int fs_read(const char* filename, int offset, int size, char* buffer);  
void fs_ls();
void fs_format();
void fs_rename(const char* old_name, const char* new_name);
bool fs_exists(const char* filename);
int fs_size(const char* filename);
void fs_append(const char* filename, const char* data, int size);
void fs_truncate(const char* filename, int new_size);
void fs_copy(const char* src_filename, const char* dest_filename);
void fs_mv(const char* old_path, const char* new_path);
void fs_defragment();
void fs_check_integrity();
void fs_backup(const char* backup_filename);
void fs_restore(const char* backup_filename);
void fs_cat(const char* filename);
void fs_diff(const char* file1, const char* file2);
void fs_log(const char* message);

#endif