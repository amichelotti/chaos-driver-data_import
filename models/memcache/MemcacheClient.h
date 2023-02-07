/*
 *	MemcacheClient.h
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

#ifndef __data_import__MemcacheClient__
#define __data_import__MemcacheClient__



#include <vector>
#include <string>
#include <libmemcached/memcached.h>

namespace driver {

namespace data_import {

class MemcacheClient  {
  std::vector<std::string>        servers;
  uint32_t                        data_pack_len;
  memcached_st *                  mc_client;

 protected:

  
 public:
  MemcacheClient();
  ~MemcacheClient();
  void addServer(const std::string& name,int port=11211);
    /**
     * @brief get a memcached key
     * 
     * @param key key
     * @param buffer output buffer
     * @param buflen size of the buffer
     * @param len returned key buf
     * @return int >=0 number of bytes retrieved  <0 error
     */
  int get(const std::string&key,void*buffer,size_t buflen,size_t&len);
  /*
  */
  int set(const std::string&key,void*buf,size_t len);
  int set(const std::string&key,const std::string&val){return set(key,(void*)val.c_str(),val.size());}


};
}  // namespace data_import
}  // namespace driver
#endif /* defined(__DataImport__MemcacheClient__) */
