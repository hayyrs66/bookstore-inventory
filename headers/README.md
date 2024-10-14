
# Book Inventory System

## Project Overview

This project is a book inventory management system that uses a **B-Tree** and an **unordered_map** for efficient storage, searching, and retrieval of book data. The system also includes multithreaded data processing and compression comparison using Huffman and arithmetic compression techniques. Below is a detailed explanation of the project's structure and its components.

## 1. Book Class (Book.h, Book.cpp)
- **Purpose**: Represents the book entity, holding information such as `ISBN`, `name`, `author`, `category`, `price`, and `quantity`.
- **Attributes**:
  - `isbn`, `name`, `author`, `category`, `price`, `quantity`: These store the basic details of a book.
  - `longitudaritmetica`: Stores the compressed size of the book's data using arithmetic compression.
- **Key Methods**:
  - Constructors for initialization, both parameterized and default.
  - `toJson()`: Converts the book's attributes into a JSON object.
  - `fromJson()`: Initializes the book's attributes from a JSON object.

## 2. BTreeNode Class (BTreeNode.h, BTreeNode.cpp)
- **Purpose**: Represents a node in the B-Tree, storing multiple books (`keys`) and pointers to child nodes (`children`).
- **Attributes**:
  - `keys`: A vector storing `Book` objects as keys in the node.
  - `children`: A vector of unique pointers to the child nodes of the current node.
  - `t`: The minimum degree of the B-Tree.
  - `leaf`: A flag indicating if the node is a leaf.
- **Key Methods**:
  - `traverse()`: Recursively visits all nodes and prints the book ISBNs.
  - `search()`: Recursively searches for a book by ISBN in the node and its children.
  - `insertNonFull()`: Inserts a book into the node if it is not full.
  - `splitChild()`: Splits a full child node to maintain the B-Tree properties.
  - `remove()`: Handles removal of a book from the node, with helper methods to handle rebalancing.

## 3. BTree Class (BTree.h, BTree.cpp)
- **Purpose**: Manages the overall structure of the B-Tree, where each node is a `BTreeNode`.
- **Attributes**:
  - `root`: A smart pointer to the root node of the B-Tree.
  - `t`: The minimum degree of the tree.
- **Key Methods**:
  - `insert()`: Inserts a `Book` into the tree, managing root splitting if necessary.
  - `remove()`: Removes a book from the tree based on ISBN.
  - `search()`: Initiates a search for a book by ISBN.
  - `printTree()`: Prints all the books in the tree using the node’s `printNode()` method.

## 4. Inventary Class (Inventary.h, Inventary.cpp)
- **Purpose**: Combines both the B-Tree structure and an `unordered_map` for managing the book inventory.
- **Attributes**:
  - `tree`: A B-Tree object that stores and manages books.
  - `bookMap`: An unordered map storing books by name for faster access.
- **Key Methods**:
  - `exportBookMapToJson()`: Exports the book data from `bookMap` to a JSON file.
  - `searchByName()`: Searches for books by name using `bookMap`.
  - `searchRecursive()`: Recursively searches the B-Tree for books by name.
  - `findBookByISBN()`: Searches for a book by ISBN in the B-Tree.
  - `sortedStr()`: Sorts a JSON object by specified keys.
  - `printInventary()`: Prints the entire inventory.

## 5. Testing Class (Testing.h, Testing.cpp)
- **Purpose**: This class processes input data, performs search operations, and compares the effectiveness of different compression methods.
- **Attributes**:
  - `inventary`: The inventory object used to manage books and perform operations on them.
- **Key Methods**:
  - `parseJson()`: Parses a JSON string into a `Json::Value` object.
  - `processChunk()`: Processes a chunk of input data, sanitizes the JSON, and performs search operations. It is designed for multithreading.
  - `printInventory()`: Prints the entire inventory.

## 6. HuffmanTree and ArithmeticCompression Classes
- **Purpose**: Used for compressing the book data using Huffman and arithmetic compression techniques.

## Project Workflow
1. **Books are managed** by the `Inventary` class using both a **B-Tree** for ISBN-based operations and an **unordered_map** for fast name-based searches.
2. **The B-Tree** handles all complex data operations, such as inserting, deleting, and searching for books.
3. **Data Export** is handled by the `Inventary` class, allowing the inventory to be exported to JSON.
4. **Multithreading** is introduced by the `Testing` class, which processes input data in chunks for efficiency.
5. **Compression** is applied using Huffman and arithmetic algorithms to compare data compression effectiveness.

