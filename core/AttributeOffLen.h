#ifndef __ATTRIBUTE_OFF_LEN_H__
#define __ATTRIBUTE_OFF_LEN_H__
#include <vector>
#include <chaos/common/chaos_constants.h>
namespace driver
{

namespace data_import
{
struct AttributeOffLen
{
    uint32_t index;
    std::string name;
    std::string keybind;
    std::string desc;
    chaos::DataType::DataType type;
    uint32_t offset;
    uint32_t len;
    double factor; //conversion factor if any
    int lbe;       //-1 no lbe, 0-little, 1-big
    void *buffer;
    void *old_buffer;
};

typedef std::vector<AttributeOffLen *> AttributeOffLenVec;
typedef std::vector<AttributeOffLen *>::iterator AttributeOffLenIterator;

AttributeOffLenVec json2Attribute(const std::string &jsonin);

int decodeType(const std::string &str_type, chaos::DataType::DataType &attribute_type);

} // namespace data_import
} // namespace driver
#endif