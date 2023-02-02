/*
 *	RTGenericDanteCU.h
 *	!CHOAS
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

#ifndef ChaosRTControlUnit_RTGenericDanteCU_h
#define ChaosRTControlUnit_RTGenericDanteCU_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include "DanteDriver.h"
class RTGenericDanteCU:
public chaos::cu::control_manager::RTAbstractControlUnit {
    PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTGenericDanteCU)
protected:
    
    ::driver::data_import::DanteDriver driver;
public:
    /*!
     Construct a new CU with full constructor
     */
    RTGenericDanteCU(const std::string& _control_unit_id,
               const std::string& _control_unit_param,
               const ControlUnitDriverList& _control_unit_drivers);
    /*!
     Destructor a new CU
     */
    ~RTGenericDanteCU();
    
    
protected:
    
    void unitDefineActionAndDataset();
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

#endif
