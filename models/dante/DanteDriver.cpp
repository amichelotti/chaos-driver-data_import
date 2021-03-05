/*
 *	DanteDriver.cpp
 *	!CHAOS
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

#define DanteDriverLAPP_ INFO_LOG(DanteDriver)
#define DanteDriverLDBG_ DBG_LOG(DanteDriver)
#define DanteDriverLERR_ ERR_LOG(DanteDriver)

#define DanteDriverLOG_MC_ERROR(x)                        \
  if (mc_result != MEMCACHED_SUCCESS) {                   \
    DanteDriverLERR_ << memcached_strerror(mc_client, x); \
  }

#include "DanteDriver.h"

#include <json/json.h>

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/utility/endianess.h>
#include <common/crest/chaos_crest.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

// GET_PLUGIN_CLASS_DEFINITION
// we need only to define the driver because we don't are makeing a plugin
using namespace chaos::common::data;
namespace driver {

namespace data_import {
// GET_PLUGIN_CLASS_DEFINITION
// we need to define the driver with alias version and a class that implement it
DanteDriver::DanteDriver()
    : maxUpdateRefresh(100) {
}

DanteDriver::~DanteDriver() {
  DEBUG_CODE(DanteDriverLDBG_ << "DEINIT MEMCACHED");
}
/**
 * @brief Initialize Driver
 * 
 * @param initParameter a JSON describing memcached server, dante rest server, and dataset to retrieve.
 */
void DanteDriver::driverInit(const char *initParameter) throw(
    chaos::CException) {
  MemcachedDataImporterDriver::driverInit(initParameter);
  // parse json string
  std::string  json_string(initParameter);
  Json::Reader json_reader;
  Json::Value  json_parameter;

  if (!json_reader.parse(json_string, json_parameter)) {
    throw chaos::CException(-1, "Json init paremeter has not been parsed", __PRETTY_FUNCTION__);
  }
  const Json::Value &json_dyn = json_parameter["dyn_ds"];
  if (json_dyn.isNull()) {
    DanteDriverLERR_ << " Dynamic dataset is not set 'dyn_ds'";
  } else {
    attribute_off_len_vec = ::driver::data_import::json2Attribute(json_dyn);
  }
  const Json::Value &json_sta = json_parameter["sta_ds"];
  if (json_sta.isNull()) {
    DanteDriverLERR_ << " Static dataset is not set 'sta_ds'";
  } else {
    static_attribute_off_len_vec = ::driver::data_import::json2Attribute(json_sta);
  }

  // fetch value from json document
  const Json::Value &json_server_urls = json_parameter["dante_server_url"];
  // check madatory data
  if (json_server_urls.isNull()) {
    throw chaos::CException(-2, "dante_server_url is mandatory", __PRETTY_FUNCTION__);
  }
  danteRestServer                 = json_server_urls.asString();
  const Json::Value &json_element = json_parameter["dante_element"];
  // check madatory data
  if (json_element.isNull()) {
    throw chaos::CException(-2, "dante_element is mandatory", __PRETTY_FUNCTION__);
  }
  danteElement                 = json_element.asString();
  const Json::Value &rest_user = json_parameter["dante_user"];
  if (!rest_user.isNull()) {
    username = rest_user.asString();
  }
  const Json::Value &rest_passwd = json_parameter["dante_password"];

  if (!rest_passwd.isNull()) {
    password = rest_passwd.asString();
  }
  for (::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
       it != attribute_off_len_vec.end();
       it++) {
    key2item[0][(*it)->name] = *it;
    if ((*it)->buffer == NULL) {
      (*it)->buffer = calloc(1, (*it)->len + (((*it)->type == chaos::DataType::TYPE_STRING) ? 1 : 0));
    }
  }

  for (::driver::data_import::AttributeOffLenIterator it = static_attribute_off_len_vec.begin();
       it != static_attribute_off_len_vec.end();
       it++) {
    key2item[1][(*it)->name] = *it;
    if ((*it)->buffer == NULL) {
      (*it)->buffer = calloc(1, (*it)->len + (((*it)->type == chaos::DataType::TYPE_STRING) ? 1 : 0));
    }
  }
  if (danteRestServer.size() > 0) {
    crest_handle = chaos_crest_open(danteRestServer.c_str());
  } else {
    crest_handle = NULL;
  }
}
void DanteDriver::updateProperties() {
  int err;
  if ((err = fetch()) != 0) {
    DanteDriverLERR_ << "ERROR fetching";
  }
  for (::driver::data_import::AttributeOffLenIterator it = static_attribute_off_len_vec.begin();
       it != static_attribute_off_len_vec.end();
       it++) {
    if ((err = readDataOffset((*it)->buffer, (*it)->keybind, (*it)->offset, (*it)->len))) {
      DanteDriverLERR_ << "Error reading attribute " << (*it)->name << " from driver with error " << err;
    }
    copy((*it)->buffer, *it);
  }

  for (::driver::data_import::AttributeOffLenIterator it = attribute_off_len_vec.begin();
       it != attribute_off_len_vec.end();
       it++) {
    if ((err = readDataOffset((*it)->buffer, (*it)->keybind, (*it)->offset, (*it)->len))) {
      DanteDriverLERR_ << "Error reading attribute " << (*it)->name << " from driver with error " << err;
    }
    copy((*it)->buffer, *it);
  }
}

