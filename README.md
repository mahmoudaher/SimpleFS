# Simple File System Simulator (SimpleFS)

# Project Description
This project simulates a simple file system over a virtual disk (`disk.sim`). 
It provides fundamental file operations using only low-level system calls** such as `open`, `read`, `write`, `lseek`, `close`, 
and others—without relying on any standard file I/O libraries.

The system mimics the structure of a basic file system with a **metadata section (first 4096 bytes)** followed by a **data storage region**. Each file has a name, size, starting address, and creation timestamp stored in the metadata.




# Features

- File creation, deletion, and renaming
- Writing to and reading from files
- Appending data and truncating file contents
- Copying and moving files
- Listing all stored files
- Formatting the virtual disk
- File existence check and size retrieval
- Disk defragmentation
- Metadata and integrity check
- Backup and restore functionality
- Viewing file contents (`cat`)
- Comparing files (`diff`)
- Logging operations


# How to Compile?
with bash - command : make
