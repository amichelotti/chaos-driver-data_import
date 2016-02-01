/*
 *	RTChaos2Memcache.cpp
 *	!CHAOS
 *      Andrea Michelotti 29/1/2016
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

#include "RTChaos2Memcache.h"
//#include "RTChaos2MemcacheDriver.h"
#include <boost/algorithm/string.hpp>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <common/debug/core/debug.h>
#include<boost/algorithm/string.hpp>

using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;
using namespace driver::data_import;
using namespace driver::misc;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(RTChaos2Memcache)


#define RTChaos2MemcacheLAPP_		LAPP_ << "[RTChaos2Memcache] "
#define RTChaos2MemcacheLDBG_		LDBG_ << "[RTChaos2Memcache] " << __PRETTY_FUNCTION__ << " "
#define RTChaos2MemcacheLERR_		LERR_ << "[RTChaos2Memcache] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



/*
 Construct
 */
RTChaos2Memcache::RTChaos2Memcache(const string& _control_unit_id, const string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
driver::misc::RTDataSync(_control_unit_id,_control_unit_param,_control_unit_drivers) {
    buffer=0;size=0;
    DPRINT("CREATED");
}


    

/*
 Destructor
 */
RTChaos2Memcache::~RTChaos2Memcache() {

}

void RTChaos2Memcache::unitDefineActionAndDataset() throw(chaos::CException) {
        DPRINT("Creating attributes");

     addAttributeToDataSet("memcacheurl",
						  "Memcache server",
						  DataType::TYPE_STRING,
						  DataType::Input,256);
     
     addAttributeToDataSet("memcachekey",
						  "Memcache output key",
						  DataType::TYPE_STRING,
						  DataType::Input,256);
     
    RTDataSync::unitDefineActionAndDataset();
    
   
}



//!Initialize the Custom Control Unit
void RTChaos2Memcache::unitInit() throw(chaos::CException) {
    memcached_return_t              mc_result = MEMCACHED_SUCCESS;
    DPRINT("INIT");

    std::vector<std::string> urls;
    const char*url=getAttributeCache()->getROPtr<char>(DOMAIN_INPUT, "memcacheurl");
    const char*key=getAttributeCache()->getROPtr<char>(DOMAIN_INPUT, "memcachekey");
    if(url == NULL || key ==NULL){
        throw chaos::CException(-1,__PRETTY_FUNCTION__, "cannot find 'memcacheurl' or 'memcachekey' required initialization  ");
    }
     mc_client = memcached(NULL, 0);
    if(mc_client == NULL) {
        throw chaos::CException(-9, "Error allocating mc_client", __PRETTY_FUNCTION__);
    }
     
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, (uint64_t)1);
     mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_NO_BLOCK, (uint64_t)1);
   mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_NOREPLY, (uint64_t)1);
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_KETAMA, (uint64_t)1);
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_TCP_NODELAY,(uint64_t)1);
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, (uint64_t)2);
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, (uint64_t)1);
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT,(uint64_t)2);
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT,(uint64_t)500);
    mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_TCP_KEEPALIVE,(uint64_t)1);

    
    server_url.assign(url);
    server_key.assign(key);
    boost::split(urls,server_url,boost::is_any_of(":"));
    DPRINT("Adding server %s",server_url.c_str());

  try {
            //try adding server
            
            if((mc_result = memcached_server_add(mc_client, urls[0].c_str(), boost::lexical_cast<in_port_t>(urls[1]))) != MEMCACHED_SUCCESS) {
                throw chaos::CException(-11, "Error adding server into memcache client", __PRETTY_FUNCTION__);
            }
        } catch(boost::bad_lexical_cast const& e) {
            throw chaos::CException(-12, e.what(), __PRETTY_FUNCTION__);
        }
    rattrs=data_group->getAttributes();
    for (std::vector<ChaosDatasetAttribute*>::iterator i=rattrs.begin();i!=rattrs.end();i++){
        std::string name=(*i)->getName();
        if((*i)->getDir()== chaos::DataType::Output){
            size+=(*i)->getSize();
            DPRINT("dynamic adding attribute: %s size :%d dir: %d type:%d, tot size:%d",name.c_str(),(*i)->getSize(),(*i)->getDir(),(*i)->getType(),size);
        }
        
    }
    if(size){
        size*=2;
        DPRINT("allocating a buffer of %d bytes",size);
        buffer = (char*)malloc(size);
    }
    
}

void RTChaos2Memcache::unitRun() throw(chaos::CException) {
    int off=0;
    DPRINT("RUN");
     for (std::vector<ChaosDatasetAttribute*>::iterator i=rattrs.begin();i!=rattrs.end();i++){
        if((*i)->getDir()== chaos::DataType::Output){
            uint32_t siz;
            void*ptr=(*i)->get(&siz);
            if(off+siz>=size){
                DERR("some remote variables changed their size");
                buffer=(char*)realloc((void*)buffer,off+siz+size);
                size=off+siz+size;                
            }
            memcpy(buffer+off,ptr,siz);
           off+=siz;

        }
        
    }
    DPRINT("pushing %d bytest to %s key:%s",off,server_url.c_str(),server_key.c_str());
    memcached_set(mc_client,server_key.c_str(),server_key.length(),buffer,off,0,0);
    
  
}
//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void RTChaos2Memcache::unitStart() throw(chaos::CException) {
 
}
//!Execute the Control Unit work
void RTChaos2Memcache::unitStop() throw(chaos::CException) {

}

//!Deinit the Control Unit
void RTChaos2Memcache::unitDeinit() throw(chaos::CException) {
       if(mc_client){
        memcached_free(mc_client);
        mc_client = NULL;
    }
       if(buffer){
           free(buffer);
           size=0;
           buffer=0;
       }

}
