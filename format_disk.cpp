#include <iostream>
#include <fstream>
using namespace std;

int main() {
    const char* disk_name = "disk.sim";
    const size_t disk_size = 1024 * 1024;
    ofstream disk(disk_name, ios::binary);
    if (!disk.is_open()) {
        cerr << "Failed to create disk file." << endl;
        return 1;
    }
    char buffer[4096] = {0};
    for (size_t i = 0; i < disk_size; i += sizeof(buffer)) {
        disk.write(buffer, sizeof(buffer));
    }
    disk.close();
    cout << "Disk formatted successfully." << endl;
    return 0;
}