chaos::common::data::CDWUniquePtr DanteDriver::getDrvProperties() {
  chaos::common::data::CDWUniquePtr res(new chaos::common::data::CDataWrapper());
  updateProperties();
  chaos::common::data::CDWUniquePtr dyn = attribute2CDW(attribute_off_len_vec);
  chaos::common::data::CDWUniquePtr sta = attribute2CDW(static_attribute_off_len_vec);

  if (dyn.get()) {
    res->addCSDataValue("DYN", *(dyn.get()));
  }
  if (sta.get()) {
    res->addCSDataValue("STA", *(sta.get()));
  }
  DanteDriverLDBG_ << " DYN:" << dyn->getCompliantJSONString() << " STA:" << sta->getJSONString() << " Returning " << res->getCompliantJSONString();

  return res;
}

int DanteDriver::setDrvProperty(const std::string &key, const std::string &value) {
  DanteDriverLERR_ << " Attributes are read only, cannot set:" << key << " to:" << value;
  return -1;
}

void DanteDriver::driverDeinit() throw(chaos::CException) {
  for (::driver::data_import::AttributeOffLenIterator i = attribute_off_len_vec.begin(); i != attribute_off_len_vec.end(); i++) {
    free((*i)->buffer);
    free((*i)->old_buffer);
    delete (*i);
  }
  MemcachedDataImporterDriver::driverDeinit();
}

chaos::common::data::CDWUniquePtr DanteDriver::postData(const std::string &func, const chaos::common::data::CDataWrapper *par) {
  std::string json_par = "{}";
  char        ansbuf[1024];
  memset(ansbuf, 0, sizeof(ansbuf));
  if (crest_handle == NULL) {
    return chaos::common::data::CDWUniquePtr();
  }
  chaos::common::data::CDWUniquePtr retv = CDWUniquePtr(new CDataWrapper());
  int                               ret;
  chaos::common::data::CDataWrapper protocol_error;
  std::stringstream                 full;
  full << "/element/" << danteElement << "/" << func;

  if (par) {
    json_par = par->getCompliantJSONString();
  }
  ret = ::http_post(crest_handle, full.str().c_str(), json_par.c_str(), json_par.size() + 1, ansbuf, sizeof(ansbuf));
  if (ret == 0) {
    try {
      DanteDriverLDBG_ << "Req:\"" << full.str() << " data:" << json_par << " answer:" << ansbuf;
      retv->setSerializedJsonData(ansbuf);
      DanteDriverLDBG_ << "\" Server Returned:" << ansbuf;
    } catch (...) {
      std::stringstream ss;
      ss << "REQ:\"" << full.str() << "\" ret:" << ret << " invalid JSON response:" << ansbuf;
      DanteDriverLERR_ << ss.str();
      protocol_error.addStringValue("msg", "invalid JSON response");
      retv->addCSDataValue(PROT_ERROR, protocol_error);
      DanteDriverLERR_ << "Invalid response" << retv->getCompliantJSONString();
    }
  } else {
    protocol_error.addStringValue("msg", "SERVER ERROR");
    protocol_error.addInt32Value("err", ret);

    retv->addCSDataValue(PROT_ERROR, protocol_error);
    DanteDriverLERR_ << "req:\"" << full.str() << "\""
                     << " Server ERROR returned:" << ret << " buf:" << ansbuf;
  }

  return retv;
}

