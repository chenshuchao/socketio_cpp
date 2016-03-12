#include <iostream>
#include <string>
#include <json/json.h>

using namespace std;

int main() {
  //string strJ("{\"ha\" : \"la\", \"shuchao\": \"10\"}");
  string strJ("[\"chat\", \"lalaalla\"]");
  Json::Reader reader;
  Json::Value root;
  if(!reader.parse(strJ,root)){
    cout << "error" << endl;
    return -1;
  }
  //cout << root["shuchao"] << endl; 
  int size = root.size();
  for(int i=0; i<size; ++i)
  {
    cout << root[i] << endl;
  }
}
