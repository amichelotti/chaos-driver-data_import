/*
 *	ModbusDataImporterDriver.cpp
 *	!CHAOS
 * 2022
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2022 INFN, National Institute of Nuclear Physics
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


#include <stdlib.h>
#include <string.h>
#include <common/debug/core/debug.h>
#define ModbusDataImporterDriverLAPP_	INFO_LOG(ModbusDataImporterDriver)
#define ModbusDataImporterDriverLDBG_	DBG_LOG(ModbusDataImporterDriver)
#define ModbusDataImporterDriverLERR_	ERR_LOG(ModbusDataImporterDriver)
#include <iostream>
#include <fstream>
#include <functional>
#include <string>

#include "ModbusDataImporterDriver.h"
#include <common/modbus/core/ModbusChannelFactory.h>

using namespace ::driver::data_import;
//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(ModbusDataImporterDriver, 1.0.0, ::driver::data_import::ModbusDataImporterDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(::driver::data_import::ModbusDataImporterDriver, server_url [array of strings like host:port])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(::driver::data_import::ModbusDataImporterDriver, data_key [string])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(::driver::data_import::ModbusDataImporterDriver, data_pack_len [uint32_t])
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
ModbusDataImporterDriver::ModbusDataImporterDriver():slave_id(-1){
	
}

ModbusDataImporterDriver::~ModbusDataImporterDriver() {
   driverDeinit();
}
void ModbusDataImporterDriver::driverInit(const chaos::common::data::CDataWrapper& cjson){
if(cjson.hasKey("slave_id")){
		slave_id=cjson.getInt32Value("slave_id");
	}
	driver = ::common::modbus::ModbusChannelFactory::getChannel(cjson);
	if(driver.get() == NULL){
         throw chaos::CException(-1, "Cannot allocate resources for Modbus driver", __PRETTY_FUNCTION__);
    }

    LDBG_ << __FUNCTION__ <<" driver count:"<<driver.use_count()<<" pointer:x"<<std::hex<<driver.get()<<std::dec;

   
     if(driver->connect()==false){
         throw chaos::CException(-3, "cannot perform modbus connect", __FUNCTION__);

      }
}
void ModbusDataImporterDriver::driverInit(const char *initParameter)  {
    
    AbstractDataImportDriver::driverInit(initParameter);
	chaos::common::data::CDataWrapper cjson;
	cjson.setSerializedJsonData(initParameter);
	ModbusDataImporterDriverLDBG_<<"Initializing Modbus Channel "<<cjson.getJSONString();
	return driverInit(cjson);
	


}

void ModbusDataImporterDriver::driverDeinit()  {
	driver->close();
	driver->deinit();
	driver.reset();

}

int ModbusDataImporterDriver::fetchData(void *buffer, unsigned int buffer_len,const std::string key) {

	return 0;
}


int ModbusDataImporterDriver::readDataOffset(void* data_ptr, const std::string &key, uint32_t offset, uint32_t lenght){

	int ret;
	ret=driver->read_input_registers(offset,lenght,(uint16_t*)data_ptr,slave_id);

	if(ret==lenght){
		return 0;
	}
	return ret;
}



