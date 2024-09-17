#pragma once
#include "BTree.h"
#include <json/json.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

class Inventary
{
    BTree tree;
    unordered_map<string, Book> bookMap;

public:
    Inventary(int t) : tree(t) {}

    void exportBookMapToJson(const std::string &filename)
    {
        std::ofstream outFile(filename);
        if (!outFile.is_open())
        {
            std::cerr << "Error al abrir el archivo para exportar los datos del bookMap." << std::endl;
            return;
        }

        // Iterar sobre los libros en bookMap
        for (const auto &pair : bookMap)
        {
            Json::Value bookJson = pair.second.toJson(); // Convertir cada libro a JSON

            // Mensaje de depuración para ver qué libros se exportan
            std::cerr << "Exportando libro con ISBN: " << pair.second.isbn << " y nombre: " << pair.second.name << std::endl;

            // Configurar el escritor para que no tenga indentación ni saltos de línea
            Json::StreamWriterBuilder writer;
            writer["indentation"] = "";            // Sin indentación
            writer["dropNullPlaceholders"] = true; // Omitir valores nulos

            std::string jsonLine = Json::writeString(writer, bookJson);
            outFile << jsonLine << std::endl; // Escribir cada libro en una línea
        }

        outFile.close();
        std::cerr << "Exportación completada: los datos de bookMap han sido guardados en " << filename << std::endl;
    }

    void insert(const Book &book)
    {
        std::cerr << "Intentando insertar libro en B-tree y bookMap: " << book.isbn << " - " << book.name << std::endl;

        // Insertar en la B-tree
        tree.insert(book);

        // Insertar en el bookMap por nombre
        bookMap[book.name] = book;
        std::cerr << "Libro insertado correctamente en bookMap: " << book.name << std::endl;

        // Depuración adicional
        BTreeNode *node = tree.search(book.isbn);
        if (node != nullptr)
        {
            std::cerr << "Confirmación: Libro con ISBN " << book.isbn << " encontrado en la B-tree tras la inserción." << std::endl;
        }
        else
        {
            std::cerr << "Error: No se encontró el libro con ISBN " << book.isbn << " en la B-tree tras la inserción." << std::endl;
        }
    }

    void remove(const std::string &isbn)
    {
        cout << "==> Buscando ISBN en DELETE: " << isbn << endl;
        BTreeNode *node = tree.search(isbn);
        if (node != nullptr)
        {
            cout << "Book found in B-tree. " << isbn << " Proceeding with removal." << endl;
            tree.remove(isbn);

            auto it = std::find_if(bookMap.begin(), bookMap.end(), [&](const auto &pair)
                                   { return pair.second.isbn == isbn; });

            if (it != bookMap.end())
            {
                bookMap.erase(it);
                cout << "Book removed from bookMap." << endl;
            }
        }
        else
        {
            cout << "Error: Book with ISBN " << isbn << " not found in B-tree for DELETE." << endl;
        }
    }

    void update(const Json::Value &bookData)
    {
        std::string isbn = bookData["isbn"].asString();
        std::cerr << "==> Buscando ISBN en PATCH: " << isbn << std::endl;

        // Buscar el libro en la B-tree por su ISBN
        BTreeNode *node = tree.search(isbn);
        if (node != nullptr)
        {
            bool bookFound = false;

            // Buscar el libro en las claves del nodo correspondiente
            for (auto &book : node->keys)
            {
                if (book.isbn == isbn)
                {
                    bookFound = true;
                    std::cerr << "==> ISBN " << isbn << " encontrado en la B-tree. Procediendo con la actualización." << std::endl;

                    // Guardar el nombre original antes de modificarlo
                    std::string oldName = book.name;

                    // Mostrar estado del libro antes de la actualización
                    std::cerr << "Estado anterior del libro:" << std::endl;
                    std::cerr << "Nombre: " << book.name << ", Autor: " << book.author << ", Categoría: " << book.category << ", Precio: " << book.price << ", Cantidad: " << book.quantity << std::endl;

                    // Actualizar los campos del libro basados en PATCH
                    if (bookData.isMember("name"))
                    {
                        std::string newName = bookData["name"].asString();
                        std::cerr << "Actualizando nombre: " << book.name << " a " << newName << std::endl;
                        book.name = newName;
                    }
                    if (bookData.isMember("author"))
                    {
                        std::string newAuthor = bookData["author"].asString();
                        std::cerr << "Actualizando autor: " << book.author << " a " << newAuthor << std::endl;
                        book.author = newAuthor;
                    }
                    if (bookData.isMember("category"))
                    {
                        std::string newCategory = bookData["category"].asString();
                        std::cerr << "Actualizando categoría: " << book.category << " a " << newCategory << std::endl;
                        book.category = newCategory;
                    }
                    if (bookData.isMember("price"))
                    {
                        std::string newPrice = bookData["price"].asString();
                        std::cerr << "Actualizando precio: " << book.price << " a " << newPrice << std::endl;
                        book.price = newPrice;
                    }
                    if (bookData.isMember("quantity"))
                    {
                        std::string newQuantity = bookData["quantity"].asString();
                        std::cerr << "Actualizando cantidad: " << book.quantity << " a " << newQuantity << std::endl;
                        book.quantity = newQuantity;
                    }

                    // Reinsertar el libro actualizado en el bookMap
                    if (bookMap.find(oldName) != bookMap.end())
                    {
                        bookMap.erase(oldName);
                    }
                    bookMap[book.name] = book;

                    // Mostrar estado actualizado del libro
                    std::cerr << "Estado actualizado del libro:" << std::endl;
                    std::cerr << "Nombre: " << book.name << ", Autor: " << book.author << ", Categoría: " << book.category << ", Precio: " << book.price << ", Cantidad: " << book.quantity << std::endl;
                    break;
                }
            }

            if (!bookFound)
            {
                std::cerr << "Error: ISBN " << isbn << " fue encontrado en el nodo, pero no se localizó el libro específico." << std::endl;
            }
        }
        else
        {
            std::cerr << "Error: Libro con ISBN " << isbn << " no encontrado en la B-tree para PATCH." << std::endl;
        }
    }

    vector<Book> searchByName(const std::string &name)
    {
        std::vector<Book> results;
        auto it = bookMap.find(name);
        if (it != bookMap.end())
        {
            results.push_back(it->second);
        }
        return results;
    }

    void searchRecursive(BTreeNode *node, const string &name, vector<Book> &results)
    {
        if (!node)
            return;

        for (const auto &book : node->keys)
        {
            if (book.name == name)
            {
                results.push_back(book);
            }
        }

        if (!node->leaf)
        {
            for (auto &child : node->children)
            {
                searchRecursive(child.get(), name, results);
            }
        }
    }

    // Optionally: a method to find a book by ISBN
    Book *findBookByISBN(const std::string &isbn)
    {
        // Traverse the B-Tree to find the book by ISBN (only for removal/update)
        BTreeNode *node = tree.search(isbn);
        if (node != nullptr)
        {
            for (auto &book : node->keys)
            {
                if (book.isbn == isbn)
                {
                    return &book;
                }
            }
        }
        return nullptr;
    }

    std::string sortedStr(const Json::Value &value, const std::vector<std::string> &sortKeys) const;

    void printInventary()
    {
        tree.printTree();
    }
};
