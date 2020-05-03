/*
 *	FilePosixDataImporterDriver.cpp
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


#include <stdlib.h>
#include <string.h>
#include <common/debug/core/debug.h>
#define FilePosixDataImporterDriverLAPP_	INFO_LOG(FilePosixDataImporterDriver)
#define FilePosixDataImporterDriverLDBG_	DBG_LOG(FilePosixDataImporterDriver)
#define FilePosixDataImporterDriverLERR_	ERR_LOG(FilePosixDataImporterDriver)
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#define FilePosixDataImporterDriverLOG_MC_ERROR(x)\
if(mc_result != MEMCACHED_SUCCESS) {\
FilePosixDataImporterDriverLERR_ << memcached_strerror(mc_client, x);\
}

#include <common/misc/data/core/DataSet.h>
#include <driver/data-import/models/posixFile/FilePosixDataImporterDriver.h>

#include <json/json.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(FilePosixDataImporterDriver, 1.0.0, FilePosixDataImporterDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(FilePosixDataImporterDriver, server_url [array of strings like host:port])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(FilePosixDataImporterDriver, data_key [string])
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(FilePosixDataImporterDriver, data_pack_len [uint32_t])
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

/*
 *
 2. e- current [mA]
3. e+ current [mA]
4. e+ rate [Hz]
5. e- rate [Hz]
6. linac mode
7. number of e- bunches	#
8. e- bunch 1-32 word	bit 32-1
9. e- bunch 33-64 word	bit 32-1
10. e- bunch 65-96 word	bit 32-1
11. e- bunch 96-120 word bit 32-8
12. number of e+ bunche	#
13. e+ bunch 1-32 word	bit 32-1
14. e+ bunch 33-64 word	bit 32-1
15. e+ bunch 65-96 word	bit 32-1
16. e+ bunch 96-120 wor	bit 32-8
17. status e-
18. status e+
19. status DAFNE
20. fill number	#
21. e- lifetime
22. e+ lifetime
23. LumIP1 [cm-2 s-1]/1e28 --> SIDDHARTA Kaon Monitor [Hz] Jenuary 2008
24. LumIP2 [cm-2 s-1]/1e28 --> LUMI Bhabha Monitor [Hz] Jannuary 2008
25. interaction flag #
26. RF frequency [Hz]
27. Roundness e- [Ey/Ex]
28. Roundness e+ [Ey/Ex]
29. KLOE field	 [Gauss]
30. BTF Energy [MeV]
31. BTF Ploarity [-1=e+, +1=e-]
32. BTF Target Line [1 bend line, 0 straght line]
33. WID e- [mm]
34. wid e- [mm]
35. WID e+ [mm]
36. wid e+ [mm]
37. X BPMEL204	[mm]
38. Y BPMEL204	[mm]
39. X BPMEL205	[mm]
40. Y BPMEL205	[mm]
41. X BPMEL206	[mm]
42. Y BPMEL206	[mm]
43. X BPMEL207	[mm]
44. Y BPMEL207	[mm]
45. temperature I1001 [C]
46. temperature I1002 [C]
47. temperature I1003 [C]
48. temperature I1004 [C]
49. X BPMEL202	[mm]
50. Y BPMEL202	[mm]
51. X BPMEL203	[mm]
52. Y BPMEL203	[mm]
 */
//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
FilePosixDataImporterDriver::FilePosixDataImporterDriver(){
	size = 0;
	buf=0;
	last_hash=-1;
    timeDependentName=false;
	allLines=false;

}

FilePosixDataImporterDriver::~FilePosixDataImporterDriver() {
   driverDeinit();
}

void FilePosixDataImporterDriver::driverInit(const char *initParameter) throw(chaos::CException) {
    Json::Value						json_parameter;
    Json::StyledWriter				json_writer;
    Json::Reader					json_reader;
    
    AbstractDataImportDriver::driverInit(initParameter);
    //parse json string
    std::string json_string(initParameter);
    if(!json_reader.parse(json_string, json_parameter)) {
    	ERR("missing JSON initialization");
        throw chaos::CException(-1, "Json init parameter has not been parsed", __PRETTY_FUNCTION__);
    }
    
    DPRINT("json init :%s",json_string.c_str());
    
    //fetch value from json document
    const Json::Value& filename= json_parameter["file_name"];
	const Json::Value& filename_time= json_parameter["file_time_name"];
	const Json::Value& all_lines= json_parameter["all_line"];

    const Json::Value& sseparator = json_parameter["separator"];
    
    if(sseparator.isNull()){
    	*separator=0;
    } else {
    	std::string s=sseparator.asString();
    	std::strncpy(separator, s.c_str(),2);

    }
    if(all_lines.isBool()){
		allLines=all_lines.asBool();
	}
    
    //check madatory data
    if (filename.isNull()) {
		if(filename_time.isNull()){
        	throw chaos::CException(-2, "file name is mandatory", __PRETTY_FUNCTION__);
		} else {
			fileName=filename_time.asString();
			timeDependentName=true;

		}
    } else {
		fileName=filename.asString();
	}

    
    DPRINT("File name %s, separator :'%c'",fileName.c_str(),separator);


}

void FilePosixDataImporterDriver::driverDeinit() throw(chaos::CException) {
	if(buf){
		free(buf);
		buf=NULL;
		size =0;
	}


}

