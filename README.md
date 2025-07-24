# UnixFileSystem

A command-line based simulation of a Unix-like file system (EXT2/EXT3) implemented in C/C++. This project was developed for the "File Handling and Implementation" course and focuses on disk and file system management through direct binary manipulation.

## Features

- Virtual disk creation and deletion (binary files as disks)
- Partition management: primary, extended, and logical partitions (using MBR and EBR)
- EXT2/EXT3 filesystem formatting and journaling support
- Session-based file system with user/group authentication
- File and directory operations (create, delete, edit, copy, move)
- Permissions system for users, groups, and others (similar to Unix)
- Full command interpreter (no GUI)
- Graphviz-based reporting (MBR, inodes, blocks, tree, etc.)
- Filesystem loss simulation and recovery

## File System Architecture

Implements core EXT2/EXT3 structures:

- **MBR** (Master Boot Record)
- **Partition table** with extended and logical partitions
- **Superblock**, bitmaps, inode tables
- **Directory entries**, data blocks, and block pointers (direct/indirect)
- **User/group management** via `users.txt` in the root directory
- **Permissions** encoded as 9-bit fields for U-G-O (user-group-other)

## Commands

The following command set is implemented and parsed from the CLI:

### Disk and Partition Management
- `mkdisk`: Create a virtual disk
- `rmdisk`: Remove a virtual disk
- `fdisk`: Create/delete/resize partitions
- `mount` / `unmount`: Mount or unmount partitions in memory

### File System Formatting
- `mkfs`: Format partitions as EXT2/EXT3

### User and Group Management
- `login` / `logout`
- `mkgrp` / `rmgrp`
- `mkusr` / `rmusr`

### File and Directory Operations
- `mkdir`: Create directory (with optional recursive flag)
- `mkfile`: Create file (with content or size)
- `cat`: Display file contents
- `edit`: Edit file with new content
- `rem`: Remove files or directories recursively
- `ren`: Rename files or directories
- `cp`: Copy files or directories
- `mv`: Move files or directories
- `find`: Wildcard search (`*`, `?`)
- `chmod`: Change permissions
- `chown`: Change file ownership
- `chgrp`: Change group ownership

### File System Simulation
- `pause`: Wait for key input
- `loss`: Simulate file system failure (wipes metadata)
- `recovery`: Restore metadata using journaling and superblock

### Scripting and Reports
- `exec`: Run script files containing commands
- `rep`: Generate Graphviz reports for:
  - MBR / EBR
  - Superblock
  - Inodes and blocks
  - Bitmaps
  - Directory tree
  - File content (`file`)
  - `ls`-like listings

## Technical Constraints

- Implemented in C/C++ only
- No dynamic data structures in memory (no linked lists, trees, etc.)
- Filesystem must be managed entirely via disk structures
- Must compile and run on GNU/Linux
- No code modification allowed during evaluation

## Example Usage

```bash
mkdisk -size=10 -u=M -path="/home/Disco.dk"
fdisk -size=1024 -path="/home/Disco.dk" -name=Part1
mount -path="/home/Disco.dk" -name=Part1
mkfs -id=vd1 -type=full -fs=3fs
login -usr=root -pwd=123 -id=vd1
mkgrp -name=developers
mkusr -usr=alice -pwd=123 -grp=developers
mkdir -path="/home/alice/docs" -p
mkfile -path="/home/alice/docs/report.txt" -size=200
````

## Reports (Graphviz)

* Generated reports include detailed visualizations of:

  * MBR, partitions and logical layout
  * File system trees
  * Inode/block allocation
  * Bitmap states

## License

This project was developed as part of an academic assignment and is intended for educational purposes.
