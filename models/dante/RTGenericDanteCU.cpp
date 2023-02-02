/*
 *	RTGenericDanteCU.cpp
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

#include "RTGenericDanteCU.h"
#include <chaos/cu_toolkit/control_manager/ControlUnitTypes.h>
using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(RTGenericDanteCU)

#define DI_CUSTOM_HEAD "[" << getCUID()<< "] - "
#define DILAPP_	INFO_LOG(RTGenericDanteCU) << DI_CUSTOM_HEAD
#define DILDBG_	DBG_LOG(RTGenericDanteCU) << DI_CUSTOM_HEAD
#define DILERR_	ERR_LOG(RTGenericDanteCU) << DI_CUSTOM_HEAD


#define LOG_AND_THROW(code, msg)\
DILERR_ << msg;\
throw chaos::CException(code, msg, __PRETTY_FUNCTION__);

/*
 Construct
 */
RTGenericDanteCU::RTGenericDanteCU(const string& _control_unit_id,
                       const string& _control_unit_param,
                       const ControlUnitDriverList& _control_unit_drivers):
RTAbstractControlUnit(_control_unit_id,
                      _control_unit_param,
                      _control_unit_drivers){
                        driver.driverInit(_control_unit_param.c_str());

}

/*
 Destructor
 */
RTGenericDanteCU::~RTGenericDanteCU() {
    //clear all allocated slot
    
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
void RTGenericDanteCU::unitDefineActionAndDataset()  {

    chaos::common::data::CDWUniquePtr p=driver.getDataset();

    if(p.get()){
            DILDBG_<<"DATASET:"<<p->getJSONString();

        addAttributesToDataSet(*p.get(),chaos::DataType::DataSetAttributeIOAttribute::Output);
    }

}


//!Define custom control unit attribute
void RTGenericDanteCU::unitDefineCustomAttribute() {
}

//!Initialize the Custom Control Unit
void RTGenericDanteCU::unitInit()  {
    
    
}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void RTGenericDanteCU::unitStart()  {
    
}

//!Execute the Control Unit work
void RTGenericDanteCU::unitRun()  {
     chaos::common::data::CDWUniquePtr p=driver.getDataset();
    if(p.get()){
        updateDataSet(*p.get());
        DILDBG_<<"UPDATE DATASET:"<<p->getJSONString();

    }
          getAttributeCache()->setOutputDomainAsChanged();

  
}

//!Execute the Control Unit work
void RTGenericDanteCU::unitStop()  {
    
}

//!Deinit the Control Unit
void RTGenericDanteCU::unitDeinit()  {
}
