/*
 *	ModbusDataImporterDriver.h
 *	!CHAOS
 *	Created by Andrea Michelotti
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

#ifndef __data_import__ModbusDataImporterDriver__
#define __data_import__ModbusDataImporterDriver__

#include <driver/data-import/core/AbstractDataImportDriver.h>

#include <chaos/common/data/DatasetDB.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <driver/modbus/core/ChaosModbusInterface.h>
#include <common/modbus/ModBus.h>

/*
 driver definition
 */
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(ModbusDataImporterDriver)

namespace driver {

namespace data_import {

class ModbusDataImporterDriver : public AbstractDataImportDriver {
  
  ::common::modbus::AbstractModbusChannel_psh driver;

 protected:
  int32_t slave_id;
  void driverInit(const chaos::common::data::CDataWrapper& data);
  void driverInit(const char *initParameter) ;
  void driverDeinit() ;
  int  fetchData(void *buffer, unsigned int buffer_len, const std::string key = "");
  int  readDataOffset(void *data_ptr, const std::string &key, uint32_t offset, uint32_t lenght);

 public:
  ModbusDataImporterDriver();
  ~ModbusDataImporterDriver();
};
}  // namespace data_import
}  // namespace driver
#endif /* defined(__DataImport__ModbusDataImporterDriver__) */
