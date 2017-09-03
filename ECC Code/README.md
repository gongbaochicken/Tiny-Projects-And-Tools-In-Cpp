# ECC Code
### Date: 2016

I built this error correcting code as an small projects for course Algorithm in Real World.

The algorithm is using both DFT under GF(2^8) and FFT under GF(257),
and decoding using Gaussian Elimination. I implemented my code in C++, and my IDE working for this project is Xcode. For your convenience, I provide the screenshots for the result of test cases, and I used the test cases on Piazza. 

The encode algorithm works, as it can produce the desired code word; and the decoding algorithm works, because as you can tell it can recover the information from the codeword with the tolerance of missing part of it.

Reference:
<a>http://www.cs.utsa.edu/~wagner/laws/FFM.html </a>

PS: I wrote a piece of code to transfer these three look-up tables from hex to decimal number.
I just don't include it in the zip file.
