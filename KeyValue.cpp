#include "KeyValue.h"

#include <fstream>

using std::ifstream;
using std::ofstream;
using std::string;
using std::optional;


namespace skv {

const string KeyValue::Location("./STM.kv");


KeyValue& KeyValue::instance() {
    static KeyValue keyValue;

    return keyValue;
}


optional<string> KeyValue::load(const std::string &key) const {
    auto it = kv.find(key);

    if (it == kv.end()) return {};
    return it->second;
}


bool KeyValue::store(const string &key, const string &value) {
    kv[key] = value;

    ofstream ofs(Location, ofstream::out | ofstream::trunc);
    if (!ofs) return false;

    for (const auto &e: kv) {
        ofs << e.first << ':' << e.second << '\n';
    }
    ofs << '\n';

    return true;
}


KeyValue::KeyValue() {
    ifstream ifs(Location);

    string line;
    while (getline(ifs, line)) {
        auto pos = line.find_first_of(':');
        if (pos > 0 && pos != string::npos) {
            auto key   = line.substr(0, pos);
            auto value = line.substr(pos+1);

            kv[key] = value;
        }
    }
}


// I don't even bother to store key-value pairs on destruction.
KeyValue::~KeyValue() {}


}
