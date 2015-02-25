//
//  DataImportDriverInterface.h
//  DataImport
//
//  Created by Claudio Bisegni on 25/02/15.
//
//

#ifndef __DataImport__DataImportDriverInterface__
#define __DataImport__DataImportDriverInterface__

#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>

/*!
 Class that simplify the method call of the driver hiding the message system
 */
class DataImportDriverInterface {
    
protected:
    chaos::cu::driver_manager::driver::DrvMsg message;
    
    chaos::cu::driver_manager::driver::DriverAccessor* accessor;
public:
    
    DataImportDriverInterface(chaos::cu::driver_manager::driver::DriverAccessor*_accessor);
    ~DataImportDriverInterface();
};

#endif /* defined(__DataImport__DataImportDriverInterface__) */
