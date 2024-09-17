#include "Inventary.h"
#include <string>
#include <vector>

#include <json/json.h>
#include <string>
#include <vector>

std::string Inventary::sortedStr(const Json::Value& value, const std::vector<std::string>& sortKeys) const {
    Json::Value sortedValue;

    char newKey[60]; // Buffer para las nuevas claves formateadas
    int i = 0;

    for (const auto& key : sortKeys) {
        snprintf(newKey, sizeof(newKey), "SORTEDKEY:%03d-%s", i++, key.c_str());
        sortedValue[newKey] = value[key];  // Asignar el valor con la clave prefijada
    }

    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";  // No agregar indentación ni saltos de línea
    writer["commentStyle"] = "None"; // Desactivar comentarios
    writer["dropNullPlaceholders"] = true; // Eliminar valores nulos
    writer["omitEndingLineFeed"] = true; // Evitar salto de línea al final

    std::string result = Json::writeString(writer, sortedValue);

    // Eliminar el prefijo "SORTEDKEY" de las claves
    std::size_t pos = 0;
    while ((pos = result.find("SORTEDKEY:", pos)) != std::string::npos) {
        result.erase(pos, 14);
    }

    return result;
}
