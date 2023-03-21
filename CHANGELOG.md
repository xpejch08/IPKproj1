
# Chanellog

- The program is an implementation of a TCP/UDP client that communicates with a server done in c++ 20. 
- I first implemented the basic structure and defined my c++ classes. 
- The classes are helperFunctions,  clientTCP and clientUDP and they are described more in detail in the readme.
- After that I defined the methods of each class.
- In the main function of the code I first create all obects and then it carries on to the main part.
- I check if we are doing TCP or UDP and jump into the correct if branch
- The program also checks for SIGINT/C-c all the time and ends the program gracefully depenging on if we are in TCP or UDP
- in the if branches I call the object methods accordingly depending on if we are in TCP or UDP
- The code was tested multiple times on the virtual machine given with the task
- I wasn't able to find any problems or errors while running the code 
- I programmed the code locally and on github before I read that it should be done one the school git so the school git isn't in a very nice state 

