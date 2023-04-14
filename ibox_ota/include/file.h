#ifndef _FILE_H_
#define _FILE_H_

#include <string>
using std::string;

namespace ota_file {
static std::string getDirectory(std::string filename) {
  const size_t last_slash_idx = filename.rfind('/');
  if (std::string::npos != last_slash_idx) {
    return filename.substr(0, last_slash_idx);
  }
  return "";
}

static std::string getFileName(std::string filename) {
  const size_t last_slash_idx = filename.rfind('/');
  if (std::string::npos != last_slash_idx) {
    return filename.substr(last_slash_idx + 1, last_slash_idx);
  }
  return filename;
}

static std::string getTarNamePre(std::string packageName) {
  std::string strPreName{""};
  std::string strFileName = ota_file::getFileName(packageName);
  uint32_t nPos = strFileName.find(".tar.gz");
  if (-1 != nPos) {
    strPreName = strFileName.substr(0, nPos);
  }
  return strPreName;
}
}  // namespace ota_file

#endif