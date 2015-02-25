//
//  DataImportDriverInterface.cpp
//  DataImport
//
//  Created by Claudio Bisegni on 25/02/15.
//
//

#include <driver/data_import/core/DataImportDriverInterface.h>

DataImportDriverInterface::DataImportDriverInterface(chaos::cu::driver_manager::driver::DriverAccessor*_accessor):
accessor(_accessor){
    assert (_accessor);
};

DataImportDriverInterface::~DataImportDriverInterface(){
    
}