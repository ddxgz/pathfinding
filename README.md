Pathfinding
===========
                                                                              
This problem is from a company's hiring exam. If the appearence in here is not appropriate, please inform me to remove.
___
                                                                              
####Problem description:                                              
Find a shortest path from a start point to a goal point, and probably there will be 0, 1 or more checkpoints need to be arrived.                    
                                                                              
The map of which is like: 
                                                    
```                                                                              
11 5                                                                         
###########                                                                  
#...G.....#                                                                  
#.#.#.#...#                                                                  
#..@@@..S.#                                                                  
###########       
```                                                           
                                                                              
The first line contains two numbers to indicate the width and height of       
the map. Each characters in the map stands for different kinds of objects:    
                                                                              
* 'S' means the orienteering start.                                             
* 'G' means the orienteering goal.                                              
* '@' means an orienteering checkpoint.                                         
* '.' means an opened-block that players can pass.                              
* '#' means a closed-block that players cannot pass.                            
                                                                              
1 <= width <= 100                                                             
1 <= height <= 100                                                            
The maximum number of checkpoints is 18.                                      
                                                                              
####Rules:                                                                        
* It is allowed to move only by one step vertically or horizontally (up, down, left, or right) to the next block. Other types of movements, such as moving diagonally (left up, right up, left down and right down) and skipping one or more blocks, are NOT permitted.                              
* You MUST NOT get out of the map.                                             
* Distance is to be defined as the number of movements to the different blocks.
* You CAN pass opened-blocks, checkpoints, the start, and the goal more  than once if necessary.

____
###My solution so far             
                                                                             
To my point, the most difficult part is how to find a best path to go through every checkpoint. At present, I just use a stupid method which is "calculate every sequence". If you are intrested in this problem, please kindly share your thoughts with me.                                   
                                                                             
In this piece of code, A* was utilized to find the path. The h(n) is calculated by Manhattan * distance instead of Euclidean. This is referenced from `Patrick Lester` <http://www.policyalmanac.org/games/aStarTutorial.htm>
                                                                             
For the path finding with @(checkpoint), a dummy method was used, which just try all the possible paths with different sequence of checkpoints, this is the most cost part of this program. There should be a much more clever method to deal with this.                                             
                                                                             
Dynamic programming is probably a ideal method to deal with this.
 
                                                                             
Besides about the solution, any suggestion on the code is also very welcome. 

                                                                             
Run: ./a.out < example1.txt                                                  
                                                                                                                                             
                                                                              
