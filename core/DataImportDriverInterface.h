/*
 *	DataImportDriverInterface.h
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

#ifndef __DataImport__DataImportDriverInterface__
#define __DataImport__DataImportDriverInterface__

#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverAccessor.h>
#include "AbstractDataImportDriver.h"

/*!
 Class that simplify the method call of the driver hiding the message system
 */
class DataImportDriverInterface: public AbstractDataImportDriver {
    
protected:
    chaos::cu::driver_manager::driver::DrvMsg message;
    
    chaos::cu::driver_manager::driver::DriverAccessor* accessor;
    AbstractDataImportDriver*impl;

public:
    
    DataImportDriverInterface(chaos::cu::driver_manager::driver::DriverAccessor*_accessor);
    ~DataImportDriverInterface();
    
    int fetchNewDatablock();
    int readAttribute(void *attribute_ptr, int from, int len);
    int readAttribute(void *attribute_ptr, const std::string& key,int from, int len);

};

#endif /* defined(__DataImport__DataImportDriverInterface__) */
