#include "ResourceFile.h"

#define RES_MAGIC_NUMBER 0x921A

HeaderInfo *readHeader(std::ifstream &ifs)
{
    HeaderInfo *header = new HeaderInfo;

    ifs.read(reinterpret_cast<char*>(header), sizeof(HeaderInfo));
    std::cout << std::showbase << std::setw(4) << std::hex;
    std::cout   << "Signature: " << header->signature << std::endl
                << "Footer addr: " << header->footerAddr << std::endl;

    if (header->signature != RES_MAGIC_NUMBER)
    {
        std::cerr << "Invalid magic number!" << std::endl;
        return nullptr;
    }

    std::cout << "Magic number is valid!\n\n";
    return header;
}

void readFileInfo(std::ifstream &ifs, FileInfo &fileInfo)
{
    ifs.read(reinterpret_cast<char*>(&fileInfo), sizeof(FileInfo) - sizeof(void *));
    std::cout   << "ext:        \"" << fileInfo.ext << '"' << std::endl
                << "dataAddr:   " << std::hex << fileInfo.dataAddr << std::endl
                << "size:       " << std::dec << fileInfo.size << " bytes\n\n";

    fileInfo.data = malloc(fileInfo.size);

    std::streampos pos = ifs.tellg();
    ifs.seekg(fileInfo.dataAddr);
    // TODO: check for failure

    ifs.read(reinterpret_cast<char *>(fileInfo.data), fileInfo.size);

    if (pos == -1)
    {
        ifs.seekg(0, std::ios::end);
    }
    else
    {
        ifs.seekg(pos);
    }
}

ResourceFile::ResourceFile(const std::string &path)
{
    std::ifstream ifs(path, std::ios::binary);

    header = readHeader(ifs);
    if (header == nullptr)
    {
        return;
    }

    ifs.seekg(header->footerAddr);
//    std::vector<FileInfo> assets;
    int i = 0;
    while (!ifs.eof() && ifs.peek() != EOF) {
        FileInfo file;
        std::cout << "File #" << std::dec << ++i << '\n';
        readFileInfo(ifs, file);
        assets.push_back(file);
    }

    std::cout << std::endl << "Found " << std::dec << assets.size() << " files" << std::endl;

    i = 0;
    for (auto it = assets.begin(); it != assets.end(); ++it)
    {
        ++i;
        if (it->data != nullptr)
        {
            std::ofstream outFile(std::to_string(i) + '.' + std::string(it->ext), std::ios::binary);
            outFile.write(static_cast<char *>(it->data), it->size);
            outFile.close();
            free(it->data);
        }
    }

    ifs.close();
}

const HeaderInfo *ResourceFile::getInfo() const
{
    return header;
}

const std::vector<FileInfo> &ResourceFile::getAssets() const
{
    return assets;
}
