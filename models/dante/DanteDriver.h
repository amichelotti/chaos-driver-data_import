/*
 *	DanteDriver.h
 *	!CHOAS
 *	Created by Andrea Michelotti
 *  This drivers allows to control a Dante Devil, though REST (for commands) and memcache for data.
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


#ifndef __data_import__DanteDriver__
#define __data_import__DanteDriver__


#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <string>

#include <driver/data-import/models/memcache/MemcachedDataImporterDriver.h>

DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(DanteDriver)
/*
 driver definition
 */
#include <common/crest/chaos_crest.h>

#include <driver/data-import/core/AttributeOffLen.h>

namespace driver{

namespace data_import{

class DanteDriver:public MemcachedDataImporterDriver {
    
    std::string danteRestServer,username,password;
    chaos_crest_handle_t crest_handle;
    //chaos::common::data::CDWUniquePtr dataset;
    driver::data_import::AttributeOffLenVec attribute_off_len_vec;
    std::map<const std::string,driver::data_import::AttributeOffLen *> key2item;
protected:
    /*!
     "server_url":["host:port",...]
     "data_keys":memcached key array
     "data_pack_len":the lenght of the intere datapack to read
     */
    void driverInit(const char *initParameter) throw(chaos::CException);
    void driverDeinit() throw(chaos::CException);
    int  fetchData(void *buffer, unsigned int buffer_len);
    int readDataOffset(void* data_ptr, const std::string& key,uint32_t offset, uint32_t lenght);
public:
    /**
     * @brief makes a rest post calling the function and giving the JSON parameters
     * 
     * @param func REST function to call
     * @param par arguments of the function (NULL no parameters)
     * @return a JSON response
     */
    chaos::common::data::CDWUniquePtr postData(const std::string&func,const chaos::common::data::CDataWrapper* par);
    
    /**
     * @brief Get the Data corresponding to the key
     * 
     * @param key dataset key
     * @param ptr buffer
     * @param maxsize maxbug size, if zero use the default for the requested data
     * @return int 0 on success
     */
    int getData(const std::string& key,void*ptr,int maxsize=0);

    DanteDriver();
    ~DanteDriver();
};
}
}

#endif /* defined(__DataImport__DanteDriver__) */
