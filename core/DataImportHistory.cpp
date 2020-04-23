/*
 *	DataImportHistory.cpp
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

#include <driver/data-import/core/DataImportHistory.h>
#include <driver/data-import/core/AbstractDataImportDriver.h>

#include <chaos/common/utility/endianess.h>
#include <driver/data-import/core/AttributeOffLen.h>
#include <json/json.h>

using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(DataImportHistory)

#define DI_CUSTOM_HEAD "[" << getCUID() << " - " << getCUInstance() << "] - "
#define DILAPP_	INFO_LOG(DataImportHistory) << DI_CUSTOM_HEAD
#define DILDBG_	DBG_LOG(DataImportHistory) << DI_CUSTOM_HEAD
#define DILERR_	ERR_LOG(DataImportHistory) << DI_CUSTOM_HEAD


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
static const char * const ERROR_MSG_TYPE_JSON_DATASET_ATTRIBUTE_LBE = "The attribute lbe need to be a boolean";
/*
 Construct
 */
DataImportHistory::DataImportHistory(const string& _control_unit_id,
                       const string& _control_unit_param,
                       const ControlUnitDriverList& _control_unit_drivers):DataImport(_control_unit_id,
                              _control_unit_param,
                              _control_unit_drivers){
	 history=NULL;
	    driver_interface=NULL;
	    db = NULL;
}

/*
 Destructor
 */
DataImportHistory::~DataImportHistory() {


}


void DataImportHistory::unitDefineActionAndDataset() throw(chaos::CException) {
	DataImport::unitDefineActionAndDataset();

	 //insert your definition code here

}


//!Define custom control unit attribute
void DataImportHistory::unitDefineCustomAttribute() {
}

//!Initialize the Custom Control Unit
void DataImportHistory::unitInit() throw(chaos::CException) {
	DataImport::unitInit();
	std::string ttype=std::string("DataImportHistory_")+getCUID();

	 Json::Value						json_parameter;
	    Json::StyledWriter				json_writer;
	    Json::Reader					json_reader;

	    //parse json string
	    DEBUG_CODE(DILDBG_ << "Try to parse received json parameter");
	    if(!json_reader.parse(getCUParam(), json_parameter)) {
	        LOG_AND_THROW(-1, ERROR_MSG_BAD_JSON_PARAMETER)
	    }

	    //fetch value from json document

	    int idx = 0;
	    const Json::Value& db_description = json_parameter["db"];
	    if(db_description.isNull()){
	    	ERR("db decription not present");
	    	throw chaos::CException(-10,"db decription not present",__PRETTY_FUNCTION__);
	    }
	    const Json::Value& dbtype=db_description["dbtype"];
	    const Json::Value& dbname=db_description["dbname"];
	    const Json::Value& dbreplication=db_description["dbreplication"];
	    const Json::Value& dbserver=db_description["dbserver"];

	    if(dbtype.isNull()){
	  	    	ERR("dbtype decription not present");
	  	    	throw chaos::CException(-11,"dbtype description not present",__PRETTY_FUNCTION__);
	  	 }
	    if(dbname.isNull()){
	    	  	    	ERR("dbname decription not present");
	    	  	    	throw chaos::CException(-12,"dbname description not present",__PRETTY_FUNCTION__);
	    }

	    if(dbserver.isNull()){
	   	    	  	    	ERR("dbserver decription not present");
	   	    	  	    	throw chaos::CException(-13,"dbserver description not present",__PRETTY_FUNCTION__);
	   	 }

	    db = ::common::misc::data::DBbaseFactory::getInstance(dbtype.asCString(),dbname.asCString());
	    if(db == NULL){
	    	ERR("cannot instanciate type:%s name:%s",dbtype.asCString(),dbname.asCString());
	    	throw chaos::CException(-13,"cannot instaciate DV",__PRETTY_FUNCTION__);
	    }
    history = new ::common::misc::data::DataSet(ttype,getCUID());
    db->addDBServer(dbserver.asCString());
    if(!dbreplication.isNull()){
    	db->setDBParameters("replication",dbreplication.asCString());
    }


    
    //check the value set on MDS for in_1 channel
    for(::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //get hte base address of the value form the cache
        DILAPP_ "Get pointer from cache for attribute " << (*it)->name;

        (*it)->buffer = getAttributeCache()->getRWPtr<void*>(chaos::common::data::cache::DOMAIN_OUTPUT, (*it)->name);

        if((*it)->buffer != NULL) {
	  history->add((*it)->name,(*it)->type, (char*)(*it)->buffer,(*it)->len);


        } else {
        	throw chaos::CException(-1, "Error retriving pointer", __PRETTY_FUNCTION__);
        }
    }
    if(db->connect()!=0){
    	throw chaos::CException(-100, "Cannot connect to DB server", __PRETTY_FUNCTION__);


    }
}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void DataImportHistory::unitStart() throw(chaos::CException) {
    
}

//!Execute the Control Unit work
void DataImportHistory::unitRun() throw(chaos::CException) {
    int err = 0;
    //fetch new datablock
    if((err = driver_interface->fetchNewDatablock())) {
       DILERR_ << "fetching NO datablock return:" << err;
       return;
    }


    //fetch all single attribute f)rom datablock
    for(::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //
        if((err = driver_interface->readAttribute((*it)->buffer, (*it)->offset, (*it)->len))) {
            DILERR_ << "Error reading attribute " << (*it)->name << " from driver with error " << err;
        }else if((*it)->lbe>=0){
            //apply some filter if we need it
            switch((*it)->type) {
                case DataType::TYPE_INT32:
                    if((*it)->lbe){
                        *((int32_t*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::big_endian, int32_t>(*((int32_t*)(*it)->buffer));
                    }else{
                        *((int32_t*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::little_endian, int32_t>(*((int32_t*)(*it)->buffer));
                    }
                case DataType::TYPE_INT64:
                    if((*it)->lbe){
                        *((int64_t*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::big_endian, int64_t>(*((int64_t*)(*it)->buffer));
                    }else{
                        *((int64_t*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::little_endian, int64_t>(*((int64_t*)(*it)->buffer));
                    }
                case DataType::TYPE_DOUBLE:
                    if((*it)->lbe){
                        *((double*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::big_endian, double>(*((double*)(*it)->buffer));
                    }else{
                        *((double*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::little_endian, double>(*((double*)(*it)->buffer));
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
    db->pushData(*history);
    //! set output dataset as changed
    getAttributeCache()->setOutputDomainAsChanged();
}

//!Execute the Control Unit work
void DataImportHistory::unitStop() throw(chaos::CException) {
    
}

//!Deinit the Control Unit
void DataImportHistory::unitDeinit() throw(chaos::CException) {
    if(driver_interface) delete(driver_interface);
    if(history) delete(history);
    if(db){
    	db->disconnect();
    }
    history=NULL;
    driver_interface=NULL;
}

//! pre imput attribute change
void DataImportHistory::unitInputAttributePreChangeHandler() throw(chaos::CException) {
    
}

//! attribute changed handler
void DataImportHistory::unitInputAttributeChangedHandler() throw(chaos::CException) {
}
