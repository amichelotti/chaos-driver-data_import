/*
 *	RTChaos2Memcache.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *      Collects and align a given series of Libera BPMs
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
#ifndef ChaosRTControlUnit_RTChaos2Memcache_h
#define ChaosRTControlUnit_RTChaos2Memcache_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <driver/misc/core/ChaosControllerGroup.h>
#include <driver/misc/core/ChaosDatasetAttribute.h>
#include <driver/misc/core/ChaosDatasetAttributeSinchronizer.h>
#include <driver/misc/core/RTDataSync.h>
#include <libmemcached/memcached.h>

   
    namespace driver {
        
        namespace data_import {
                
class RTChaos2Memcache : public ::driver::misc::RTDataSync {
	PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTChaos2Memcache)
public:
    /*!
     Construct a new CU with full constructor
     */
    RTChaos2Memcache(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers);
    /*!
     Destructor a new CU
     */
    ~RTChaos2Memcache();

protected:
    memcached_st *mc_client;
    std::string server_url;
    std::string server_key;
    char *buffer;
    int size;
    std::vector< ::driver::misc::ChaosDatasetAttribute* > rattrs;
     
    /*!
		Define the Control Unit Dataset and Actions
		*/
		void unitDefineActionAndDataset()throw(chaos::CException);
                void unitInit() throw(chaos::CException);
			/*(Optional)
			 Execute the work, this is called with a determinated delay, it must be as fast as possible
			 */
			void unitStart() throw(chaos::CException);
			/*(Optional)
			 The Control Unit will be stopped
			 */
			void unitStop() throw(chaos::CException);
			/*(Optional)
			 The Control Unit will be deinitialized and disposed
			 */
			void unitDeinit() throw(chaos::CException);
			void unitRun() throw(chaos::CException);

 
};
        }}
#endif
