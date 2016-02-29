#include <iostream>
#include <string>
#include "json_util.h"

using namespace std;
using namespace bytree;

int main() {
  JsonCodec codec;
  codec.Add("int", 10);
  codec.Add("string", "bytree");
  cout << codec.Stringify() << endl;

  JsonCodec codec2;
  codec2.Parse(" { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ");
  cout << codec2.Stringify() << endl;
  cout << codec2.GetString("hello") << endl;
  cout << codec2.GetInt("i") << endl;

  JsonCodec codec3;
  int arr[] = {1, 2, 3};
  vector<int> vec(arr, arr+sizeof(arr)/sizeof(int));
  codec3.Add("array", vec);
  cout << codec3.Stringify() << endl;

  return 0;
}
