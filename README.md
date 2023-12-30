# Mining FIs using CLM

## Interacting with the CPP part
### Possible command-line arguments
```text
-output output.json
-history history.json
-transaction "ABCD FEDC ASIJ LSNQ ISNE"
-image image.json
```
`-output`: /path/to/output.json - Stores the final image of the graph in this file.

`-history`: /path/to/history.json - Stores the jsonHistory part of the final image of the graph in this file.

`-transactions`: Input transactions to be processed by graph algorithm, separated by whitespaces.

`-image`: /path/to/image.json - Loads up the initial graph from a JSON image.

`-minsup`: Minimum Support Threshold > 0, if an invalid value or no value is provided the program uses the default value i.e 2

If neither `-output` and `-history` are provided then the script pretty-prints final graph image to console.

### Return codes
```text
 0 : Success
-1 : Invalid set of arguments
-2 : Bad history file
-3 : Bad output file
-4 : Bad image file
-5 : No transactions provided
-6 : Failed to open image file
-7 : Failed to open history file
-8 : Failed to open output file
```

## Example
A simple run to process the transactions `BCDE ABDE BCDE ABCDE DE C` and store the output to `output.json` and load up the graph from the image file `image.json`.
```text
clm-miner.exe -image image.json -output output.json -transactions "BCDE ABDE BCDE ABCDE DE C"
```
The script returns `code 0` if everything works fine.

## Contributors
```text
- Abdur Rahman Goraya (Graph, Edges, CLM, CPP API)
- Syed Ahsan Naqvi (UI)
- Mian Ali Ahmed (CLM-Miner?)
```