# Mining FIs using CLM

## Interacting with the CPP part
### Required command-line arguments
`transactions`: the input transactions
`max-nodes`: the maximum number of nodes in the universe transactions
`min-support`: the minimum support number to find valid FIs

### Optional command-line arguments
`graph-output`: the JSON file to store graph output in
`fis-output`: the JSON file to store FIs found using `min-support` in
`image`: the initial image JSON file to load the graph from, this should be an instance of JSON using the `output` argument.

### Return codes
```text
 0 : Success
 1 : Invalid set of arguments
 2 : Max Nodes number not provided
 3 : Max Nodes number is invalid
 4 : Min Support number not provided
 5 : Min Support number is invalid
 6 : Transactions not provided
10 : Bad Graph Output file
11 : Failed to write to Graph Output file
20 : Bad FIs Output file
21 : Failed to write to FIs Output file
30 : Failed to load up image file
```

### Example Run
A simple run to process the transactions `CDEF DE FG CDF DF CEF BCDEF` with a `min support` of 2.

```text
clm-miner -transactions "CDEF DE FG CDF DF CEF BCDEF" -min-support 4 -max-nodes 6
```
Output:
```text
Nodes: 
	node : weight
	B : 1
	C : 4
	D : 5
	E : 4
	F : 6
	G : 1
Edges: 
	from : to : extra information : weight
	C : D : E,F, : 2
	C : E : F, : 3
	C : F :  : 4
	D : E : F, : 2
	D : F :  : 4
	E : F :  : 3
	D : E :  : 1
	F : G :  : 1
	C : D : F, : 1
	B : C : D,E,F, : 1
	B : D : E,F, : 1
	B : E : F, : 1
	B : F :  : 1
CLM: 
	  | B | B C D E F G | C | B C D E F G | D | B C D E F G | E | B C D E F G | F | B C D E F G | G | B C D E F G | 
	B | 1 | 0 0 0 0 0 0 | 1 | 0 0 1 1 1 0 | 1 | 0 0 0 1 1 0 | 1 | 0 0 0 0 1 0 | 1 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 
	C | 0 | 0 0 0 0 0 0 | 4 | 0 0 0 0 0 0 | 3 | 0 0 0 2 3 0 | 3 | 0 0 0 0 3 0 | 4 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 
	D | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 5 | 0 0 0 0 0 0 | 3 | 0 0 0 0 2 0 | 4 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 
	E | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 4 | 0 0 0 0 0 0 | 3 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 
	F | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 6 | 0 0 0 0 0 0 | 1 | 0 0 0 0 0 0 | 
	G | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 0 | 0 0 0 0 0 0 | 1 | 0 0 0 0 0 0 | 
FIs:
	C
	CD
	CE
	CF
	D
	DE
	DF
	E
	EF
	F

Process finished with exit code 0
```

## Contributors
```text
- Abdur Rahman Goraya (Graph, Edges, CLM, CLM-Miner, CPP API)
- Syed Ahsan Naqvi (UI)
- Mian Ali Ahmed
- Course Instructor: Dr. Zahid Halim
- Course Lab Instructor: Sir Usama Arshad
```

## Based on
This project is based on the paper
> On the Efficient Representation of Datasets as Graphs to Mine Maximal Frequent Itemsets

by
- Zahid Halim, Senior Member, IEEE
- Omer Ali
- Muhammad Ghufran Khan