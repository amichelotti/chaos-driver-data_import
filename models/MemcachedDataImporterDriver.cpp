/*
 *	MemcachedDataImporterDriver.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
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

#define MemcachedDataImporterDriverLAPP_	INFO_LOG(MemcachedDataImporterDriver)
#define MemcachedDataImporterDriverLDBG_	DBG_LOG(MemcachedDataImporterDriver)
#define MemcachedDataImporterDriverLERR_	ERR_LOG(MemcachedDataImporterDriver)

#define MemcachedDataImporterDriverLOG_MC_ERROR(x)\
if(mc_result != MEMCACHED_SUCCESS) {\
MemcachedDataImporterDriverLERR_ << memcached_strerror(mc_client, x);\
}

#include <driver/data_import/models/MemcachedDataImporterDriver.h>

#include <json/json.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(MemcachedDataImporterDriver, 1.0.0, MemcachedDataImporterDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(MemcachedDataImporterDriver, server_url [array of strings like host:port])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(MemcachedDataImporterDriver, data_key [string])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(MemcachedDataImporterDriver, data_pack_len [uint32_t])
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION


//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
MemcachedDataImporterDriver::MemcachedDataImporterDriver():
mc_client(NULL) {
}

MemcachedDataImporterDriver::~MemcachedDataImporterDriver() {
    if(mc_client){
        memcached_free(mc_client);
        mc_client = NULL;
    }
}

void MemcachedDataImporterDriver::driverInit(const char *initParameter) throw(chaos::CException) {
    Json::Value						json_parameter;
    Json::StyledWriter				json_writer;
    Json::Reader					json_reader;
    std::string                     host_port;
    std::vector<std::string>        host_port_vec;
    memcached_return_t              mc_result = MEMCACHED_SUCCESS;
    
    AbstractDataImportDriver::driverInit(initParameter);
    //parse json string
    std::string json_string(initParameter);
    if(!json_reader.parse(json_string, json_parameter)) {
        throw chaos::CException(-1, "Json init paremeter has not been parsed", __PRETTY_FUNCTION__);
    }
    
    DEBUG_CODE(MemcachedDataImporterDriverLDBG_ << "Received JOSN parameter:" << json_writer.write(json_parameter);)
    
    //fetch value from json document
    const Json::Value& json_server_urls = json_parameter["server_url"];
    const Json::Value& json_data_key = json_parameter["data_key"];
    const Json::Value& json_data_pack_len= json_parameter["data_pack_len"];
    
    //check madatory data
    if (json_server_urls.isNull()) {
        throw chaos::CException(-2, "server_url is mandatory", __PRETTY_FUNCTION__);
    }
    
    if (!json_server_urls.isArray()) {
        throw chaos::CException(-3, "server_url attribute need to be an array", __PRETTY_FUNCTION__);
    }
    
    if (json_data_key.isNull()) {
        throw chaos::CException(-4, "data_key attribute is mandatory", __PRETTY_FUNCTION__);
    }
    
    if (!json_data_key.isString()) {
        throw chaos::CException(-5, "data_key needs to be a string", __PRETTY_FUNCTION__);
    }
 
    //get the key to use
    data_key = json_data_key.asString();
    
    if (json_data_pack_len.isNull()) {
        throw chaos::CException(-6, "data_pack_len attribute is mandatory", __PRETTY_FUNCTION__);
    }
    
    if (!json_data_pack_len.isUInt()) {
        throw chaos::CException(-7, "data_pack_len needs to be an unsigned int value", __PRETTY_FUNCTION__);
    }
    //get the datpack len
    if(!growMem(json_data_pack_len.asUInt())) {
        throw chaos::CException(-8, "Unable to allocate the memory for the data pack buffer", __PRETTY_FUNCTION__);
    }
    //allcoate mc client
    mc_client = memcached(NULL, 0);
    if(mc_client == NULL) {
        throw chaos::CException(-9, "Error allocating mc_client", __PRETTY_FUNCTION__);
    }
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, (uint64_t)1))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_NO_BLOCK, (uint64_t)1))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_NOREPLY, (uint64_t)1))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_KETAMA, (uint64_t)1))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_TCP_NODELAY,(uint64_t)1))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, (uint64_t)2))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS, (uint64_t)1))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT,(uint64_t)2))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT,(uint64_t)500))
    MemcachedDataImporterDriverLOG_MC_ERROR(mc_result = memcached_behavior_set(mc_client, MEMCACHED_BEHAVIOR_TCP_KEEPALIVE,(uint64_t)1))

    
    //add server url to the memcached client
    for (Json::ValueConstIterator it = json_server_urls.begin();
         it != json_server_urls.end();
         ++it) {
        //clean the previously fetched info
        host_port_vec.clear();
        //get the string
        host_port = it->asString();
        //split host form port
        boost::split(host_port_vec,
                     host_port,
                     boost::is_any_of(":"));
        
        //check if the host string is correct
        if(host_port_vec.size() != 2) {
            throw chaos::CException(-10, "the single host url need to be of the form host:port", __PRETTY_FUNCTION__);
        }
        
        try {
            //try adding server
            DEBUG_CODE(MemcachedDataImporterDriverLDBG_ << "Adding server " << host_port_vec[0];)
            
            if((mc_result = memcached_server_add(mc_client, host_port_vec[0].c_str(), boost::lexical_cast<in_port_t>(host_port_vec[1]))) != MEMCACHED_SUCCESS) {
                MemcachedDataImporterDriverLOG_MC_ERROR(mc_result)
                throw chaos::CException(-11, "Error adding server into memcache client", __PRETTY_FUNCTION__);
            }
        } catch(boost::bad_lexical_cast const& e) {
            throw chaos::CException(-12, e.what(), __PRETTY_FUNCTION__);
        }
    }
}

void MemcachedDataImporterDriver::driverDeinit() throw(chaos::CException) {
    if(mc_client){
        memcached_free(mc_client);
        mc_client = NULL;
    }
    AbstractDataImportDriver::driverDeinit();
}

int MemcachedDataImporterDriver::fetchData(void *buffer, unsigned int buffer_len) {
    CHAOS_ASSERT(mc_client)
    int err = 0;
    uint32_t flags= 0;
    memcached_return_t rc;
    size_t value_length= 0;
    
    //get the value form memcached
    char *value = memcached_get(mc_client,
                                data_key.c_str(),
                                data_key.length(),
                                &value_length,
                                &flags,
                                &rc);
    //check if we have something
    if (value != NULL) {
        if(value_length != buffer_len) {
            err = -1;
            MemcachedDataImporterDriverLERR_ << "The found value for key " << data_key << " doesn't match the data pack lenght";
        } else {
            //copy data to main buffer
            std::memcpy(buffer, (const char*)value, buffer_len);
        }
        free(value);
    }else {
            MemcachedDataImporterDriverLERR_ << "Error retriving data from key " <<data_key;
            err = -2;
    }
    
    return err;
}