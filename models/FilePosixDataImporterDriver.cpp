/*
 *	FilePosixDataImporterDriver.cpp
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
#include <stdlib.h>
#include <string.h>
#include <common/debug/core/debug.h>
#define FilePosixDataImporterDriverLAPP_	INFO_LOG(FilePosixDataImporterDriver)
#define FilePosixDataImporterDriverLDBG_	DBG_LOG(FilePosixDataImporterDriver)
#define FilePosixDataImporterDriverLERR_	ERR_LOG(FilePosixDataImporterDriver)
#include <iostream>
#include <fstream>

#define FilePosixDataImporterDriverLOG_MC_ERROR(x)\
if(mc_result != MEMCACHED_SUCCESS) {\
FilePosixDataImporterDriverLERR_ << memcached_strerror(mc_client, x);\
}

#include <driver/data-import/models/FilePosixDataImporterDriver.h>

#include <json/json.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(FilePosixDataImporterDriver, 1.0.0, FilePosixDataImporterDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(FilePosixDataImporterDriver, server_url [array of strings like host:port])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(FilePosixDataImporterDriver, data_key [string])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(FilePosixDataImporterDriver, data_pack_len [uint32_t])
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION


//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
FilePosixDataImporterDriver::FilePosixDataImporterDriver(){
	size = 0;
	buf=0;

}

FilePosixDataImporterDriver::~FilePosixDataImporterDriver() {
   driverDeinit();
}

void FilePosixDataImporterDriver::driverInit(const char *initParameter) throw(chaos::CException) {
    Json::Value						json_parameter;
    Json::StyledWriter				json_writer;
    Json::Reader					json_reader;
    
    AbstractDataImportDriver::driverInit(initParameter);
    //parse json string
    std::string json_string(initParameter);
    if(!json_reader.parse(json_string, json_parameter)) {
    	ERR("missing JSON initialization");
        throw chaos::CException(-1, "Json init parameter has not been parsed", __PRETTY_FUNCTION__);
    }
    
    DPRINT("json init :%s",json_string.c_str());
    
    //fetch value from json document
    const Json::Value& filename= json_parameter["file_name"];
    const Json::Value& sseparator = json_parameter["separator"];
    
    if(sseparator.isNull()){
    	*separator=0;
    } else {
    	std::string s=sseparator.asString();
    	std::strncpy(separator, s.c_str(),2);

    }
    
    
    //check madatory data
    if (filename.isNull()) {
        throw chaos::CException(-2, "file name is mandatory", __PRETTY_FUNCTION__);
    }

    fileName=filename.asString();
	DPRINT("File name %s, separator :'%c'",fileName.c_str(),separator);


}

void FilePosixDataImporterDriver::driverDeinit() throw(chaos::CException) {

}

int FilePosixDataImporterDriver::fetchData(void *buffer, unsigned int buffer_len) {

	std::ifstream file(fileName.c_str(),std::ios::in);
	pnt=NULL;

	if(file.is_open()){
		size = file.tellg();
		if(size>0){

			buf = (char*)realloc(buf,size);
			file.read(buf,size);
			DPRINT("read %d bytes",size);

		} else {
			ERR("file is empty ");
			return -1;
		}

		file.close();
	} else {
		ERR("cannot open file");
		return -3;
	}
return 0;
}

int FilePosixDataImporterDriver::readDataOffset(void* data_ptr, uint32_t offset, uint32_t lenght){
	if(*separator==0){
		// no separator, treats as binary;
		if(lenght>=size){
			ERR("requested length %d is invalid, tot available is %d",lenght,size);
			return -1;
		}
		memcpy(data_ptr,buf+offset,lenght);
		DPRINT("binary copy from off %d, lenght %d",offset,lenght);
		return 0;
	} else {
		// treats as string
		if(pnt==NULL){
			pnt=strtok(buf,separator);
		} else {
			pnt=strtok(NULL,separator);

		}
		if(pnt==NULL){
			ERR("no other parameters")
			return -2;
		}
		if(strchr(pnt,'.')){
			// contain . is a double
			if(lenght==sizeof(float)){
				sscanf(pnt,"%f",data_ptr);
				DPRINT("parsed float:%f",*(float*)data_ptr);
			} else {
				sscanf(pnt,"%lf",data_ptr);
				DPRINT("parsed double:%lf",*(double*)data_ptr);

			}
			return 0;
		} else {
			if(lenght==sizeof(int64_t)){
				int64_t temp=strtoll(pnt,0,offset);
				memcpy(data_ptr,(void*)&temp,lenght);
				DPRINT("parsed int64_t :%lld [0x%llx] base:%d",*(double*)data_ptr,temp,temp,offset);
				return 0;
			} else if(lenght == sizeof(int32_t)){
				int32_t temp=strtol(pnt,0,offset);
				memcpy(data_ptr,(void*)&temp,lenght);
				DPRINT("parsed int32_t :%d [0xlx] base:%d",*(double*)data_ptr,temp,temp,offset);
				return 0;
			}
		}
	}
	ERR("error parsing");
	return -100;
}



