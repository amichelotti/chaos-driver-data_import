/*
 *	DataImportDriverInterface.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
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
#include <driver/data-import/core/DataImportDriverInterface.h>
#include <driver/data-import/core/AbstractDataImportDriver.h>

#define DIDILAPP_	INFO_LOG(DataImportDriverInterface)
#define DIDILDBG_	DBG_LOG(DataImportDriverInterface)
#define DIDILERR_	ERR_LOG(DataImportDriverInterface)

DataImportDriverInterface::DataImportDriverInterface(chaos::cu::driver_manager::driver::DriverAccessor*_accessor):
accessor(_accessor){
    assert (_accessor);
    impl=(AbstractDataImportDriver*)_accessor->getImpl();
};

DataImportDriverInterface::~DataImportDriverInterface(){
    
}

#if 0
int DataImportDriverInterface::fetchNewDatablock() {
    int ret,ret2;
    message.opcode = DataImportDriverOpcode_FETCH_NEW_DATABLOCK;
    ret2=accessor->send(&message);
    ret=message.ret;
   // DEBUG_CODE(DIDILDBG_<<"fetchNewDatablock,func ret:"<<ret<<",accessor ret "<<ret2;)
    return ret;
}

int DataImportDriverInterface::readAttribute(void *attribute_ptr, int from, int len) {
    int ret,ret2;
    message.parm[0] = from;
    message.parm[1] = len;
    message.opcode = DataImportDriverOpcode_GET_DATA;
    message.resultData = attribute_ptr;
    ret2=accessor->send(&message);
    ret=message.ret;
    return ret;
}
int DataImportDriverInterface::readAttribute(void *attribute_ptr, const std::string& key,int from, int len){
    int ret,ret2;
    message.parm[0] = from;
    message.parm[1] = len;
    message.inputData=(void*)strdup(key.c_str());
    message.inputDataLength=key.size() +1;
    message.opcode = DataImportDriverOpcode_GET_KEY_DATA;
    message.resultData = attribute_ptr;
    ret2=accessor->send(&message);
    if(message.inputData){
        free(message.inputData);
    }
    ret=message.ret;
    return ret;
}
#else
int DataImportDriverInterface::fetchNewDatablock() {
    return impl->fetch();
}

int DataImportDriverInterface::readAttribute(void *attribute_ptr, int from, int len) {
    return impl->readDataOffset(attribute_ptr,from, len);
}
int DataImportDriverInterface::readAttribute(void *attribute_ptr, const std::string& key,int from, int len){
   return impl->readDataOffset(attribute_ptr,key,from, len);

}
#endif