/*
 *	FilePosixDataImporterDriver.h
 *	!CHAOS
 *	Created by Andrea Michelotti
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


#ifndef __data_import__FilePosixDataImporterDriver__
#define __data_import__FilePosixDataImporterDriver__

#include <driver/data-import/core/AbstractDataImportDriver.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <chaos/common/data/DatasetDB.h>

#include <string>

#include <boost/functional/hash.hpp>
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(FilePosixDataImporterDriver)
/*
 driver definition
 */
class FilePosixDataImporterDriver:
public AbstractDataImportDriver {

    char separator[2];
    std::string fileName;
    char*buf,*pnt;
    int size;
    std::size_t last_hash,current_hash;
    bool allLines; // reads from begain or tail

protected:
    bool timeDependentName;
    /*!
     "server_url":["host:port",...]
     "data_key":memcached string key
     "data_pack_len":the lenght of the intere datapack to read
     */
    void driverInit(const char *initParameter) throw(chaos::CException);
    void driverDeinit() throw(chaos::CException);
    int  fetchData(void *buffer, unsigned int buffer_len);
    int readDataOffset(void* data_ptr, const std::string &key, uint32_t offset, uint32_t lenght);
public:
    FilePosixDataImporterDriver();
    ~FilePosixDataImporterDriver();
};
#endif /* defined(__DataImport__FilePosixDataImporterDriver__) */
