/**
 * @file KeyValue.h
 * @author
 * @brief Naive implementation of key-value store, with no integrity check, transaction log,...
 * @version 0.1
 * @date 2022-05-12
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <map>
#include <string>
#include <optional>


/**
 * @brief Simple Key Value namespace.
 */
namespace skv {


/**
 * @brief Singleton key-value store.
 */
class KeyValue {
public:
    static KeyValue& instance();

    /**
     * @brief Search for a value by its key name.
     *
     * @param[in] key Key name.
     * @return Value of key if available, otherwise nothing.
     */
    std::optional<std::string> load(const std::string &key) const;

    /**
     * @brief Store key and value to persistent storage.
     *
     * @param[in] key   Key name
     * @param[in] value Value of key
     * @return True on success and False otherwise.
     */
    bool store(const std::string &key, const std::string &value);

    KeyValue(const KeyValue&)            = delete;
    KeyValue(KeyValue&&)                 = delete;
    KeyValue& operator=(const KeyValue&) = delete;
    KeyValue& operator=(KeyValue&&)      = delete;

    ~KeyValue();

private:
    KeyValue();

    static const std::string Location;

    std::map<std::string, std::string> kv;
};


} // End of namespace #skv
