#ifndef __ATTRIBUTE_OFF_LEN_H__
#define __ATTRIBUTE_OFF_LEN_H__
#include <vector>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>
#include <json/json.h>

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
    AttributeOffLen():buffer(NULL),old_buffer(NULL),len(0),offset(0){};
};

typedef std::vector<AttributeOffLen *> AttributeOffLenVec;
typedef std::vector<AttributeOffLen *>::iterator AttributeOffLenIterator;
typedef std::vector<AttributeOffLen *>::const_iterator AttributeOffLenCIterator;

// must contain a dataset key
AttributeOffLenVec json2Attribute(const Json::Value&);

int copy(void*ptr,const AttributeOffLen*v);
AttributeOffLenVec json2Attribute(const std::string &jsonin);
chaos::common::data::CDWUniquePtr attribute2CDW(const AttributeOffLenVec& attrs );
int decodeType(const std::string &str_type, chaos::DataType::DataType &attribute_type);

} // namespace data_import
} // namespace driver
#endif