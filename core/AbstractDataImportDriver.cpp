/*
 *	DummyDriver.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#include "AbstractDataImportDriver.h"
#include <cstdlib>
#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>

namespace cu_driver = chaos::cu::driver_manager::driver;

#define FREE_MALLOC(x) if(x){free(x); x=NULL;}

#define DataImportDriverLAPP_		LAPP_ << "[AbstractDataImportDriver] "
#define DataImportDriverLDBG_		LDBG_ << "[AbstractDataImportDriver] "
#define DataImportDriverLERR_		LERR_ << "[AbstractDataImportDriver] "

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(AbstractDataImportDriver) {
    buffer_data_block = NULL;
}

//default descrutcor
AbstractDataImportDriver::~AbstractDataImportDriver() {
    FREE_MALLOC(buffer_data_block)
}

void AbstractDataImportDriver::driverInit(const char *initParameter) throw(chaos::CException) {
	DataImportDriverLAPP_ << "Init driver";
	
}

void AbstractDataImportDriver::driverDeinit() throw(chaos::CException) {
	DataImportDriverLAPP_ << "Deinit driver";
    FREE_MALLOC(buffer_data_block)
}

void AbstractDataImportDriver::growMem(unsigned int new_mem_size) {
    buffer_data_block = std::realloc(buffer_data_block, new_mem_size);
}

//! Execute a command
cu_driver::MsgManagmentResultType::MsgManagmentResult AbstractDataImportDriver::execOpcode(cu_driver::DrvMsgPtr cmd) {
	cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
	switch(cmd->opcode) {
		case DataImportDriverOpcode_SET_CH_1:
			if(!cmd->inputData  || (sizeof(int32_t) != cmd->inputDataLength)) break;
			//we can get the value
			
		break;

		case DataImportDriverOpcode_GET_CH_1:
			
		break;
	}
	return result;
}
