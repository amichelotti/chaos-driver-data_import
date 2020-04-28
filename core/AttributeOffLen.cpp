#include "AttributeOffLen.h"
#include <chaos/common/global.h> 
static const char * const ERROR_MSG_BAD_JSON_PARAMETER = "Error parsing JSON";
static const char * const ERROR_MSG_BAD_JSON_DATASET_ATTRIBUTE = "The attribute description within the dataset attribute need to be an object";
static const char * const ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_NAME = "The attribute name is mandatory";
static const char * const ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_NAME = "The attribute name need to be a string";
static const char * const ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_DESC = "The attribute description is mandatory";
static const char * const ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_DESC = "The attribute description need to be a string";
static const char * const ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_TYPE = "The attribute type is mandatory";
static const char * const ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_TYPE = "The attribute type need to be a string of value[int32, int64, double, string, binary, boolean]";
static const char * const ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_OFFSET = "The attribute offset is mandatory";
static const char * const ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_OFFSET = "The attribute offset need to be an integer";
static const char * const ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_LENGHT = "The attribute lenght is mandatory";
static const char * const ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_LENGHT = "The attribute lenght need to be a n integer";
static const char * const ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_LBE = "The attribute lbe need to be a boolean";
using namespace chaos;


#define DILAPP_	INFO_LOG(AttributeOffLen) 
#define DILDBG_	DBG_LOG(AttributeOffLen) 
#define DILERR_	ERR_LOG(AttributeOffLen) 

#define LOG_AND_THROW(code, msg)\
DILERR_ << msg;\
throw chaos::CException(code, msg, __PRETTY_FUNCTION__);


namespace driver{
    namespace data_import{

