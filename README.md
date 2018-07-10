# Data structures and algorithms - C++

**The cppdsaext project provides a library of data structures and algorithms for software development in C++.**  
*Similar libraries are also available for software development in C [(cdsaext)](https://github.com/raltnoeder/cdsaext), Java [(javadsaext)](https://github.com/raltnoeder/javadsaext) and Go [(godsaext)](https://github.com/raltnoeder/godsaext)*.

Check the **experimental** and various **future/...** branches for **recent development, fixes and upcoming new features.**

The primary project goals are:
- Correctness and robustness
- High quality implementation
- Clean and easy to use API
- Suitability for special applications (e.g., embedded systems)
- Lightweight and small enough to be verifiable/provable

## Available building blocks

**Algorithms & utility functions**  
BSearch - Binary search on a sorted array  

**Data structures**  
QTree - Sorted key/value map providing O(log(n)) lookup, insert, delete  
VMap - Double ended queue (deque) key/value map  
VList - Double ended queue (deque) list  

**In development (experimental, future/...)**  
integerparse - Safe and strictly typed string to number conversion  
RndNrGen - Pseudo random number generators (PRNGs)  
PrngXorshift128Plus - Fast pseudo random number generation, xorshift128+ algorithm  
PrngXoroshiro128 - Fast pseudo random number generation, xoroshiro128 algorithm  
