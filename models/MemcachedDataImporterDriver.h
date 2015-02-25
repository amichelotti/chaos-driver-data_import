//
//  MemcachedDataImporterDriver.h
//  DataImport
//
//  Created by Claudio Bisegni on 25/02/15.
//
//

#ifndef __data_import__MemcachedDataImporterDriver__
#define __data_import__MemcachedDataImporterDriver__

#include <driver/data_import/core/AbstractDataImportDriver.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <chaos/common/data/DatasetDB.h>


DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(MemcachedDataImporterDriver)
/*
 driver definition
 */
class MemcachedDataImporterDriver:
public AbstractDataImportDriver {
protected:
    int fetchDataBuffer(void *buffer, unsigned int buffer_len);
public:
    MemcachedDataImporterDriver();
    ~MemcachedDataImporterDriver();
};
#endif /* defined(__DataImport__MemcachedDataImporterDriver__) */
