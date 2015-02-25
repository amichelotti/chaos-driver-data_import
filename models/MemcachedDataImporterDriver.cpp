//
//  MemcachedDataImporterDriver.cpp
//  DataImport
//
//  Created by Claudio Bisegni on 25/02/15.
//
//


#define MemcachedDataImporterDriverLAPP_	INFO_LOG(MemcachedDataImporterDriver)
#define MemcachedDataImporterDriverLDBG_	DBG_LOG(MemcachedDataImporterDriver)
#define MemcachedDataImporterDriverLERR_	ERR_LOG(MemcachedDataImporterDriver)

#include <driver/data_import/models/MemcachedDataImporterDriver.h>

//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(MemcachedDataImporterDriver, 1.0.0,MemcachedDataImporterDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(MemcachedDataImporterDriver, http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION


//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
MemcachedDataImporterDriver::MemcachedDataImporterDriver(){
}

MemcachedDataImporterDriver::~MemcachedDataImporterDriver(){
}

int MemcachedDataImporterDriver::fetchDataBuffer(void *buffer, unsigned int buffer_len) {
    int err = 0;
    return err;
}