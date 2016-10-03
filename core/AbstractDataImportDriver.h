/*
 *	AbstractDataImportDriver.h
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
    //update the data block from the subclass calling fetchData
    DataImportDriverOpcode_FETCH_NEW_DATABLOCK  = chaos::cu::driver_manager::driver::OpcodeType::OP_USER,
    //opcode to fetch data
    /*!
     cmd.parm[0]    //is the offset
     cmd.parm[1]   //is the lenght of data to read;
     cmd.resultDataLength //is set to the readed data lenght
     if( < lenght) {
     ADIDLERR_<<"Data buffer of the message have lower size of the requested command [offset:"<<offset<<" lenght:"<<lenght<<"]";
     } else {
     
     cmd->resultDataLength = lenght;
     */
	DataImportDriverOpcode_GET_DATA
} DataImportDriverOpcode;


/*
 This is the abstract base driver for all driver that will act as importer.
 The subclass need to fetch a DataPack from his transport layer. The abstraction will
 manage all interaction for fetch a data from this buffer nd forward it to the
 cliend side.
 */
class AbstractDataImportDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
    //is the buffer where the sublcass need to copy the while data pack
    char *buffer_data_block;
    uint32_t buffer_len;

    //read data from offset

protected:
    void driverInit(const char *initParameter) throw(chaos::CException);
    void driverDeinit() throw(chaos::CException);
    virtual int fetchData(void *buffer, unsigned int buffer_len)=0;
    virtual int readDataOffset(void* data_ptr, uint32_t offset, uint32_t lenght);
    //! expand the datapack memory buffer
    bool growMem(unsigned int new_mem_size);
public:
	AbstractDataImportDriver();
	~AbstractDataImportDriver();
    //! Execute a command
	chaos::cu::driver_manager::driver::MsgManagmentResultType::MsgManagmentResult
    execOpcode(chaos::cu::driver_manager::driver::DrvMsgPtr cmd);
};

#endif /* defined(__ControlUnitTest__DummyDriver__) */
