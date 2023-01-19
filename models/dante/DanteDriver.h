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

/*
 driver definition
 */
#include <common/crest/chaos_crest.h>

#include <driver/data-import/core/AttributeOffLen.h>
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(DanteDriver)

namespace driver{

namespace data_import{
    static const char* PROT_ERROR="protocol_error";

class DanteDriver:public MemcachedDataImporterDriver {
    
    std::string danteRestServer,username,password;
    std::string danteElement;
    //chaos_crest_handle_t crest_handle;
    //chaos::common::data::CDWUniquePtr dataset;
    ::driver::data_import::AttributeOffLenVec attribute_off_len_vec;
    ::driver::data_import::AttributeOffLenVec static_attribute_off_len_vec;

    std::map<const std::string,::driver::data_import::AttributeOffLen *> key2item[2];

protected:
    void updateProperties();
    /*!
     "server_url":["host:port",...]
     "data_keys":memcached key array
     "data_pack_len":the lenght of the intere datapack to read
     */
public:
    enum DSTYPE {
        DYNAMIC,
        STATIC
    };
 void driverInit(const char *initParameter) ;
    void driverDeinit() ;
   
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
     * @param typ choose if dynamic or static dataset
     * @param maxsize maxbug size, if zero use the default for the requested data
     * @return int 0 on success
     */
    int getData(const std::string& key,void*ptr,DSTYPE typ=DYNAMIC,int maxsize=0);
    chaos::common::data::CDWUniquePtr getDrvProperties();
    chaos::common::data::CDWUniquePtr getDataset(DSTYPE typ=DYNAMIC);
    // retrieve in in the keys requested
    int getData(chaos::common::data::CDataWrapper&in,DSTYPE typ=DYNAMIC);

   // bool dataHasChanged(const std::string& key="");
    int setDrvProperty(const std::string& key, const std::string& value);

    DanteDriver();
    ~DanteDriver();
};
}
}

#endif /* defined(__DataImport__DanteDriver__) */
