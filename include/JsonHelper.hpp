#ifndef JSONHELPER_HPP
#define JSONHELPER_HPP

// https://takap-tech.com/entry/2019/10/24/235423
// JsonHelper.hpp

#include "picojson.h"
#include <functional>

/**
 * picojson用の書き込み時に使うヘルパークラス
 */
class Json
{
    picojson::object obj;

public:

    operator picojson::object() { return this->obj; }
    operator picojson::value() { return picojson::value(this->obj); }

    picojson::object getObject() { return *this; }
    picojson::value toValue() { return *this; }

    inline void add(const std::string& key, const int value)
    {
        Json::add(this->obj, key, (double)value);
    }
    inline void add(const std::string& key, const unsigned int value)
    {
        Json::add(this->obj, key, (double)value);
    }
    inline void add(const std::string& key, const float value)
    {
        Json::add(this->obj, key, (double)value);
    }
    inline void add(const std::string& key, const bool value)
    {
        this->obj.insert(std::make_pair(key.c_str(), picojson::value(value)));
    }
    inline void addStr(const std::string& key, const std::string& value)
    {
        this->obj.insert(std::make_pair(key.c_str(), picojson::value(value)));
    }
    inline void add(const std::string& key, Json child)
    {
        this->obj.insert(std::make_pair(key.c_str(), child.toValue()));
    }
    
    template<class T>
    void add(const std::string& key, T value, const std::function<Json(T)>& convert)
    {
        this->obj.insert(std::make_pair(key.c_str(), convert(value).getValue()));
    }
    
    template<class T>
    void add(const std::string& key, const std::vector<T>& list, const std::function<Json(T)>& convert)
    {
        picojson::array jsonList;
        for (const auto& item : list)
        {
            jsonList.push_back(convert(item).toValue());
        }

        obj.insert(std::make_pair(key.c_str(), picojson::value(jsonList)));
    }

    std::string serialize()
    {
        return this->toValue().serialize();
    }

    // オブジェクトに値を追加する
    inline static void add(picojson::object& obj, const std::string& key, double value)
    {
        obj.insert(std::make_pair(key.c_str(), value));
    }
};

#endif /* JSONHELPER_HPP */
