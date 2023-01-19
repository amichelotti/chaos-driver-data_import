/*
 *	AbstractDataImportDriver.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include <driver/data-import/core/AbstractDataImportDriver.h>

#include <cstdlib>
#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>
#include "AttributeOffLen.h"
using namespace chaos::cu::driver_manager::driver;
using namespace driver::data_import;
#define FREE_MALLOC(x) if(x){free(x); x=NULL;}

#define ADIDLAPP_	INFO_LOG(AbstractDataImportDriver)
#define ADIDLDBG_	DBG_LOG(AbstractDataImportDriver)
#define ADIDLERR_	ERR_LOG(AbstractDataImportDriver)

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(AbstractDataImportDriver) {
    buffer_data_block = NULL;
    buffer_len = 0L;
    
    maxUpdateRefresh=100;

}

//default descrutcor
AbstractDataImportDriver::~AbstractDataImportDriver() {
    FREE_MALLOC(buffer_data_block)
}

void AbstractDataImportDriver::driverInit(const char *initParameter)  {
    ADIDLAPP_ << "Init driver";
    
}

void AbstractDataImportDriver::driverDeinit()  {
    ADIDLAPP_ << "Deinit driver";
    FREE_MALLOC(buffer_data_block)
}

bool AbstractDataImportDriver::growMem(unsigned int new_mem_size) {
    if((buffer_data_block = (char*)std::realloc(buffer_data_block, new_mem_size)) != NULL) {
        buffer_len = new_mem_size;
    }
    return buffer_data_block != NULL;
}

int AbstractDataImportDriver::fetch(const std::string key){
     std::map<std::string, uint64_t>::iterator i   = last_fetch.find(key);
  uint64_t                                  now = chaos::common::utility::TimingUtil::getTimeStamp();
  if ((i == last_fetch.end()) || ((now - i->second) > maxUpdateRefresh)) {
        last_fetch[key] = now;
         return fetchData(buffer_data_block,buffer_len,key);

  }
    return 0;
}
int AbstractDataImportDriver::fetchData(void *buffer, unsigned int buffer_len,const std::string key){
        ADIDLERR_ << "Not implemented";

    return 0;

}

//read data from offset
int AbstractDataImportDriver::readDataOffset(void* data_ptr,
                                             uint32_t offset,
                                             uint32_t lenght) {
    int err = 0;
    CHAOS_ASSERT(buffer_data_block)
    if((offset+lenght)>buffer_len) return -1;
    //copy seletected portion of data
    std::memcpy(data_ptr, (buffer_data_block+offset), lenght);
    return err;
}
int AbstractDataImportDriver::readDataOffset(AttributeOffLen*v){
    return copy(v,(buffer_data_block+v->offset));

}

int  AbstractDataImportDriver::readDataOffset(void* data_ptr, const std::string& key,uint32_t offset, uint32_t lenght){
    ADIDLERR_<<" not implemented: key:"<<key<<" off:"<<offset<<" len:"<<lenght;
    return -1;
}
//! Execute a command
MsgManagmentResultType::MsgManagmentResult AbstractDataImportDriver::execOpcode(DrvMsgPtr cmd) {
    MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
        case DataImportDriverOpcode_FETCH_NEW_DATABLOCK:
            if((cmd->ret = fetch())) {
	      //ADIDLERR_<<"Error during the new datablock fetch command with code:" << cmd->ret;
	      //result = MsgManagmentResultType::MMR_ERROR;
            } else {
                DEBUG_CODE(ADIDLDBG_ << "New datablock fetched";)
            }
            break;

            
        case DataImportDriverOpcode_GET_DATA:{
            uint32_t offset=cmd->parm[0];
            cmd->resultDataLength = (uint32_t)cmd->parm[1];
            cmd->ret = readDataOffset(cmd->resultData, offset, cmd->resultDataLength);
            if (cmd->ret != 0) {
	      //                ADIDLERR_<<"get data "<<offset<<" lenght:"<<cmd->resultDataLength<<" ret:"<<cmd->ret;

	      //                result = MsgManagmentResultType::MMR_ERROR;
            }
            break;
        }
         case DataImportDriverOpcode_GET_KEY_DATA:{
            uint32_t offset=cmd->parm[0];
            cmd->resultDataLength = (uint32_t)cmd->parm[1];
            std::string arg((const char*)cmd->inputData);
            cmd->ret = readDataOffset(cmd->resultData, arg,offset, cmd->resultDataLength);
            if (cmd->ret != 0) {
	      //                ADIDLERR_<<"get data "<<offset<<" lenght:"<<cmd->resultDataLength<<" ret:"<<cmd->ret;

	      //                result = MsgManagmentResultType::MMR_ERROR;
            }
            break;
         }
    }
    return result;
}
