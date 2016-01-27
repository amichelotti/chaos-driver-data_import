/*
 *	DataImport.h
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

#ifndef ChaosRTControlUnit_DataImport_h
#define ChaosRTControlUnit_DataImport_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <driver/data-import/core/DataImportDriverInterface.h>

struct AttributeOffLen {
    uint32_t                    index;
    std::string                 name;
    chaos::DataType::DataType   type;
    uint32_t                    offset;
    uint32_t                    len;
    int                         lbe;//-1 no lbe, 0-little, 1-big
    void                        *buffer;
};

typedef std::vector<AttributeOffLen*>               AttrbiuteOffLenVec;
typedef std::vector<AttributeOffLen*>::iterator     AttrbiuteOffLenVecIterator;

class DataImport:
public chaos::cu::control_manager::RTAbstractControlUnit {
    PUBLISHABLE_CONTROL_UNIT_INTERFACE(DataImport)
    
    std::vector<AttributeOffLen*> attribute_off_len_vec;
    
    DataImportDriverInterface *driver_interface;
public:
    /*!
     Construct a new CU with full constructor
     */
    DataImport(const std::string& _control_unit_id,
               const std::string& _control_unit_param,
               const ControlUnitDriverList& _control_unit_drivers);
    /*!
     Destructor a new CU
     */
    ~DataImport();
    
    int decodeType(const std::string& str_type,
                   chaos::DataType::DataType& attribute_type);
    
protected:
    /*!
     read the json control unit load parameter need to create the dataset form the data to import.
     The json format is like the template below:
     {
     -the name of the attribute
     "name": string,
     
     -the description fo the attribute
     "description": string,
     
     -the type of the attribute as: (u)int32, (u)int64, double, string, binary, boolean
     "type": string.
     
     -the offset is mandatory and represent the start of the value within a datablock
     "offset": int
     
     -the len is mandatory and represent how much data need to be copied
     "len": int
     
     -the maximum size of the attribute used only with string or binary types
     "max_size": int
     }]
     }
     */
    void unitDefineActionAndDataset()throw(chaos::CException);
    /*!(Optional)
     Define the Control Unit custom attribute
     */
    void unitDefineCustomAttribute();
    /*!(Optional)
     Initialize the Control Unit and all driver, with received param from MetadataServer
     */
    void unitInit() throw(chaos::CException);
    /*!(Optional)
     Execute the work, this is called with a determinated delay
     */
    void unitStart() throw(chaos::CException);
    /*!
     Execute the work, this is called with a determinated delay, it must be as fast as possible
     */
    void unitRun() throw(chaos::CException);
    
    /*!(Optional)
     The Control Unit will be stopped
     */
    void unitStop() throw(chaos::CException);
    
    /*!(Optional)
     The Control Unit will be deinitialized and disposed
     */
    void unitDeinit() throw(chaos::CException);
    
    //! Pre input attribute change
    /*!(Optional)
     This handler is called befor the update of the
     cached input attribute with the requested valure
     */
    void unitInputAttributePreChangeHandler() throw(chaos::CException);
    
    //! Handler called on the update of one or more input attribute
    /*!(Optional)
     After an input attribute has been chacnged this handler
     is called
     */
    void unitInputAttributeChangedHandler() throw(chaos::CException);
};

#endif
