/*
 *	MemcacheClient.cpp
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2023 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/chaos_errors.h>
#include <chaos/common/global.h>

#define MemcacheClientLAPP_ INFO_LOG(MemcacheClient)
#define MemcacheClientLDBG_ DBG_LOG(MemcacheClient)
#define MemcacheClientLERR_ ERR_LOG(MemcacheClient)

#define MemcacheClientLOG_MC_ERROR(x)                             \
  if (mc_result != MEMCACHED_SUCCESS) {                                        \
    MemcacheClientLERR_ << memcached_strerror(mc_client, x);      \
    throw chaos::CException(-10,memcached_strerror(mc_client, x),__PRETTY_FUNCTION__);\
  }

#include <driver/data-import/models/memcache/MemcacheClient.h>

using namespace driver::data_import;
// GET_PLUGIN_CLASS_DEFINITION
// we need only to define the driver because we don't are makeing a plugin
  void MemcacheClient::addServer(const std::string& name,int port){
      memcached_return_t mc_result = MEMCACHED_SUCCESS;

 if ((mc_result = memcached_server_add(
               mc_client, name.c_str(),port)) !=
          MEMCACHED_SUCCESS) {
        MemcacheClientLOG_MC_ERROR(mc_result);
  }
  }

   int MemcacheClient::get(const std::string&key,void*buffer,size_t buflen,size_t&len){
    uint32_t flags = 0;
    memcached_return_t rc;
    int copied=-1;
    char *value = memcached_get(mc_client, key.c_str(), key.size(),
                                &len, &flags, &rc);
    if ((value != NULL)&&(rc==MEMCACHED_SUCCESS)) {
      if (len > buflen) {
        MemcacheClientLERR_ << "The size "<<len<<" of key " << key
                                         << " doesn't fit into buffer size:"<<buflen;
      } else {
        copied=std::min(len,buflen);
        std::memcpy(buffer, (const char *)value, std::min(len,buflen));
        
       /* MemcacheClientLDBG_ << "READ[" << *it
                                         << "] size:" << value_length;*/
      }
      free(value);
  }
  return copied;
   }
  /*
  */
  int MemcacheClient::set(const std::string&key,void*buf,size_t len){

    memcached_return_t ret= memcached_set(mc_client, key.c_str(), key.size(), (const char*)buf, len, (time_t) 0, 0);
    if (ret == MEMCACHED_SUCCESS){
      return len;
    }
    MemcacheClientLDBG_ << "Error writing "<<memcached_strerror(mc_client, ret);   
    return -1;
    
  }
// GET_PLUGIN_CLASS_DEFINITION
// we need to define the driver with alias version and a class that implement it
MemcacheClient::MemcacheClient()  {
  memcached_return_t mc_result = MEMCACHED_SUCCESS;

  mc_client = memcached(NULL, 0);
  if (mc_client == NULL) {
    throw chaos::CException(-9, "Error allocating mc_client",
                            __PRETTY_FUNCTION__);
  }
  MemcacheClientLOG_MC_ERROR(
      mc_result = memcached_behavior_set(
          mc_client, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, (uint64_t)1))
      MemcacheClientLOG_MC_ERROR(
          mc_result = memcached_behavior_set(
              mc_client, MEMCACHED_BEHAVIOR_NO_BLOCK, (uint64_t)1))
          MemcacheClientLOG_MC_ERROR(
              mc_result = memcached_behavior_set(
                  mc_client, MEMCACHED_BEHAVIOR_NOREPLY, (uint64_t)1))
              MemcacheClientLOG_MC_ERROR(
                  mc_result = memcached_behavior_set(
                      mc_client, MEMCACHED_BEHAVIOR_KETAMA, (uint64_t)1))
                  MemcacheClientLOG_MC_ERROR(
                      mc_result = memcached_behavior_set(
                          mc_client, MEMCACHED_BEHAVIOR_TCP_NODELAY,
                          (uint64_t)1))
                      MemcacheClientLOG_MC_ERROR(
                          mc_result = memcached_behavior_set(
                              mc_client,
                              MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT,
                              (uint64_t)2))
                          MemcacheClientLOG_MC_ERROR(
                              mc_result = memcached_behavior_set(
                                  mc_client,
                                  MEMCACHED_BEHAVIOR_REMOVE_FAILED_SERVERS,
                                  (uint64_t)1))
                              MemcacheClientLOG_MC_ERROR(
                                  mc_result = memcached_behavior_set(
                                      mc_client,
                                      MEMCACHED_BEHAVIOR_RETRY_TIMEOUT,
                                      (uint64_t)2))
                                  MemcacheClientLOG_MC_ERROR(
                                      mc_result = memcached_behavior_set(
                                          mc_client,
                                          MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT,
                                          (uint64_t)500))
                                      MemcacheClientLOG_MC_ERROR(
                                          mc_result = memcached_behavior_set(
                                              mc_client,
                                              MEMCACHED_BEHAVIOR_TCP_KEEPALIVE,
                                              (uint64_t)1))

  

}

MemcacheClient::~MemcacheClient() {
  DEBUG_CODE(MemcacheClientLDBG_ << "DEINIT MEMCACHED");

  if (mc_client) {
    memcached_free(mc_client);
    mc_client = NULL;
  }
}
