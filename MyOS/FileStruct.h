#include <string>
#include "common.h"
#include <map>

using namespace std;

namespace sup
{
    struct FileStruct
    {
    public:
        
    };

    class FileSystem
    {
    public:

        std::map<std::string,uint64_t> _filename_to_id;
        std::map<uint64_t,FileStruct> _id_to_fs;
    };
}