# Inventory System Recommendations

## Recommendations for Optimizing Searches and Updates in the Inventory System

To optimize the performance of searches and updates in the inventory system, I decided to use a **hybrid approach** that combines a **B-Tree** structure with an **unordered_map**. The **B-Tree** is particularly effective when searching or updating records that rely on a unique identifier, such as the ISBN of books, because it allows these operations to be performed in logarithmic time, even with large datasets. Additionally, using an **unordered_map** to handle name-based searches boosts performance since it offers average constant time complexity for lookup operations, making it ideal for fields that aren't inherently sorted, like book titles.

On top of that, the inclusion of **multithreading** in the file processing function allows us to handle large data volumes concurrently. This makes the system more efficient, particularly when processing large batches of records or performing bulk updates.

## Data Structure Used and Decisions Made to Solve the Problem

When building the system, I opted for a combination of **B-Tree** and **unordered_map**. The **B-Tree** was chosen because of its ability to keep data balanced and to ensure efficient operations such as searching, inserting, and deleting, even as the dataset grows larger. The **unordered_map** was incorporated because of its speed when performing lookups on fields that don’t necessarily need to be sorted, like book names.

Additionally, we integrated compression algorithms like Huffman and Arithmetic Compression to reduce the overall size of the stored data. After running tests, I found that **Huffman compression** was more suitable for the data size and structure we were working with.


# Data Compression Analysis

## Questions and Answers

### a. Which algorithm is better for compressing these data and why?

After performing compression tests with both algorithms (Huffman and Arithmetic Compression), **Huffman** proved to be the better algorithm for compressing these specific data. This is because Huffman compression creates an optimized code based on the frequency of characters, which resulted in a better compression rate for our data, which consists of relatively short text with repetitive patterns. Huffman was more efficient due to the simplicity and the distribution of characters in our dataset.

Although Arithmetic Compression is theoretically more efficient for large datasets with fine-grained probability distributions, it introduced more overhead in this case due to its complexity and the more intensive computational requirements.

### b. If the best compression algorithm was Huffman, what should be considered to make Arithmetic Compression a better algorithm, and what changes should be made?

For **Arithmetic Compression** to outperform Huffman, we should consider several factors and make some adjustments:
- **Size of the data**: Arithmetic Compression performs better with large data volumes. If the compressed data were much larger, such as continuous data streams or extensive files, the performance of Arithmetic Compression would likely improve.
- **Distribution of the data**: If the distribution of characters were more uniform and not as skewed as in our test data (where certain characters appear more frequently), Arithmetic Compression could have an advantage since it handles fine-grained probabilities better.
- **Optimization of the implementation**: The current implementation of Arithmetic Compression could be optimized to reduce the computational overhead. Improving the algorithms for probability calculation and range handling could make Arithmetic Compression more efficient, even for moderate data volumes.

### c. How was the Lab01 program affected by these new requirements, and is data compression worth it?

The Lab01 program was affected by the new compression requirements in two main ways:
1. **Performance**: The addition of compression algorithms introduced computational overhead, particularly with Arithmetic Compression, which increased processing times in some cases.
2. **Complexity**: The program became more complex as it now handles the results of two different compression algorithms and compares the sizes of compressed files.

As for whether data compression is worth it, it depends on the context. If storage space is a significant concern (e.g., if the data grows substantially), then data compression is valuable. However, if the data volumes are small and processing time is critical, compression might not be as necessary.

### d. What recommendations would you make to improve the algorithms seen in class?

1. **Improve the Huffman implementation**: While Huffman works well for small datasets with repetitive patterns, it could be improved to better handle larger datasets with less variability. An adaptable implementation that recognizes cases where Huffman may not be ideal would be beneficial.
   
2. **Optimize Arithmetic Compression**: Although Arithmetic Compression is theoretically superior in some cases, its complexity makes the implementation more challenging. Improving probability calculations and reducing computational overhead could make it more competitive with Huffman, even for smaller datasets.

3. **Evaluate Hybrid Algorithms**: In some cases, using a combination of algorithms could be beneficial. For example, Huffman could be applied to smaller datasets, and Arithmetic Compression could be used when the data size exceeds a certain threshold.

4. **Extensive Testing**: I would recommend performing more tests on different types of datasets (large, small, diverse distributions) to gain a deeper understanding of how these algorithms behave under various scenarios, and adjust the implementations based on the most commonly processed data.

