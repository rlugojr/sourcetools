#ifndef PARSR_READER_H
#define PARSR_READER_H

#include <fstream>
#include <string>
#include <cerrno>

namespace parsr {

inline bool read(const std::string& absolutePath, std::string* pContent)
{
  std::ifstream ifs(absolutePath, std::ios::in | std::ios::binary);
  if (ifs) {
    ifs.seekg(0, std::ios::end);
    pContent->resize(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    ifs.read(const_cast<char*>(pContent->c_str()), pContent->size());
    ifs.close();
    return true;
  }
  return false;
}

}  // namespace parsr

#endif
