/*
 *	RTDB.cpp
 *	!CHAOS
 *	Created automatically
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

#include <driver/data-import/core/RTDB.h>


using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;

using namespace ::driver::import;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(RTDB)

#define DI_CUSTOM_HEAD "[" << getCUID() << " - " << getCUInstance() << "] - "
#define DILAPP_	INFO_LOG(RTDB) << DI_CUSTOM_HEAD
#define DILDBG_	DBG_LOG(RTDB) << DI_CUSTOM_HEAD
#define DILERR_	ERR_LOG(RTDB) << DI_CUSTOM_HEAD


RTDB::RTDB(const string& _control_unit_id,
		const string& _control_unit_param,
		const ControlUnitDriverList& _control_unit_drivers):
		RTAbstractControlUnit(_control_unit_id,
				_control_unit_param,
				_control_unit_drivers){
	db = NULL;
	history = NULL;
}



void RTDB::unitDefineActionAndDataset() throw(chaos::CException) {
	//insert your definition code here
	chaos::cu::control_manager::RTAbstractControlUnit::addAttributeToDataSet("TTL",
			"Time To Live",
			chaos::DataType::TYPE_INT32,
			chaos::DataType::Input);

	chaos::cu::control_manager::RTAbstractControlUnit::addAttributeToDataSet("DBTYPE",
			"DB name",
			chaos::DataType::TYPE_STRING,
			chaos::DataType::Input);

	chaos::cu::control_manager::RTAbstractControlUnit::addAttributeToDataSet("DBNAME",
			"DB name",
			chaos::DataType::TYPE_STRING,
			chaos::DataType::Input);

	chaos::cu::control_manager::RTAbstractControlUnit::addAttributeToDataSet("DBSERVER",
			"DB server",
			chaos::DataType::TYPE_STRING,
			chaos::DataType::Input);


}




//!Initialize the Custom Control Unit
void RTDB::unitInit() throw(chaos::CException) {

	int ret;
	std::string type=std::string("RTDB_")+getCUID();
	history = new ::common::misc::data::DataSet(getCUID(),getCUID());
	std::vector< ::common::misc::data::DataSet::DatasetElement_psh> elems=history->getDataSet();
	for(std::vector< ::common::misc::data::DataSet::DatasetElement_psh>::iterator i=elems.begin();i!=elems.end();i++){
		void *pnt =getAttributeCache()->getRWPtr<void*>(chaos::common::data::cache::DOMAIN_OUTPUT, (*i)->name);
		void *pntro =(void*)getAttributeCache()->getROPtr<void*>(chaos::common::data::cache::DOMAIN_INPUT, (*i)->name);
		if(pnt){
			RTDBLDBG_ << "adding OUTPUT attribute "<<(*i)->name<<" to history";
			history->set((*i)->name,pnt,(*i)->size);
		}
		if(pntro){
			RTDBLDBG_ << "adding INPUT attribute "<<(*i)->name<<" to history";

			history->set((*i)->name,pntro,(*i)->size);
		}
	}



	std::string name=getAttributeCache()->getValue<std::string>(DOMAIN_INPUT,"DBNAME");
	std::string server=getAttributeCache()->getValue<std::string>(DOMAIN_INPUT,"DBSERVER");
	std::string tt=getAttributeCache()->getValue<std::string>(DOMAIN_INPUT,"DBTYPE");

	db =::common::misc::data::DBbaseFactory::getInstance(tt,name);

	db->addDBServer(server);
	if((ret=db->connect())!=0){
		throw chaos::CException(ret,"cannot connect to DB "+ name + " server:"+server,__PRETTY_FUNCTION__);
	}

}

void RTDB::addAttributeToDataSet(const std::string& attribute_name,
		const std::string& attribute_description,
		chaos::DataType::DataType attribute_type,
		chaos::DataType::DataSetAttributeIOAttribute attribute_direction,bool historize,
		uint32_t maxSize){
	if(historize){
		history->add(attribute_name,(::common::misc::data::dataTypes)attribute_type,(void*)0,maxSize);
	}
	chaos::cu::control_manager::RTAbstractControlUnit::addAttributeToDataSet(attribute_name,
			attribute_description,
			attribute_type,
			attribute_direction,maxSize);

}
//!Deinit the Control Unit
void RTDB::unitDeinit() throw(chaos::CException) {
	db->disconnect();
}

//! pre imput attribute change
void RTDB::unitStore(){
	db->pushData(*history);
}