chaos::common::data::CDWUniquePtr DanteDriver::getDataset(DSTYPE typ) {
  chaos::common::data::CDWUniquePtr                                               res(new CDataWrapper());
  std::map<const std::string, ::driver::data_import::AttributeOffLen *>::iterator k;
  int                                                                             indx = (typ == DYNAMIC) ? 0 : 1;
  int                                                                             err;
  for (k = key2item[indx].begin(); k != key2item[indx].end(); k++) {
    if ((err = readDataOffset(k->second->buffer, k->second->keybind, k->second->offset, k->second->len))) {
      DanteDriverLERR_ << "Error reading attribute " << k->first << "[" << k->second->keybind << "] from driver with error " << err;
    } else {
      /*  TYPE_BOOLEAN = 0,
            //!Integer 32 bit length
            TYPE_INT32,
            //!Integer 64 bit length
            TYPE_INT64,
            //!Double 64 bit length
            TYPE_DOUBLE,
            //!C string variable length
            TYPE_STRING,
            //!byte array variable length
            TYPE_BYTEARRAY,
            
            TYPE_CLUSTER,
            //!modifier to be ored to normal data types
            TYPE_ACCESS_ARRAY=0x100,
            */
      uint32_t tt = ((unsigned)k->second->type) & ((unsigned)chaos::DataType::TYPE_ACCESS_ARRAY);
      if (tt) {
        tt              = ((unsigned)k->second->type) & (~(unsigned)chaos::DataType::TYPE_ACCESS_ARRAY);
        k->second->type = (chaos::DataType::DataType)(tt);

        res->appendArray(k->first, k->second->type, (const char *)k->second->buffer, k->second->len);
       // DanteDriverLDBG_ << " VECTOR :" << k->first<<res->getJSONString();

      } else if (k->second->type == chaos::DataType::TYPE_BOOLEAN) {
        res->addBoolValue(k->first, *(bool *)k->second->buffer);
      } else if (k->second->type == chaos::DataType::TYPE_INT32) {
        res->addInt32Value(k->first, *(int32_t *)k->second->buffer);
      } else if (k->second->type == chaos::DataType::TYPE_DOUBLE) {
        res->addDoubleValue(k->first, *(double *)k->second->buffer);
      } else if (k->second->type == chaos::DataType::TYPE_INT64) {
        res->addInt64Value(k->first, *(int64_t *)k->second->buffer);
      } else if (k->second->type == chaos::DataType::TYPE_STRING) {
        std::string a((const char *)k->second->buffer);
        res->addStringValue(k->first, a);
      } else if (k->second->type == chaos::DataType::TYPE_BYTEARRAY) {
        res->addBinaryValue(k->first, (const char *)k->second->buffer, k->second->len);
      }
    }
  }
  return res;
}

int DanteDriver::getData(const std::string &key, void *ptr, DSTYPE typ, int maxsize) {
  int                                                                             err  = 0;
  int                                                                             indx = (typ == DYNAMIC) ? 0 : 1;
  std::map<const std::string, ::driver::data_import::AttributeOffLen *>::iterator k    = key2item[indx].find(key);
  if (k == key2item[indx].end()) {
    DanteDriverLERR_ << "Key " << key << " not found";
    return -1;
  }
  ::driver::data_import::AttributeOffLen *  it  = k->second;
  std::map<std::string, uint64_t>::iterator i   = last_fetch.find(it->keybind);
  uint64_t                                  now = chaos::common::utility::TimingUtil::getTimeStamp();
  if ((i == last_fetch.end()) || ((now - i->second) > maxUpdateRefresh)) {
    DanteDriverLDBG_ << "Fetching:" << it->keybind << " for:" << key;
    if ((err = fetch(it->keybind)) != 0) {
      DanteDriverLERR_ << "ERROR fetching:" << it->keybind;
      return err;
    }
    last_fetch[it->keybind] = now;
  }
  if ((err = readDataOffset(it->buffer, it->keybind, it->offset, it->len))) {
    DanteDriverLERR_ << "Error reading attribute " << it->name << " from driver with error " << err;
    return err;
  }
  if ((maxsize > 0) && (maxsize < it->len)) {
    DanteDriverLERR_ << "Error attribute " << it->name << " size of type: " << it->len << " bigger than allocated:" << maxsize;
    return -200;
  }
  copy(ptr, it);

  return err;
}
}  // namespace data_import
}  // namespace driver