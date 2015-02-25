/*
 *	DummyDriver.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#ifndef ChaosControlUnitDriver_DataImport_h
#define ChaosControlUnitDriver_DataImport_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>


typedef enum DataImportDriverOpcode {
    //opcode to fetch data
	DataImportDriverOpcode_GET_DATA = chaos::cu::driver_manager::driver::OpcodeType::OP_USER
} DataImportDriverOpcode;


/*
 This is the abstract base driver for all driver that will act as importer.
 The subclass need to fetch a DataPack from his transport layer. The abstraction will
 manage all interaction for fetch a data from this buffer nd forward it to the
 cliend side.
 */
class AbstractDataImportDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
    //is the buffer where the sublcass need to copy the while data pack
    void *buffer_data_block;
	void driverInit(const char *initParameter) throw(chaos::CException);
	void driverDeinit() throw(chaos::CException);
    
    void growMem(unsigned int new_mem_size);
protected:
    virtual int fetchDataBuffer(void *buffer, unsigned int buffer_len)=0;
public:
	AbstractDataImportDriver();
	~AbstractDataImportDriver();
    //! Execute a command
	cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);

};

#endif /* defined(__ControlUnitTest__DummyDriver__) */
