# PMCSN
This project simulates the check-in area of an airport, which is
usually an area that sees high passenger traffic. Our aim is to 
analyze different queue configurations in order to determine the best policy 
that should be used to make costumers happy. The underlining assumption
it's that airport passengers will choose one company over another based
on the wait in the check-in queues. If the wait it's too long a passenger
will probably choose another company for her/his next flight. It should also
be taken into consideration that, given the current covid-19 emergency, the delays can
be increased due to the necessary covid test that all passengers without
green pass should perform. 

## Configurations
The available queue configurations are:
* **SINGLE** - Only one queue in which all passenger will be enqueued in FIFO
  (First In First Out) order. When an officer it's available, picks the first element
  from the queue
* **MULTI** - One FIFO queue for each officer. When a passenger arrives, she/he can
  choose the queue with the least number of enqueued passengers
* **SITA** - 3 FIFO queues for 3 different types of passengers (Online, National, International check-in).
  When a passenger arrives, she/he is enqueued in the corresponding queue based on 
  the service type
  
All three configurations can be used to simulate the check-in area, while the covid test
area can be simulated using only the first two, since there's no correlation
between the test and the check-in type. 

## Demo
The demo.c file in the ./lib folder can be used to simulate all three configurations 
with and without the covid test area. The simulation can be configured by
varying the arrival rate, service rate, probability of having a green pass, etc.

## Configuration
This project can be configured through the constants defined in the config.h file in the root
directory. 

#### Demo parameters:
* FILENAME : name of the file where the result will be written 
  (ex. "filename.csv")
* STOP : simulation end time (sec)
* OFF : number of officers for the check-in queues
* ONLINEOFF : number of officers for online check-in
* NATIONALOFF : number of officers for national check-in
* INTERNATIONALOFF : number of officers for international check-in
* TESTOFF : number of officers for the test queues
* SEED : initial seed

#### Check-in area parameters:
* LAMBDA_1_5 : passenger arrival rate between 1:00:01 and 5:00:00
* LAMBDA_5_9 : passenger arrival rate between 5:00:01 and 9:00:00
* LAMBDA_9_13 : passenger arrival rate between 9:00:01 and 13:00:00
* LAMBDA_13_17 : passenger arrival rate between 13:00:01 and 17:00:00
* LAMBDA_17_21 : passenger arrival rate between 17:00:01 and 21:00:00
* LAMBDA_21_1 : passenger arrival rate between 21:00:01 and 1:00:00
* ONLINEMU : online check-in passenger service rate
* NATIONALMU : national check-in passenger service rate
* INTERNATIONALMU : international check-in passenger service rate
* ONLINEP : probability of having a passenger with online check-in
* NATIONALP : probability of having a passenger with national check-in
* INTERNATIONALP : probability of having a passenger with international check-in
* MAXWAIT : maximum time a passenger is willing to wait in the queue

#### Covid test area parameters:
* TESTMU : covid test service rate
* GREENPASSP : probability of having the greenpass
* POSITIVEP : probability of being positive to the covid test
* TESTWAIT: time to wait for the covid test results (usually 15 min)

## Deployment
    
    make all
    
    ./demo

## Cleanup

    make clean