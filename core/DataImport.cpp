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

#include <driver/data-import/core/DataImport.h>
#include <driver/data-import/core/AbstractDataImportDriver.h>
#include <chaos/cu_toolkit/control_manager/ControlUnitTypes.h>
#include <chaos/common/utility/endianess.h>

#include <json/json.h>

using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager;
using namespace driver::data_import;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(DataImport)

#define DI_CUSTOM_HEAD "[" << getCUID()<< "] - "
#define DILAPP_	INFO_LOG(DataImport) << DI_CUSTOM_HEAD
#define DILDBG_	DBG_LOG(DataImport) << DI_CUSTOM_HEAD
#define DILERR_	ERR_LOG(DataImport) << DI_CUSTOM_HEAD


#define LOG_AND_THROW(code, msg)\
DILERR_ << msg;\
throw chaos::CException(code, msg, __PRETTY_FUNCTION__);

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
        attribute_off_len_vec=driver::data_import::json2Attribute(_control_unit_param);

}

/*
 Destructor
 */
DataImport::~DataImport() {
    //clear all allocated slot
    for(driver::data_import::AttributeOffLenIterator i =attribute_off_len_vec.begin();i!=attribute_off_len_vec.end();i++){
            delete(*i);

    }
}

//!

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
 
 -optional for the numeric type, if it is set it will be used to convert to the
 -hendian of the machine where run the control unit
 "lbe":true(little)-false(big)
 }]
 }*/
void DataImport::unitDefineActionAndDataset() throw(chaos::CException) {
    //insert your definition code here
    Json::Value						json_parameter;
    Json::StyledWriter				json_writer;
    Json::Reader					json_reader;
    addStateVariable(StateVariableTypeAlarmCU, "fetching_key","Error fetching key");
    addStateVariable(StateVariableTypeAlarmCU, "fetching_data_block","Error fetching data block");

    //parse json string
    DEBUG_CODE(DILDBG_ << "Try to parse received json parameter");
    if(!json_reader.parse(getCUParam(), json_parameter)) {
        DILERR_<<" RECEIVED:"<<getCUParam();
        LOG_AND_THROW(-1, "Bad JSON")
    }
    
    //fetch value from json document
    DEBUG_CODE(DILDBG_ << "Received JSON parameter:" << json_writer.write(json_parameter);)
    
    int idx = 0;
    for(driver::data_import::AttributeOffLenIterator i =attribute_off_len_vec.begin();i!=attribute_off_len_vec.end();i++){
            switch((*i)->type) {
            case DataType::TYPE_INT32:
            case DataType::TYPE_INT64:
            case DataType::TYPE_DOUBLE:
            case DataType::TYPE_BOOLEAN:
                addAttributeToDataSet((*i)->name,
                                      (*i)->desc,
                                      (*i)->type,
                                      DataType::Output);
                break;
                
                
            default:
                addAttributeToDataSet((*i)->name,
                                      (*i)->desc,
                                      (*i)->type,
                                      DataType::Output,
                                      (*i)->len);
                
                break;
        }
        
    }
    
}


//!Define custom control unit attribute
void DataImport::unitDefineCustomAttribute() {
}

//!Initialize the Custom Control Unit
void DataImport::unitInit() throw(chaos::CException) {
    
    driver_interface = new DataImportDriverInterface(getAccessoInstanceByIndex(0));
    
    //check the value set on MDS for in_1 channel
    for(::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //get hte base address of the value form the cache
        DILAPP_ "Get pointer from cache for attribute " << (*it)->name;
        (*it)->buffer = getAttributeCache()->getRWPtr<void*>(chaos::common::data::cache::DOMAIN_OUTPUT, (*it)->name);
        if((*it)->buffer == NULL) {
            throw chaos::CException(-1, "Error retrieving pointer", __PRETTY_FUNCTION__);
        }
        
    }
}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void DataImport::unitStart() throw(chaos::CException) {
    
}

//!Execute the Control Unit work
void DataImport::unitRun() throw(chaos::CException) {
    int err = 0;
    bool changed=false;
    //fetch new datablock
    DILDBG_<<" Fetch from driver";
	setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_key", chaos::common::alarm::MultiSeverityAlarmLevelClear);
	setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_data_block", chaos::common::alarm::MultiSeverityAlarmLevelClear);

    if((err = driver_interface->fetchNewDatablock())) {
      if(err==  DATA_IMPORT_NO_CHANGE){
	    DILDBG_<<" No Data Change";
       // setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_data_block", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

	    return;
      }
       DILERR_ << "fetching NO datablock return:" << err;
       	setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_data_block", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

       return;
    }


    //fetch all single attribute f)rom datablock
    for(::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //
        if((err = driver_interface->readAttribute((*it)->buffer, (*it)->keybind,(*it)->offset, (*it)->len))) {
            DILERR_ << "Error reading attribute " << (*it)->name << " from key:"<<(*it)->keybind<<" driver with error " << err;
            setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_key", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
            metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError, boost::str(boost::format("Error fetching key '%1%") %  (*it)->name ));

        }else if((*it)->lbe>=0){
             if(memcmp((*it)->buffer,(*it)->old_buffer,(*it)->len)){
                changed=true;
                memcpy((*it)->old_buffer,(*it)->buffer,(*it)->len);
            }
            
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
                    if((*it)->factor){
                         *((int32_t*)(*it)->buffer) =  *((int32_t*)(*it)->buffer) *(*it)->factor;
                    }
                    DILDBG_<<" reading INT32 attribute idx:"<<(*it)->index<<" name:"<<(*it)->name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((int32_t*)(*it)->buffer);
                   
                break;
                case DataType::TYPE_INT64:
                    if((*it)->lbe){
                        *((int64_t*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::big_endian, int64_t>(*((int64_t*)(*it)->buffer));
                    }else{
                        *((int64_t*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::little_endian, int64_t>(*((int64_t*)(*it)->buffer));
                    }
                    DILDBG_<<" reading INT64 attribute idx:"<<(*it)->index<<" name:"<<(*it)->name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((int64_t*)(*it)->buffer);
                break;

                case DataType::TYPE_DOUBLE:
                    if((*it)->lbe){
                        *((double*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::big_endian, double>(*((double*)(*it)->buffer));
                    }else{
                        *((double*)(*it)->buffer) = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                        chaos::common::utility::little_endian, double>(*((double*)(*it)->buffer));
                    }
                    if((*it)->factor){
                         *((double*)(*it)->buffer)= *((double*)(*it)->buffer)*(*it)->factor;
                    }
                    DILDBG_<<" reading DOUBLE attribute idx:"<<(*it)->index<<" name:"<<(*it)->name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((double*)(*it)->buffer);

                    break;
                    
                default:
                    break;
            }
        }
    }
    if(changed){
    //! set output dataset as changed
        getAttributeCache()->setOutputDomainAsChanged();
    } else {
        DILDBG_<<" Nothing changed";
 
    }
}

//!Execute the Control Unit work
void DataImport::unitStop() throw(chaos::CException) {
    
}

//!Deinit the Control Unit
void DataImport::unitDeinit() throw(chaos::CException) {
    if(driver_interface) delete(driver_interface);
}