        int decodeType(const std::string& str_type, chaos::DataType::DataType& attribute_type) {
    int err = 0;
    if(str_type.compare("int32")==0) {
        attribute_type = DataType::TYPE_INT32;
    } else if(str_type.compare("uint32")==0) {
        attribute_type = DataType::TYPE_INT64;
    } else if(str_type.compare("int64")==0) {
        attribute_type = DataType::TYPE_INT64;
    } else if(str_type.compare("uint64")==0) {
        attribute_type = DataType::TYPE_INT64;
    } else if(str_type.compare("double")==0) {
        attribute_type = DataType::TYPE_DOUBLE;
    } else if(str_type.compare("string")==0) {
        attribute_type = DataType::TYPE_STRING;
    } else if(str_type.compare("binary")==0) {
        attribute_type = DataType::TYPE_BYTEARRAY;
    } else if((str_type.compare("boolean")==0)||(str_type.compare("bool")==0) ) {
        attribute_type = DataType::TYPE_BOOLEAN;
    } else {
        err = -1;
    }
    
    return err;
}

AttributeOffLenVec json2Attribute(const std::string&jsonin){
    Json::Reader					json_reader;
    Json::Value						json_parameter;

     if(!json_reader.parse(jsonin, json_parameter)) {
        std::stringstream ss;
        ss<<ERROR_MSG_BAD_JSON_PARAMETER<<": \""<<jsonin<<"\"";
        LOG_AND_THROW(-1, ss.str());
    }
    return json2Attribute(json_parameter);
}
AttributeOffLenVec json2Attribute(const Json::Value& json_parameter){
    AttributeOffLenVec attribute_off_len_vec;
    int idx=0;
    std::string defkeybind="";
   
    const Json::Value& dataset_description = json_parameter["dataset"];
    if(dataset_description.isNull()){
        LOG_AND_THROW(-1, "a 'dataset' key must be provided")
    }

    const Json::Value& def_keybind = json_parameter["keybind"];
    if(!def_keybind.isNull()&&(def_keybind.isString())){
        defkeybind=def_keybind.asString();
    }

    for (Json::ValueConstIterator it = dataset_description.begin();
         it != dataset_description.end();
         ++it) {
        if(!it->isObject()) {
            LOG_AND_THROW(-2, ERROR_MSG_BAD_JSON_DATASET_ATTRIBUTE)
            
        }
        
        const Json::Value& json_attribute_name = (*it)["name"];
        const Json::Value& json_attribute_description = (*it)["description"];
        const Json::Value& json_attribute_type = (*it)["type"];
        const Json::Value& json_attribute_offset = (*it)["offset"];
        const Json::Value& json_attribute_len = (*it)["len"];
        const Json::Value& json_attribute_lbe = (*it)["lbe"];
        const Json::Value& json_attribute_factor = (*it)["factor"];
        const Json::Value& key_bind = (*it)["keybind"];
        std::string desc;
        if(json_attribute_name.isNull()) {
            LOG_AND_THROW(-3, ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_NAME)
        }
        if(!json_attribute_name.isString()) {
            LOG_AND_THROW(-4, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_NAME)
        }
        if((!json_attribute_description.isNull())&&json_attribute_description.isString()) {
            desc=json_attribute_description.asString();
        }
        if(json_attribute_type.isNull()) {
            LOG_AND_THROW(-7, ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_TYPE)
        }
        if(!json_attribute_type.isString()) {
            LOG_AND_THROW(-8, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_TYPE)
        }
        if(json_attribute_offset.isNull()) {
            LOG_AND_THROW(-9, ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_OFFSET)
        }
        if(!json_attribute_offset.isInt()) {
            LOG_AND_THROW(-10, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_OFFSET)
        }
        if(json_attribute_len.isNull()) {
            LOG_AND_THROW(-9, ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_LENGHT)
        }
        if(!json_attribute_len.isInt()) {
            LOG_AND_THROW(-10, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_LENGHT)
        }
        if(!json_attribute_lbe.isNull() && !json_attribute_lbe.isBool()) {
            LOG_AND_THROW(-11, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_LBE)
        }
        
        
        chaos::DataType::DataType attribute_type = DataType::TYPE_INT32;
        if(decodeType(json_attribute_type.asString(), attribute_type)) {
            LOG_AND_THROW(-12, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_TYPE)
        }
        
        //add the attribute and in case it is string or binary we need to check
        //the max size attribute
        AttributeOffLen *vec = new AttributeOffLen();
        vec->len = json_attribute_len.asInt();
        vec->desc=desc;
        switch(attribute_type) {
            case DataType::TYPE_INT32:
                vec->len=sizeof(int32_t);
                break;
            case DataType::TYPE_INT64:
                vec->len=sizeof(int64_t);
                break;
            case DataType::TYPE_DOUBLE:
                vec->len=sizeof(double);
                break;
            case DataType::TYPE_BOOLEAN:
                vec->len=sizeof(bool);
                break;
        };
       
        //add the attribute slot intto the vector
        vec->index = idx++;
        vec->name = json_attribute_name.asString();
        vec->type = attribute_type;
        vec->offset = json_attribute_offset.asInt();
        vec->len = json_attribute_len.asInt();
         if(json_attribute_factor.isNull() || !json_attribute_factor.isDouble()){
            vec->factor=0.0;
        } else {
            vec->factor=json_attribute_factor.asDouble();
        }
        vec->keybind=defkeybind;

        if(!key_bind.isNull() && key_bind.isString()){
            vec->keybind=key_bind.asString();
        }

        if(json_attribute_lbe.isNull()) {
            vec->lbe = false;
        }else {
            vec->lbe = (int)json_attribute_lbe.asBool();
        }
        vec->old_buffer=malloc(vec->len);
        DILDBG_<<"Attribute "<< vec->name<<"["<<vec->keybind<<"]"<<" +"<<vec->offset<<" type:"<<vec->type<<" len:"<<vec->len<<" factor:"<<vec->factor<<" LBE:"<<vec->lbe;
        attribute_off_len_vec.push_back(vec);
    }
    return attribute_off_len_vec;
}
    }
    }
