# Huffman Compressor

Implementation of file compression and decompression using the Huffman algorithm in C.




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
   - Creates a metadata header that contains the file size and frequency table.
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
   - Rebuilds a Huffman tree (Trie structure) from the metadata contained in the header.

3. **Output File Initialization**:
   - Generates a new output file with the original file name (excluding the `.huff` extension).

4. **Data Decompression**:
   - Loads the compressed data into a memory buffer for decompression.
   - Writes the decoded binary data to the output file (`*.orig`).

