#ifndef RESOURCEFILE_H
#define RESOURCEFILE_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

struct HeaderInfo
{
    uint16_t    signature = 0;
    uint32_t    footerAddr = 0;
} __attribute__((packed));

struct FileInfo
{
    char        ext[4];

    uint8_t     c0; // probably enum
    uint8_t     c1; // probably enum

    uint16_t    dataAddr;

    uint16_t    unk0;
    uint16_t    size;
    uint16_t    unk1;
    uint16_t    unk2;

    void*       data = nullptr;
} __attribute__((packed));

class ResourceFile
{
public:
    ResourceFile(const std::string &path);
    const HeaderInfo *getInfo() const;
    const std::vector<FileInfo> &getAssets() const;

private:
    HeaderInfo *header;
    std::vector<FileInfo> assets;
};

#endif // RESOURCEFILE_H