int FilePosixDataImporterDriver::fetchData(void *buffer, unsigned int buffer_len,const std::string key) {

	std::ifstream file;
	if(timeDependentName){
		time_t rawtime;
  		struct tm * timeinfo;
  		char buffer [512];
		time (&rawtime);
  		timeinfo = localtime (&rawtime);

  		strftime (buffer,sizeof(buffer),fileName.c_str(),timeinfo);
		file.open(buffer,std::ios::in);
		FilePosixDataImporterDriverLDBG_ << "Reading '"<<buffer<<"'";	
  
	} else {
		file.open(fileName.c_str(),std::ios::in);
		FilePosixDataImporterDriverLDBG_ << "Reading '"<<fileName.c_str()<<"'";
	}

	pnt=NULL;
	
	if(file.is_open()){
	  //got to end
	  file.seekg(0,file.end);
	  size = file.tellg();
	  file.seekg(0,file.beg);
	  
	  if(size>0){
	    std::size_t h=0;
		if(allLines){
	    	buf = (char*)realloc(buf,size+1);
			buf[size]=0;
		
	    	file.read(buf,size);
		} else {
			std::string lastLine,tmp;            
			while(!file.eof()){
				getline(file,tmp);
				if(tmp.size()>1){
					lastLine=tmp;
				} 
			}
			/*
 			file.seekg(0,file.end);
			 bool keepLooping = true;
        		while(keepLooping) {
            		char ch;
            		file.get(ch);                            // Get current byte's data

            		if((int)file.tellg() <= 1) {             // If the data was at or before the 0th byte
                		file.seekg(0);                       // The first line is the last line
                		keepLooping = false;                // So stop there
           			} else if(ch == '\n') {                   // If the data was a newline
                		keepLooping = false;                // Stop at the current position.
					} else {                                  // If the data was neither a newline nor at the 0 byte
               		 file.seekg(-2,file.cur);        // Move to the front of that data, then to the front of the data before it
            		}
       			}

        	
        	getline(file,lastLine); 
 			buf = (char*)realloc(buf,lastLine.size()+1);
			*/
			buf = (char*)realloc(buf,lastLine.size()+1);
			strncpy(buf,lastLine.c_str(),lastLine.size()+1);
			buf[lastLine.size()]=0;
			DPRINT("tail:'%s'",lastLine.c_str());

		}
	    last_hash= current_hash;
	    current_hash =::common::misc::data::simpleHash(buf,size);
	    
	    DPRINT("read tail %d bytes, hash 0x%lx, last_hash 0x%lx",size,current_hash,last_hash);
	    
	  } else {
	    ERR("file is empty %d",size);
	    return DATA_IMPORT_NO_DATA;
	  }
	  
	  file.close();
	  if(current_hash==last_hash){
	    // not changed
	    DPRINT("not changed last hash 0x%lx",last_hash);
	    return DATA_IMPORT_NO_CHANGE;
	  }
	} else {
	  ERR("cannot open file %s",fileName.c_str());
	  return DATA_IMPORT_CANNOT_ACCESS_DATA;
	}
	return 0;
}


int FilePosixDataImporterDriver::readDataOffset(void* data_ptr, const std::string &key, uint32_t offset, uint32_t lenght){
	if(*separator==0){
		// no separator, treats as binary;
		if(lenght>=size){
			ERR("requested length %d is invalid, tot available is %d",lenght,size);
			return -1;
		}
		memcpy(data_ptr,buf+offset,lenght);
		DPRINT("binary copy from off %d, lenght %d",offset,lenght);
		return 0;
	} else {
		DPRINT("parsing buffer \"%s\"",buf);
		// treats as string
		if(pnt==NULL){
			pnt=strtok(buf,separator);
		} else {
			pnt=strtok(NULL,separator);

		}
		if(pnt==NULL){
			ERR("no other parameters")
			return DATA_IMPORT_DECODE_ERROR;
		}
		DPRINT("get token \"%s\"",pnt);

		if(strchr(pnt,'.')){

			// contain . is a double
			if(lenght==sizeof(float)){
				sscanf(pnt,"%f",(float*)data_ptr);
				DPRINT("parsed float:%f",*(float*)data_ptr);
				data_results[key]=true;

			} else {
				sscanf(pnt,"%lf",(double*)data_ptr);
				DPRINT("parsed double:%lf",*(double*)data_ptr);
				data_results[key]=true;

			}
			return 0;
		} else {
			if(lenght==sizeof(int64_t)){
				int64_t temp=strtoll(pnt,0,offset);
				memcpy(data_ptr,(void*)&temp,lenght);
				DPRINT("parsed int64_t : (double) %lf [0x%lx] base:%ld",*(double*)data_ptr,temp,temp,offset);
				data_results[key]=true;
				return 0;
			} else if(lenght == sizeof(int32_t)){
				int32_t temp=strtol(pnt,0,offset);
				memcpy(data_ptr,(void*)&temp,lenght);
				DPRINT("parsed int32_t :%f [0x%x] base:%d",*(double*)data_ptr,temp,temp,offset);
				data_results[key]=true;

				return 0;
			}
		}
	}
	ERR("error parsing");
	return DATA_IMPORT_DECODE_ERROR;
}



