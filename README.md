# Dining with Turkish Philosophers
A code written to better understand the topics of Multithreading, Pthreads, and Process Synchronization.
Unlike native Dining Philosophers problem, this has a new and different stage before eating. Details below.

### To compile:
gcc -pthread -o dpp DPProblem.c

### To run:
./dpp

After running the code, you have to enter the number of philosophers

### Details:
The native Dining Philosophers problem consists of the eating-thinking cycle. In this implementation, there is an additional stage of getting Ayran before eating.
The cycle has 3 stages: Taking Ayran, Eating rice and thinking

Each philosopher cannot pass to the dining table without having one cup of ayran in their hand, and they drink all the ayran from their cup during the meal. However, there is only one ayran decanter on the table, and only one philosopher can put ayran into their cup from this decanter at a time. After a philosopher takes his/her ayran, he/she will need forks to eat, so if they are available starts to eat and drinks ayran. After eating is done, he/she starts to think and then goes back to the decanter to take new ayran. 
In each round, a random amount of ayran between 1 and 5 times the number of philosophers is poured into the decanter, and the round ends when the decanter is empty. At the end of the round, the amount each philosopher ate is printed.
