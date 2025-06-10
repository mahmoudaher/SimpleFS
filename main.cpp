#include <iostream>
#include <string>
#include <cstring>
#include "fs.h"
using namespace std;

int main() {
    int choice;
    string filename, newname;
    char buffer[1024];
    int offset, size;

    while (true) {
        cout << "\n---------------- File System ----------------\n";
        cout << "1. Create File\n";
        cout << "2. Delete File\n";
        cout << "3. Write to File\n";
        cout << "4. Read from File\n";
        cout << "5. List Files\n";
        cout << "6. Format Disk\n";
        cout << "7. Rename File\n";
        cout << "8. Check File Existence\n";
        cout << "9. Get File Size\n";
        cout << "10. Append to File\n";
        cout << "11. Truncate File\n";
        cout << "12. Copy File\n";
        cout << "13. Move File\n";
        cout << "14. Defragment Disk\n";
        cout << "15. Check Integrity\n";
        cout << "16. Backup Disk\n";
        cout << "17. Restore Disk\n";
        cout << "18. Display File Content (cat)\n";
        cout << "19. Compare Files (diff)\n";
        cout << "20. Operation Log\n";
        cout << "21. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        system("cls");

        switch (choice) {
            case 1: {
                cout << "Enter file name: ";
                cin >> filename;
                int result = fs_create(filename.c_str());
                if (result == 0) {
                    cout << "File created successfully." << endl;
                } else if (result == -1) {
                    cout << "Error: File already exists." << endl;
                } else if (result == -2) {
                    cout << "Error: Maximum number of files reached." << endl;
                }
                break;
            }
            case 2: {
                cout << "Enter file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    fs_delete(filename.c_str());
                    cout << "File deleted." << endl;
                }
                break;
            }
            case 3: {
                cout << "Enter file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    cout << "Enter data: ";
                    cin.ignore();
                    cin.getline(buffer, sizeof(buffer));
                    fs_write(filename.c_str(), buffer, strlen(buffer));
                    cout << "Data written to file." << endl;
                }
                break;
            }
            case 4: {
                cout << "Enter file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    cout << "Enter offset: ";
                    cin >> offset;
                    cout << "Enter size: ";
                    cin >> size;
                    if (size > 1024) size = 1024;
                    int bytes_read = fs_read(filename.c_str(), offset, size, buffer);
                    if (bytes_read >= 0) {
                        buffer[bytes_read] = '\0';
                        cout << "Data: " << buffer << endl;
                    } else {
                        cout << "Error: Invalid offset or size." << endl;
                    }
                }
                break;
            }
            case 5: {
                fs_ls();
                break;
            }
            case 6: {
                fs_format();
                cout << "Disk formatted." << endl;
                break;
            }
            case 7: {
                cout << "Enter old file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    cout << "Enter new file name: ";
                    cin >> newname;
                    fs_rename(filename.c_str(), newname.c_str());
                    cout << "File renamed." << endl;
                }
                break;
            }
            case 8: {
                cout << "Enter file name: ";
                cin >> filename;
                cout << (fs_exists(filename.c_str()) ? "Exists" : "Does not exist") << endl;
                break;
            }
            case 9: {
                cout << "Enter file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    int file_size = fs_size(filename.c_str());
                    cout << "Size: " << file_size << " bytes" << endl;
                }
                break;
            }
            case 10: {
                cout << "Enter file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    cout << "Enter data: ";
                    cin.ignore();
                    cin.getline(buffer, sizeof(buffer));
                    fs_append(filename.c_str(), buffer, strlen(buffer));
                    cout << "Data appended to file." << endl;
                }
                break;
            }
            case 11: {
                cout << "Enter file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    cout << "Enter new size: ";
                    cin >> size;
                    fs_truncate(filename.c_str(), size);
                    cout << "File truncated." << endl;
                }
                break;
            }
            case 12: {
                cout << "Enter source file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    cout << "Enter destination file name: ";
                    cin >> newname;
                    fs_copy(filename.c_str(), newname.c_str());
                    cout << "File copied." << endl;
                }
                break;
            }
            case 13: {
                cout << "Enter old path: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    cout << "Enter new path: ";
                    cin >> newname;
                    fs_mv(filename.c_str(), newname.c_str());
                    cout << "File moved/renamed." << endl;
                }
                break;
            }
            case 14: {
                fs_defragment();
                cout << "Disk defragmented." << endl;
                break;
            }
            case 15: {
                fs_check_integrity();
                break;
            }
            case 16: {
                cout << "Enter backup file name: ";
                cin >> filename;
                fs_backup(filename.c_str());
                cout << "Backup created (if successful)." << endl;
                break;
            }
            case 17: {
                cout << "Enter backup file name: ";
                cin >> filename;
                fs_restore(filename.c_str());
                cout << "Disk restored (if backup exists)." << endl;
                break;
            }
            case 18: {
                cout << "Enter file name: ";
                cin >> filename;
                if (!fs_exists(filename.c_str())) {
                    cout << "Error: File does not exist." << endl;
                } else {
                    fs_cat(filename.c_str());
                }
                break;
            }
            case 19: {
                cout << "Enter first file name: ";
                cin >> filename;
                cout << "Enter second file name: ";
                cin >> newname;
                if (!fs_exists(filename.c_str()) || !fs_exists(newname.c_str())) {
                    cout << "Error: One or both files do not exist." << endl;
                } else {
                    fs_diff(filename.c_str(), newname.c_str());
                }
                break;
            }
            case 20: {
                cout << "Enter log message: ";
                cin.ignore();
                cin.getline(buffer, sizeof(buffer));
                fs_log(buffer);
                cout << "Log entry added." << endl;
                break;
            }
            case 21: {
                return 0;
            }
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }
}
