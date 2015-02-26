/*
 *	DataImport.cpp
 *	!CHAOS
 *	Created automatically
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <driver/data_import/core/DataImport.h>
#include <driver/data_import/core/AbstractDataImportDriver.h>

#include <json/json.h>

using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(DataImport)

#define DI_CUSTOM_HEAD "[" << getCUID() << " - " << getCUInstance() << " - "
#define DILAPP_	INFO_LOG(DataImport) << DI_CUSTOM_HEAD
#define DILDBG_	DBG_LOG(DataImport) << DI_CUSTOM_HEAD
#define DILERR_	ERR_LOG(DataImport) << DI_CUSTOM_HEAD


#define LOG_AND_THROW(code, msg)\
DILERR_ << msg;\
throw chaos::CException(code, msg, __PRETTY_FUNCTION__);

static const char * const ERROR_MSG_BAD_JSON_PARAMETER = "Error reading json parameter";
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
/*
 Construct
 */
DataImport::DataImport(const string& _control_unit_id,
                       const string& _control_unit_param,
                       const ControlUnitDriverList& _control_unit_drivers):
RTAbstractControlUnit(_control_unit_id,
                      _control_unit_param,
                      _control_unit_drivers),
driver_interface(NULL) {
    driver_interface = new DataImportDriverInterface(getAccessoInstanceByIndex(0));
}

/*
 Destructor
 */
DataImport::~DataImport() {
    //clear all allocated slot
    for(AttrbiuteOffLenVecIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        delete(*it);
    }
    
    if(driver_interface){
        delete(driver_interface);
    }
}

//!
int DataImport::decodeType(const std::string& str_type, DataType::DataType attribute_type) {
    int err = 0;
    if(str_type.compare("int32")) {
        attribute_type = DataType::TYPE_INT32;
    } else if(str_type.compare("uint32")) {
        attribute_type = DataType::TYPE_INT64;
    } else if(str_type.compare("int64")) {
        attribute_type = DataType::TYPE_INT64;
    } else if(str_type.compare("uint64")) {
        attribute_type = DataType::TYPE_INT64;
    } else if(str_type.compare("double")) {
        attribute_type = DataType::TYPE_DOUBLE;
    } else if(str_type.compare("string")) {
        attribute_type = DataType::TYPE_STRING;
    } else if(str_type.compare("binary")) {
        attribute_type = DataType::TYPE_BYTEARRAY;
    } else if(str_type.compare("boolean")) {
        attribute_type = DataType::TYPE_BOOLEAN;
    } else {
        err = -1;
    }
    
    return err;
}

//!Return the definition of the control unit
/*!
 {
 - the dataset of the producer that is an array of json document
 "dataset": [ {
 -the name of the attribute
 "name": string,
 
 -the description fo the attribute
 "description": string,
 
 -the type of the attribute as: (u)int32, (u)int64, double, string, binary, boolean
 "type": string.
 
 -the offset is mandatory and represent the start of the value within a datablock
 "offset": int
 
 -the len is mandatory and represent how much data need to be copied
 "len": int

 }]
 }*/
