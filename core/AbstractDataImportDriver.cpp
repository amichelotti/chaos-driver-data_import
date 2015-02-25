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

#include <driver/data_import/core/AbstractDataImportDriver.h>

#include <cstdlib>
#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>

using namespace chaos::cu::driver_manager::driver;

#define FREE_MALLOC(x) if(x){free(x); x=NULL;}

#define ADIDLAPP_	INFO_LOG(AbstractDataImportDriver)
#define ADIDLDBG_	DBG_LOG(AbstractDataImportDriver)
#define ADIDLERR_	ERR_LOG(AbstractDataImportDriver)

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(AbstractDataImportDriver) {
    buffer_data_block = NULL;
    buffer_len = 0L;
}

//default descrutcor
AbstractDataImportDriver::~AbstractDataImportDriver() {
    FREE_MALLOC(buffer_data_block)
}

void AbstractDataImportDriver::driverInit(const char *initParameter) throw(chaos::CException) {
    ADIDLAPP_ << "Init driver";
    
}

void AbstractDataImportDriver::driverDeinit() throw(chaos::CException) {
    ADIDLAPP_ << "Deinit driver";
    FREE_MALLOC(buffer_data_block)
}

bool AbstractDataImportDriver::growMem(unsigned int new_mem_size) {
    if((buffer_data_block = std::realloc(buffer_data_block, new_mem_size)) != NULL) {
        buffer_len = new_mem_size;
    }
    return buffer_data_block != NULL;
}

//read data from offset
int AbstractDataImportDriver::readDataOffset(void* data_ptr,
                                             uint32_t offset,
                                             uint32_t lenght) {
    int err = 0;
    CHAOS_ASSERT(buffer_data_block)
    //cast to char to simplify the pinter artimetic
    const char * ch_casted_buf = static_cast<char*>(data_ptr);
    
    //copy seletected portion of data
    std::memcpy(data_ptr, (ch_casted_buf+offset), lenght);
    return err;
}

//! Execute a command
MsgManagmentResultType::MsgManagmentResult AbstractDataImportDriver::execOpcode(DrvMsgPtr cmd) {
    MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
        case DataImportDriverOpcode_GET_DATA:
            uint32_t offset=cmd->parm[0];
            cmd->resultDataLength = (uint32_t)cmd->parm[1];
            if(!fetchData(buffer_data_block,
                          buffer_len)) {
                ADIDLAPP_<<"Read offsett:"<<offset<<" lenght:"<<cmd->resultDataLength;
                cmd->ret = readDataOffset(cmd->resultData, offset, cmd->resultDataLength);
                if (cmd->ret != 0) {
                    result = MsgManagmentResultType::MMR_ERROR;
                }
            } else {
                ADIDLERR_<<"Data buffer of the message have lower size of the requested command [offset:"<<offset<<" lenght:"<<cmd->resultDataLength<<"]";
            }
            break;
    }
    return result;
}
