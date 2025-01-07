# Huffman Compressor

Implementation of file compression and decompression using the Huffman algorithm in C. 

Each byte value (ASCII code) corresponds to a single character and is converted into a variable-length codeword by the Huffman algorithm. The *.huff file is a compressed file format.

This algorithm Implementation does not achieve significant compression efficiency on data that is already in a compressed format. It is more suitable for files where frequency differences are meaningful, such as text files. For example, on a test.txt file generated with Lorem Ipsum text, it shows approximately 50% compression efficiency.


## Usage

### 1. Compile
An executable file named `main` will be created in the `bin` directory.

```bash
make
```

### 2. Compression
Compress the target file, and a compressed file with the `.huff` extension will be generated.

```
make run args="-c <file>"
```
or
```
bin/main -c <file>
```

### 3. Decompression
Decompress a `.huff` file. the Decompressed file will have its .huff extension replaced with `.orig`.

```
make run args="-dc <file.huff>"
```
or

```
bin/main -dc <file.huff>
```

### 4. Test
The test.sh script compresses and decompresses the target file, then checks whether the decompressed file matches the original.

```bash
bash test.sh <target-file>
```
Alternatively, you can just compare the original file and the decompressed file using the `cmp` command.






## Compressing-Logic

Compress a file using the Huffman coding algorithm.

1. **File Reading**:
   - Opens the input file in binary mode.
   - Reads the file and calculates the frequency of each byte.

2. **Huffman Tree Construction**:
   - Creates a priority queue to build the Huffman tree based on character frequencies.
   - Generates a Huffman tree and assigns codewords to each byte.

3. **Header Metadata Creation & Write**:
   - Creates a metadata header that contains the file size and codeword mapping table.
   - Writes the header to the output file.

4. **Compress Original Data & Write**:
   - Encodes the input file's contents using the generated Huffman codewords.
   - Writes the encoded binary data to the output file.


## Decompressing-Logic

Decompress a file (`*.huff`) compressed with the Huffman coding algorithm. The output file's format will be `*.orig`.

1. **File Reading**:
   - Opens the input `.huff` file in binary mode.
   - Reads the metadata header to retrieve the original file size and Huffman tree structure.

2. **Huffman Tree Reconstruction**:
   - Rebuilds a Huffman tree (`Trie` structure) from the metadata contained in the header.

3. **Output File Initialization**:
   - Generates a new output file with the original file name (excluding the `.huff` extension).

4. **Data Decompression**:
   - Loads the compressed data into a memory buffer for decompression.
   - Writes the decoded binary data to the output file (`*.orig`).

## About `*.huff` file structure
The `*.huff` file consists of **Metadata Header**, **Section Divider**, and **Compressed Data**. The details of each component are explained below.

|Section|byte size|content|
|---------------|---------------|---------------|
| **Metadata Header**   | 20 bytes + Metadata (N) | File metadata and Huffman codewords mapping table |
| **Section Divider**   | 2 bytes                | Fixed (0x00 0x00)               |
| **Compressed Data**   | Variable               | Huffman-compressed file content |


**1. Metadata Header**

The metadata header stores information about the compressed file. It contains both fixed and variable-length fields, including the data required to reconstruct the Huffman tree and decode the compressed data.





| Field     | Size (Bytes)     | Description     |
|---------------|---------------|---------------|
| magic_number| 4| File identifier 0x46465548(FFUH) |
| header_size| 4 | Total size of the header (including the codeword metadata)| 
|file_size	|8 |	Original uncompressed file size|
|codeword_map_metadata_size	|4 |	Size of the codeword metadata|
|codeword_map_metadata	| variable (N bytes)|	Huffman tree codeword mapping for decoding|


The `codeword_map_metadata` consists of mapping table information that shows how each character (ASCII code value) is mapped to a specific codeword. Each `(character : codeword)` pair is converted into the format `(character : codeword length : codeword)`. This process is performed in the `ByteTable_make_codewords_map_metadata` function. 

Total size of the header is 20 bytes. 
(fixed) + codeword_map_metadata_size (variable)


**2. Section Divider**

The section divider separates the metadata header from the actual compressed data.
`0x00 0x00 (fixed)`


**3. Compressed Data**

The compressed data contains the Huffman-encoded representation of the original file content. Its size depends on the compression efficiency and the size of the original file.




