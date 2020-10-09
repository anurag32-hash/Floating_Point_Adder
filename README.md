# Floating_Point_Adder

In this I have done a software implementation of the Floating Point Adder.

**Inputs** to the program will be:

1. A text file with each line containing a pair of floating point numbers to be added, in binary
    representation (a string of ‘0’ and ‘1’ characters in the file). See sample below.

**Outputs** from the program will be:

1. A sequence of addition results, each line containing one result (a string of ‘0’ and ‘1’
    characters) and the number of clock cycles required for performing the addition.

**Sample Input File (each line contains the representation for 2 floating point numbers,
both 32 bits wide)** :
11001100110011001111110011001100 11001100110011001111110011001100
01001110110011001111110011001101 11001100110111001101110011001110

**Sample Output File (just examples. These are not be the computed outputs for the above
input file)** :
11001110110011001111110011001101 01001110110011001111110011001101 4

```
Cases Handled                                                                  Example

Zero + Normal = Normal				                     00000000000000000000000000000000 00000000100000000000000000000010
Zero + Inf = Inf				                                     00000000000000000000000000000000 01111111100000000000000000000000
Zero + NaN = NaN (predefined)                                      00000000000000000000000000000000 01111111100000000000000001000000
(-Zero) + (-Zero) = -Zero			                             10000000000000000000000000000000 10000000000000000000000000000000
Zero + Denormal = Denormal(predefined) 		     00000000000000000000000000000000 00000000000000000100000000000000
Denormal + Denormal = Denormal (predefined)	     00000000000000001000000000000000 00000000000000001000000000000000
Denormal + Inf = Inf				                             00000000000000001000000000000000 01111111100000000000000000000000	
Denormal + NaN = NaN (predefined)		             00000000000000001000000000000000 01111111100000000000000001000000
Denormal + Normal = Denormal (predefined)	     00000000000000001000000000000000 00000011100000000100100101000000
Inf + (-Inf) = NaN (predefined)			                     01111111100000000000000000000000 11111111100000000000000000000000
Inf + Inf = Inf 				                                     01111111100000000000000000000000 01111111100000000000000000000000
Inf + Nan = Nan	(predefined)			                     01111111100000000000000000000000 01111111100000000000000001000000
Inf + Normal = Inf				                             01111111100000000000000000000000 00000011100000000100100101000000
NaN + NaN = NaN (predefined)			                     01111111100000000000000001000000 01111111100000000000000001000000
NaN + Normal = NaN (predefined)			             01111111100000000000000001000000 00000011100000000100100101000000
Normal + Normal = Denormal                                        10000001000000000000000000000000 00000000100000000000000000000010
Same exp Same mantissa Diff sign = zero                     00001010010101010101010101010101 10001010010101010101010101010101
Normal + Normal = NaN/Overflow(predefined)	     01111111011111111110111111111111 01111111011111111110111111111111
Underflow 					                                     00000000100000000000000000000011 10000000100000000000000000000001


1. First we are categorizing the operands and then accordingly adding them. 
2. In case of Normal addition, we are first aligning the fraction with lower exponent such that both the exponent are equal. 
3. A parameters will be of the operand with higher exponent and B parameters will be of the other one.
4. In case if the operands are of different signs, we're taking the 2's complement of significandB.
5. After adding them we'll check for the msb of the result. If signs are different and msb = 1, then we'll take the 2's complement of the result and flip the sign of the final result.
6. Then we'll normalize and send it to the rounding function. In case there's an overflow or underflow, we'll break the flow and return exception. 
7. After rounding we'll again check for the normalization, if not, we'll normalize it again and check for overflow and underflow, and return it.  
```


