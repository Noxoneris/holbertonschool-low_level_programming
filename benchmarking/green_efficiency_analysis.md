
for the first part and only once we need to create one directory with mkdir benchmarking followed by cd benchmarking to enter in this empty directory for now, and create file afiliated with: 

so for resume the action in script for the 3 excercice we need in order to create 1 time: touch, nano file.c , script and completed it in, compil with gcc -Wall -Werror -Wextra -pedantic -std=gnu89 -Wno-long-long file.c o- file 

And we execute  this file.c 3 time in the terminalit with ./file.c. 

Next save result and create in 1 time with touch, nano file.md and copy the result in.

To finish we used git add and name of the two file .c and .md  (space between them only),git commit -m "we put the add we do just before exactly same order and close with " .

And to finish the excercice  we git push origin main (in the repo  github)

Now we have the structure of the excercices but what happened in this script,?

We do piece by piece: 



On the baseline_loop.c it describe few thing we considerate it for the function of  processor running cloak: script will traduce language cpu to human language.

the base of c have all the instruction, int main 

in there code source and the meaning of this code

next the conditional developement of the data  we use: the move with start = clock() and end = clock() and conditionnal cell used here with:  for (and all the condition taken in the up void, that the line to give is reason of existing.

read the time and the action.

clock() returns the number of CPU clock ticks since the program started (a raw number, not seconds yet).

To convert this raw value into readable intel for us with time in second, we divided the difference between two measurment (end and start) by clock_per_sec.

To convert ticks into seconds. in one moment (that why we tips 3 times ./file.c). so we can read the cpu clock.

Next, the conditional characters For, if, Else etc, that the cells of the idea we want this file run. the base are incrementation(or desacrementation) with a data given in the head.

There we say we want the code function loop 3 time in a raw.

The printf statements at the end just display the results: the number of iterations and the elapsed time.

that give the mean of the function in file.md: the result 3 time ./file.c

baseline_loop-metrics.md                                  

Iterations: 100000000
Execution time: 0.173570 seconds
Iterations: 100000000
Execution time: 0.094409 seconds
Iterations: 100000000
Execution time: 0.152548 seconds



the second project title comparison_algorithms.c as same basics on the baseline loop will compare two methodology of reading a script and traduce it in process diference meaning of lecturing execution data.

On the comparison_algorithms.c it describe two mean :count_even_naive() → double boucle (i et j) and count_even_single_pass() → simple boucle and witch one of them lead to a better time result, that will prove one of those two script

help to aim the time speed lecturing by the cpu even if we optain the same result at the end.
 
To simplify we cal in short "naive" and "single" pass:
 
"Single" way use condition "for" and only one "if" in a single loop ( int i; mean here an argument and for the system "data[i]" will contain the intel to process with this data and his valor. written as "for (i = 0; i < ARRAY_SIZE; i++)

Followed by "if (data[i] % 2 == 0)" (the count++) determine the execution calcul, all this in one condtional loop.
 
For "naive", we make two condition but we put two function : int i and int j, programme to give order to read one result and retry with the second result in a double loop.

for (i = 0; i < ARRAY_SIZE; i++)
for (j = 0; j <= i; j++)
if (data[j] % 2 == 0)
count ++;
longer and repetitive

The effeciency for"single" is for speeding up to have a result, and the "naive" to make sure we have a good resul; that for the cpu use more energy and need more memory allocated to calculate.

Proove that a algorithm more clever = less timing CPU = less energy. we saw it in one round only there for exemple.

Naive algorithm result: 625025000
Naive algorithm time: 2.321332 seconds
Single-pass algorithm result: 25000
Single-pass algorithm time: 0.000100 seconds



The third project instrumentation_lab.c, we make a program in 3 phases: build_dataset(), process_dataset(), reduce_checksum()

we stopwatch every phase separately + total. we mark Total have to be always one each mesure

We need to align 3 strings : TOTAL, BUILD_DATA, PROCESS, REDUCE followed by a "save" in elapsed who will conserv some time between start and end. that give cpu the exact time between the begining and the end. that we will put in show 

thanks to printf() we will use in to every phases a format specifier writeten as 6 number before the 0 ( written as %.6f) we precise it before in seconds.

we mark: every phase dont have much condition but precise function to it. 

As the same function than the precedents  programs  we use memory "elapsed" to confirm the time between the beginning and the end of a loop. (each of phases always in second marked by the cpu clock. 

printf("TOTAL seconds: %.6f\n", total_elapsed);
        printf("BUILD_DATA seconds: %.6f\n", build_elapsed);
        printf("PROCESS seconds: %.6f\n", process_elapsed);
        printf("REDUCE seconds: %.6f\n", reduce_elapsed);

As Result with ./instrumentation_lab

TOTAL seconds: 0.000776
BUILD_DATA seconds: 0.000337
PROCESS seconds: 0.000283
REDUCE seconds: 0.000151
