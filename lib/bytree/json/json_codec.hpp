#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

namespace bytree {

class JsonCodec {
 public:
  JsonCodec() : d_() {
    d_.Parse("{}");
  }

  std::string Stringify() {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    d_.Accept(writer);
    return sb.GetString();
  }

  void Parse(const char* json) {
    // try catch
    d_.Parse(json);
  }

  JsonCodec& Add(std::string key, int value) {
    // key not empty
    rapidjson::Value k;
    k.SetString(key.c_str(),
                static_cast<rapidjson::SizeType>(key.size()),
                d_.GetAllocator());
    rapidjson::Value v;
    v.SetInt(value);
    d_.AddMember(k, v, d_.GetAllocator());
    return *this;
  }

  JsonCodec& Add(std::string key, const char* value) {
    rapidjson::Value k;
    k.SetString(key.c_str(),
                static_cast<rapidjson::SizeType>(key.size()),
                d_.GetAllocator());
    rapidjson::Value v;
    v.SetString(value,
                static_cast<rapidjson::SizeType>(strlen(value)),
                d_.GetAllocator());
    d_.AddMember(k, v, d_.GetAllocator());
    return *this;
  }

  JsonCodec& Add(std::string key, std::string value) {
    Add(key, value.c_str());
    return *this;
  }

  JsonCodec& Add(std::string key, std::vector<int>& vec) {
    rapidjson::Value k;
    k.SetString(key.c_str(),
                static_cast<rapidjson::SizeType>(key.size()),
                d_.GetAllocator());
    rapidjson::Value v;
    v.SetArray();
    for (int i = 0, size = vec.size(); i < size; i ++) {
      rapidjson::Value element;
      element.SetInt(vec[i]);
      v.PushBack(element, d_.GetAllocator());
    }
    d_.AddMember(k, v, d_.GetAllocator());
    return *this;
  }

  JsonCodec& Add(std::string key, std::vector<std::string>& vec) {
    rapidjson::Value k;
    k.SetString(key.c_str(),
                static_cast<rapidjson::SizeType>(key.size()),
                d_.GetAllocator());
    rapidjson::Value v;
    v.SetArray();
    for (int i = 0, size = vec.size(); i < size; i ++) {
      rapidjson::Value element;
      element.SetString(vec[i].c_str(),
                        static_cast<rapidjson::SizeType>(vec[i].size()),
                        d_.GetAllocator());
      v.PushBack(element, d_.GetAllocator());
    }
    d_.AddMember(k, v, d_.GetAllocator());
    return *this;
  }

  int GetInt(std::string key) {
    return d_[key.c_str()].GetInt();
  }
  std::string GetString(std::string key) {
    return d_[key.c_str()].GetString();
  }

 private:
  rapidjson::Document d_;
};
}