void DataImport::unitDefineActionAndDataset() throw(chaos::CException) {
    //insert your definition code here
    Json::Value						json_parameter;
    Json::StyledWriter				json_writer;
    Json::Reader					json_reader;
    
    //parse json string
    DEBUG_CODE(DILDBG_ << "Try to parse received json parameter");
    if(!json_reader.parse(getCUParam(), json_parameter)) {
        LOG_AND_THROW(-1, ERROR_MSG_BAD_JSON_PARAMETER)
    }
    
    //fetch value from json document
    DEBUG_CODE(DILDBG_ << "Received JOSN parameter:" << json_writer.write(json_parameter);)
    
    int idx = 0;
    const Json::Value& dataset_description = json_parameter["dataset"];
    for (Json::ValueConstIterator it = dataset_description.begin();
         it != dataset_description.end();
         ++it) {
        if(!it->isObject()) {
            LOG_AND_THROW(-2, ERROR_MSG_BAD_JSON_DATASET_ATTRIBUTE)
            
        }
        DILAPP_ << "Configuring dataset attribute:" << json_writer.write(*it);
        
        const Json::Value& json_attribute_name = (*it)["name"];
        const Json::Value& json_attribute_description = (*it)["description"];
        const Json::Value& json_attribute_type = (*it)["type"];
        const Json::Value& json_attribute_offset = (*it)["offset"];
        const Json::Value& json_attribute_len = (*it)["len"];
        
        
        if(json_attribute_name.isNull()) {
            LOG_AND_THROW(-3, ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_NAME)
        }
        if(!json_attribute_name.isString()) {
            LOG_AND_THROW(-4, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_NAME)
        }
        if(json_attribute_description.isNull()) {
            LOG_AND_THROW(-5, ERROR_MSG_MANDATORY_JSON_DATASET_ATTRIBUTE_DESC)
        }
        if(!json_attribute_description.isString()) {
            LOG_AND_THROW(-6, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_DESC)
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
        
        DataType::DataType attribute_type = DataType::TYPE_INT32;
        if(decodeType(json_attribute_type.asString(), attribute_type)) {
            LOG_AND_THROW(-9, ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_TYPE)
        }
        
        //add the attribute and in case it is string or binary we need to check
        //the max size attribute
        switch(attribute_type) {
            case DataType::TYPE_INT32:
            case DataType::TYPE_INT64:
            case DataType::TYPE_DOUBLE:
            case DataType::TYPE_BOOLEAN:
                addAttributeToDataSet(json_attribute_name.asString().c_str(),
                                      json_attribute_description.asString().c_str(),
                                      (DataType::DataType)attribute_type,
                                      DataType::Output);
                break;
                
                
            case DataType::TYPE_STRING:
            case DataType::TYPE_BYTEARRAY:
                addAttributeToDataSet(json_attribute_name.asString().c_str(),
                                      json_attribute_description.asString().c_str(),
                                      (DataType::DataType)attribute_type,
                                      DataType::Output,
                                      json_attribute_len.asInt());
                
                break;
        }
        
        //add the attribute slot intto the vector
        AttributeOffLen *vec = new AttributeOffLen();
        vec->index = idx++;
        vec->name = json_attribute_name.asString();
        vec->offset = json_attribute_offset.asInt();
        vec->len = json_attribute_len.asInt();
        attribute_off_len_vec.push_back(vec);
    }
}


//!Define custom control unit attribute
void DataImport::unitDefineCustomAttribute() {
    
}

//!Initialize the Custom Control Unit
void DataImport::unitInit() throw(chaos::CException) {
    //check the value set on MDS for in_1 channel
    for(AttrbiuteOffLenVecIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //get hte base address of the value form the cache
        DILAPP_ "Get pointer forom cache for attribute " << (*it)->name;
        (*it)->buffer = getAttributeCache()->getRWPtr<void*>(chaos::common::data::cache::DOMAIN_OUTPUT, (*it)->name);
        if((*it)->buffer == NULL) {
            throw chaos::CException(-1, "Error retriving pointer", __PRETTY_FUNCTION__);
        }
    }
}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void DataImport::unitStart() throw(chaos::CException) {
    
}

//!Execute the Control Unit work
void DataImport::unitRun() throw(chaos::CException) {
    int err = 0;
    //fetch new datablock
    if((err = driver_interface->fetchNewDatablock())) {
       DILERR_ << "Error fetching new datablock with error" << err;
    }

    //fetch all single attribute f)rom datablock
    for(AttrbiuteOffLenVecIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //
        if((err = driver_interface->readAttribute((*it)->buffer, (*it)->offset, (*it)->len))) {
            DILERR_ << "Error reading attribute " << (*it)->name << " from driver with error " << err;
        }
    }
    
    //! set output dataset as changed
    getAttributeCache()->setOutputDomainAsChanged();
}

//!Execute the Control Unit work
void DataImport::unitStop() throw(chaos::CException) {
    
}

//!Deinit the Control Unit
void DataImport::unitDeinit() throw(chaos::CException) {
    
}

//! pre imput attribute change
void DataImport::unitInputAttributePreChangeHandler() throw(chaos::CException) {
    
}

//! attribute changed handler
void DataImport::unitInputAttributeChangedHandler() throw(chaos::CException) {
}
