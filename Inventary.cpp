#include "Inventary.h"
#include <string>
#include <vector>

#include <json/json.h>
#include <string>
#include <vector>

std::string Inventary::sortedStr(const Json::Value& value, const std::vector<std::string>& sortKeys) const {
    Json::Value sortedValue;

    char newKey[60];
    int i = 0;

    for (const auto& key : sortKeys) {
        snprintf(newKey, sizeof(newKey), "SORTEDKEY:%03d-%s", i++, key.c_str());
        sortedValue[newKey] = value[key];
    }

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    writer["commentStyle"] = "None";
    writer["dropNullPlaceholders"] = true;
    writer["omitEndingLineFeed"] = true;

    std::string result = Json::writeString(writer, sortedValue);


    std::size_t pos = 0;
    while ((pos = result.find("SORTEDKEY:", pos)) != std::string::npos) {
        result.erase(pos, 14);
    }

    return result;
}
