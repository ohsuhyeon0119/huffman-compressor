#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h> 
#include "Huffman_tree_util.h"
#include "Huffman_node.h"
#include "PriorityQueue.h"
#include "Byte_table.h"
#include "Huffman_header.h"
#include "Trie.h"
#include "Stream_buffer.h"
#include "exception_xmacro.h"
#include <time.h>

/**
 * @brief Compress a file using the Huffman coding algorithm.
 * 
 * This function takes the path to an input file, performs compression using
 * the Huffman coding algorithm, and writes the compressed data to a new file
 * with the `.huff` extension. The process includes the following steps:
 * 
 * 1. **File Reading**:
 *    - Opens the input file in binary mode.
 *    - Reads the file and calculates the frequency of each byte.
 * 
 * 2. **Huffman Tree Construction**:
 *    - Creates a priority queue to build the Huffman tree based on character frequencies.
 *    - Generates a Huffman tree and assigns codewords to each byte.
 * 
 * 3. **Header Creation**:
 *    - Creates a metadata header that contains file size, frequency table, and Huffman tree structure.
 *    - Writes the header to the output file.
 * 
 * 4. **Data Compression**:
 *    - Encodes the input file's contents using the generated Huffman codewords.
 *    - Writes the encoded binary data to the output file.
 *    - this logic has refactored by the function 'encode_and_write'.
 * 
 * 5. **Resource Cleanup**:
 *    - Frees allocated memory and closes all file streams.
 * 
 * @param inputFilePath The path to the input file to be compressed.
 */
void compress(const char* inputFilePath);





/**
 * @brief Decompress a file compressed with the Huffman coding algorithm.
 * 
 * This function takes the path to a `.huff` compressed file, reconstructs
 * the original data by reversing the Huffman coding process, and writes the
 * decompressed data to a new file. The process includes the following steps:
 * 
 * 1. **File Reading**:
 *    - Opens the input `.huff` file in binary mode.
 *    - Reads the metadata header to retrieve the original file size and Huffman tree structure.
 * 
 * 2. **Huffman Tree Reconstruction**:
 *    - Builds a Huffman tree ( Trie structure!! ) from the metadata contained in the header.
 *      
 * 3. **Output File Creation**:
 *    - Generates a new output file with the original file name (excluding `.huff` extension).
 * 
 * 4.  **Data decompression**
 *    - load the compressed data to the memory buffer, do Decompressing.
 *    - write decoded binary data to the output file
 * 
 * 5. **Resource Cleanup**:
 *    - Frees allocated memory and closes all file streams.
 * 
 * @param inputFilePath The path to the `.huff` file to be decompressed.
 * @param outputFilePath the path to the decompressed file to be saved.
 */
void decompress(const char* inputFilePath, const char* outputFilePath);
