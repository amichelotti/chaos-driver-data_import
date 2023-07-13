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
#include <cmath>
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
void DataImport::unitDefineActionAndDataset()  {
    //insert your definition code here
    Json::Value						json_parameter;
    Json::StyledWriter				json_writer;
    Json::Reader					json_reader;
    addStateVariable(StateVariableTypeAlarmCU, "fetching_data_empty","data empty block");

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
            std::string name;
            std::copy_if((*i)->name.begin(),(*i)->name.end(),std::back_inserter(name),[](unsigned char c){return !std::isspace(c);});

            addStateVariable(StateVariableTypeAlarmCU, "fetching_key_of_"+name,"Error fetching key");
            addStateVariable(StateVariableTypeAlarmCU, "invalid_data_on_"+name,"Invalid data");

            switch((*i)->type) {
            case DataType::TYPE_JSON:{
                if((*i)->jsond.size()==0){
                    throw chaos::CException(-1, "Empty JSON specification in 'json' key", __PRETTY_FUNCTION__);

                }
                try {
                   chaos::common::data::CDataWrapper param;
                   param.setSerializedJsonData((*i)->jsond.c_str());
                  DEBUG_CODE(DILDBG_ << "Adding dataset" <<param.getJSONString());

                   addAttributesToDataSet(param); // add parameters from JSON
                   

                } catch(...){
                    throw chaos::CException(-2, "No valid JSON in 'json':"+(*i)->jsond, __PRETTY_FUNCTION__);

                }
            }
            break;
            case DataType::TYPE_INT32:
            case DataType::TYPE_INT64:
            case DataType::TYPE_DOUBLE:
            case DataType::TYPE_BOOLEAN:
                addAttributeToDataSet(name,
                                      (*i)->desc,
                                      (*i)->type,
                                      DataType::Output);
                break;
                
            case DataType::TYPE_VECTOR_BOOL:
                addBinaryAttributeAsSubtypeToDataSet(name, (*i)->desc, chaos::DataType::SUB_TYPE_BOOLEAN, (*i)->len, DataType::Output);
                break;
            case DataType::TYPE_VECTOR_INT32:
                addBinaryAttributeAsSubtypeToDataSet(name, (*i)->desc, chaos::DataType::SUB_TYPE_INT32, (*i)->len, DataType::Output);

            break;
            case DataType::TYPE_VECTOR_INT64:
                addBinaryAttributeAsSubtypeToDataSet(name, (*i)->desc, chaos::DataType::SUB_TYPE_INT64, (*i)->len, DataType::Output);

            break;
            case DataType::TYPE_VECTOR_DOUBLE:
                addBinaryAttributeAsSubtypeToDataSet(name, (*i)->desc, chaos::DataType::SUB_TYPE_DOUBLE, (*i)->len, DataType::Output);

            break;

            default:
                addAttributeToDataSet(name,
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
void DataImport::unitInit()  {
    
    driver_interface = new DataImportDriverInterface(getAccessoInstanceByIndex(0));
    
    //check the value set on MDS for in_1 channel
    for(::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //get hte base address of the value form the cache
        if(((*it)->type!=DataType::TYPE_JSON)){
            std::string name;
            std::copy_if((*it)->name.begin(),(*it)->name.end(),std::back_inserter(name),[](unsigned char c){return !std::isspace(c);});

            DILAPP_ "Get pointer from cache for attribute " << name;
            (*it)->buffer = getAttributeCache()->getRWPtr<void*>(chaos::common::data::cache::DOMAIN_OUTPUT, name);
            if((*it)->buffer == NULL) {
                throw chaos::CException(-1, "Error retrieving pointer of "+name, __PRETTY_FUNCTION__);
            }
        } else {
            (*it)->buffer = malloc((*it)->len);
        }
        
    }

}
int DataImport::incomingMessage(const std::string& key,  chaos::common::data::CDWUniquePtr& data){
    DILDBG_<<"packet from "<<key<<" :"<<data->getJSONString();
    return 0;
}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void DataImport::unitStart()  {
    //subscribe("AULA_TOUSHECK/TEMP/ZB_SENSOR_D0101");
    //subscribe("AULA_TOUSHECK/TEMP/ZB_SENSOR_C0501");

    
}

//!Execute the Control Unit work
void DataImport::unitRun()  {
    int err = 0;
    bool changed=false;
    //fetch new datablock
   // DILDBG_<<" Fetch from driver";

    if((err = driver_interface->fetchNewDatablock())) {
      if(err==  DATA_IMPORT_NO_CHANGE){
	    DILDBG_<<" No Data Change";
       // setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_data_block", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

	    return;
      }
    
      if(err==DATA_IMPORT_NO_DATA){
          int retry=3;
          do{
            DILERR_ << "fetching NO DATA:" << err<< " retrying.."<<retry;
            usleep(10000);
          } while((err = driver_interface->fetchNewDatablock())&& (retry--));

          if(retry<=0){
            setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_data_empty", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

	        return;
          }

      }
       DILERR_ << "fetching NO datablock return:" << err;
        setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_data_block", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

    } else {
	setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_data_block", chaos::common::alarm::MultiSeverityAlarmLevelClear);

    }


    //fetch all single attribute f)rom datablock
    for(::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
            std::string name;
            std::copy_if((*it)->name.begin(),(*it)->name.end(),std::back_inserter(name),[](unsigned char c){return !std::isspace(c);});

        //
        if((*it)->type== DataType::TYPE_JSON){
            memset((*it)->buffer,0,(*it)->len);
        }
        if((err = driver_interface->readAttribute((*it)->buffer, (*it)->keybind,(*it)->offset, (*it)->len))<0) {
            DILERR_ << "Error reading attribute " << name << " from key:"<<(*it)->keybind<<" driver with error " << err;
            setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_key_of_"+name, chaos::common::alarm::MultiSeverityAlarmLevelHigh);
            //metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError, boost::str(boost::format("Error fetching key '%1%") %  (*it)->name ));

        } else if((*it)->lbe>=0){
            setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_key_of_"+name, chaos::common::alarm::MultiSeverityAlarmLevelClear);

             if(memcmp((*it)->buffer,(*it)->old_buffer,(*it)->len)){
                changed=true;
                memcpy((*it)->old_buffer,(*it)->buffer,(*it)->len);
            }
            bool  isVector       = ((unsigned)(*it)->type) & ((unsigned)chaos::DataType::TYPE_ACCESS_ARRAY);
            uint32_t tt       = ((unsigned)(*it)->type) & (~(unsigned)chaos::DataType::TYPE_ACCESS_ARRAY);
            uint32_t size=(*it)->len;
            float factor=1.0;
            if((*it)->factor){
                factor=(*it)->factor;
            }

            //apply some filter if we need it
            switch(tt) {
                case DataType::TYPE_JSON:{
                    std::string sjson=std::string((const char*)(*it)->buffer,std::min(err,(int)(*it)->len));
                    if(sjson.size()==0){
                        DILERR_ << "Error reading attribute " << (*it)->name << " from key:"<<(*it)->keybind<<" VALUE IS NULL and len "<<err;

                        setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_key_of_"+name, chaos::common::alarm::MultiSeverityAlarmLevelHigh);
 
                    } else {
                        try{
                            chaos::common::data::CDataWrapper param;
                     //       DILDBG_ << (*it)->name << " serializing "<<sjson;

                            param.setSerializedJsonData(sjson.c_str());
                      //      DILDBG_ << (*it)->name << " serialized size:"<<err<<" "<<param.getJSONString();

                            updateDataSet(param);

                        } catch(...){
                            DILERR_ << "Error reading attribute " << (*it)->name << " from key:"<<(*it)->keybind<<" invalid JSON: " << sjson;

                            setStateVariableSeverity(StateVariableTypeAlarmCU,"fetching_key_of_"+name, chaos::common::alarm::MultiSeverityAlarmLevelHigh);
    
                        }
                    }
                }
                break;
                case DataType::TYPE_INT32:
                    if((*it)->original_type==(*it)->type){
                        int32_t *dest_buffer=(int32_t*)(*it)->buffer;
                        if(isVector==false){
                            size=sizeof(int32_t);
                        }
                        for(int cnt=0;cnt<size/sizeof(int32_t);cnt++){
                            if((*it)->lbe){
                                dest_buffer[cnt] = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::big_endian, int32_t>(dest_buffer[cnt])*factor;
                            }else{
                                dest_buffer[cnt] = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::little_endian, int32_t>(dest_buffer[cnt])*factor;
                            }
                          //  DILDBG_<<" reading INT32 attribute idx:"<<(*it)->index<<" name:"<<(*it)->name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE["<<cnt<<"]:"<< dest_buffer[cnt];

                        }
                    } else if((*it)->original_type==DataType::TYPE_STRING){
                        std::string tmp((const char*)(*it)->buffer,std::min(err,(int)(*it)->len));
                        *((int32_t*)(*it)->buffer) = atoi(tmp.c_str())*factor;
                        DILDBG_<<" reading INT32(string) attribute idx:"<<(*it)->index<<" name:"<<name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((int32_t*)(*it)->buffer);

                    }
                   
                break;
                case DataType::TYPE_INT64:
                 if((*it)->original_type==(*it)->type){
                    int64_t *dest_buffer=(int64_t*)(*it)->buffer;
                        if(isVector==false){
                            size=sizeof(int64_t);
                        }
                        for(int cnt=0;cnt<size/sizeof(int64_t);cnt++){
                            if((*it)->lbe){
                                dest_buffer[cnt] = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::big_endian, int64_t>(dest_buffer[cnt])*factor;
                            }else{
                                dest_buffer[cnt] = chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::little_endian, int64_t>(dest_buffer[cnt])*factor;
                            }
                            DILDBG_<<" reading INT32 attribute idx:"<<(*it)->index<<" name:"<<name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE["<<cnt<<"]:"<< dest_buffer[cnt];

                        }
                 } else  if((*it)->original_type==DataType::TYPE_STRING){
                        std::string tmp((const char*)(*it)->buffer,std::min(err,(int)(*it)->len));
                        *((int64_t*)(*it)->buffer) = atoll(tmp.c_str())*factor;
                    }
                   // DILDBG_<<" reading INT64 attribute idx:"<<(*it)->index<<" name:"<<(*it)->name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((int64_t*)(*it)->buffer);
                break;
                case DataType::TYPE_BOOLEAN:
                    if((*it)->original_type==(*it)->type){
                        bool *dest_buffer=(bool*)(*it)->buffer;
                        if(isVector==false){
                            size=sizeof(bool);
                        }
                        for(int cnt=0;cnt<size/sizeof(bool);cnt++){
                            dest_buffer[cnt] = dest_buffer[cnt]*factor;
                            

                        }
                    } else if((*it)->original_type==DataType::TYPE_STRING){
                        std::string tmp((const char*)(*it)->buffer,std::min(err,(int)(*it)->len));
                        *((bool*)(*it)->buffer) = atoi(tmp.c_str()) * factor;
                        DILDBG_<<" reading BOOL(string) attribute idx:"<<(*it)->index<<" name:"<<name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((bool*)(*it)->buffer);

                    }
                   
                break;
                case DataType::TYPE_FLOAT:{


                    if((*it)->original_type==(*it)->type){
                    float *dest_buffer=(float*)(*it)->buffer;
                        if(isVector==false){
                            size=sizeof(float);
                        }
                        for(int cnt=0;cnt<size/sizeof(float);cnt++){
                            if((*it)->lbe){
                                float d=chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::big_endian, float>(dest_buffer[cnt])*factor;
                                if(!std::isfinite(d)){
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                                } else {
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelClear);

                                }
                                dest_buffer[cnt] = d;
                            }else{
                                float d= chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::little_endian, float>(dest_buffer[cnt])*factor;
                                if(!std::isfinite(d)){
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                                } else {
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelClear);

                                }
                                dest_buffer[cnt]  =d;
                            }
                     //       DILDBG_<<" reading FLOAT attribute idx:"<<index<<" name:"<<name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE["<<cnt<<"]:"<< dest_buffer[cnt];

                        }
                } else if((*it)->original_type==DataType::TYPE_STRING){
                        std::string tmp((const char*)(*it)->buffer,std::min(err,(int)(*it)->len));
                        double d=atof(tmp.c_str())*factor;
                          if(!std::isfinite(d)){
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                            } else {
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelClear);

                            }
                        *((float*)(*it)->buffer) =d ;
                    }
                //    DILDBG_<<" reading FLOAT attribute idx:"<<(*it)->index<<" name:"<<name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((double*)(*it)->buffer);

                    break;

                }
                case DataType::TYPE_DOUBLE:

                if((*it)->original_type==(*it)->type){
                    double *dest_buffer=(double*)(*it)->buffer;
                        if(isVector==false){
                            size=sizeof(double);
                        }
                        for(int cnt=0;cnt<size/sizeof(double);cnt++){
                            if((*it)->lbe){
                                double d=chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::big_endian, double>(dest_buffer[cnt])*factor;
                                if(!std::isfinite(d)){
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                                } else {
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelClear);

                                }
                                dest_buffer[cnt] = d;
                            }else{
                                double d= chaos::common::utility::byte_swap<chaos::common::utility::host_endian,
                                chaos::common::utility::little_endian, double>(dest_buffer[cnt])*factor;
                                if(!std::isfinite(d)){
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                                } else {
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelClear);

                                }
                                dest_buffer[cnt]  =d;
                            }
                    //        DILDBG_<<" reading DOUBLE attribute idx:"<<(*it)->index<<" name:"<<name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE["<<cnt<<"]:"<< dest_buffer[cnt];

                        }
                } else if((*it)->original_type==DataType::TYPE_STRING){
                        std::string tmp((const char*)(*it)->buffer,(*it)->len);
                        double d=atof(tmp.c_str())*factor;
                          if(!std::isfinite(d)){
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelWarning);
                            } else {
                                    setStateVariableSeverity(StateVariableTypeAlarmCU,"invalid_data_on_"+name, chaos::common::alarm::MultiSeverityAlarmLevelClear);

                            }
                        *((double*)(*it)->buffer) =d ;
                    }
                 //   DILDBG_<<" reading DOUBLE attribute idx:"<<(*it)->index<<" name:"<<name<<"["<<(*it)->keybind<<"] off:"<<(*it)->offset<<" len:"<<err<<" maxlen:"<<(*it)->len<<" LBE:"<<(*it)->lbe<<" VALUE:"<< *((double*)(*it)->buffer);

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
      //  DILDBG_<<" Nothing changed";
 
    }
}

//!Execute the Control Unit work
void DataImport::unitStop()  {
    //subscribe("AULA_TOUSHECK/TEMP/ZB_SENSOR_D0101",false);
    //subscribe("AULA_TOUSHECK/TEMP/ZB_SENSOR_C0501",false);
}

//!Deinit the Control Unit
void DataImport::unitDeinit()  {
    for(::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
        it != attribute_off_len_vec.end();
        it++) {
        //get hte base address of the value form the cache
        if(((*it)->type==DataType::TYPE_JSON)){
            if((*it)->buffer){
                free((*it)->buffer);
            }
           
        }
        
    }
    if(driver_interface) delete(driver_interface);
}
