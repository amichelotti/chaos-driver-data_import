/*
 *	RTDB.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *      Historize Dataset on DB
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
#ifndef _RTDB_h
#define _RTDB_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <common/vme/core/vmewrap.h>
#include <common/misc/data/core/DBbaseFactory.h>

#ifndef RTDBLAPP_
#define RTDBLAPP_		LAPP_ << "[RTDB] "
#define RTDBLDBG_		LDBG_ << "[RTDB] " << __PRETTY_FUNCTION__ << " "
#define RTDBLERR_		LERR_ << "[RTDB] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "
#endif

namespace driver {

namespace import {

class RTDB : public chaos::cu::control_manager::RTAbstractControlUnit {
	PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTDB)
public:

	    RTDB(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers);

	    		~RTDB(){
	    			unitDeinit();
	    		}


protected:

	    		int TTL;
	    		::common::misc::data::DataSet* history;
	    		::common::misc::data::DBbase* db;
public:

	    		void addAttributeToDataSet(const std::string& attribute_name,
	    				const std::string& attribute_description,
						chaos::DataType::DataType attribute_type,
						chaos::DataType::DataSetAttributeIOAttribute attribute_direction,bool historize,
						uint32_t maxSize = 0);

	    		virtual void unitDefineActionAndDataset() ;
	    		virtual void unitInit() ;
	    		virtual void unitStart() {}
	    		virtual void unitStop() {}
	    		virtual void unitRun() {}

	    		virtual void unitDeinit() ;
	    		virtual void unitStore();

};
}}
#endif
