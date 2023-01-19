/*
 *	DataImport.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2021 INFN, National Institute of Nuclear Physics
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

#ifndef ChaosRTControlUnit_DataImport_h
#define ChaosRTControlUnit_DataImport_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <driver/data-import/core/DataImportDriverInterface.h>

#include "AttributeOffLen.h"

namespace driver {

namespace data_import {

class DataImport : public chaos::cu::control_manager::RTAbstractControlUnit {
  PUBLISHABLE_CONTROL_UNIT_INTERFACE(DataImport)
 protected:
  ::driver::data_import::AttributeOffLenVec attribute_off_len_vec;

  DataImportDriverInterface* driver_interface;

 public:
  /*!
     Construct a new CU with full constructor
     */
  DataImport(const std::string&           _control_unit_id,
             const std::string&           _control_unit_param,
             const ControlUnitDriverList& _control_unit_drivers);
  /*!
     Destructor a new CU
     */
  ~DataImport();

 protected:
  /*!
     read the json control unit load parameter need to create the dataset form the data to import.
     The json format is like the template below:
     {
     -the name of the attribute
     "name": string,
     
     -the description fo the attribute
     "description": string,
     
     -the type of the attribute as: (u)int32, (u)int64, double, string, binary, boolean
     "type": string.
     
     -the offset/base is mandatory and represent the start of the value within a datablock
     "offset": int
     
     -the len is mandatory and represent how much data need to be copied
     "len": int
     
     -the maximum size of the attribute used only with string or binary types
     "max_size": int
     }]
     }
     */
  void unitDefineActionAndDataset() ;
  /*!(Optional)
     Define the Control Unit custom attribute
     */
  void unitDefineCustomAttribute();
  /*!(Optional)
     Initialize the Control Unit and all driver, with received param from MetadataServer
     */
  void unitInit() ;
  /*!(Optional)
     Execute the work, this is called with a determinated delay
     */
  void unitStart() ;
  /*!
     Execute the work, this is called with a determinated delay, it must be as fast as possible
     */
  void unitRun() ;

  /*!(Optional)
     The Control Unit will be stopped
     */
  void unitStop() ;

  /*!(Optional)
     The Control Unit will be deinitialized and disposed
     */
  void unitDeinit() ;
};
}  // namespace data_import
}  // namespace driver
#endif
