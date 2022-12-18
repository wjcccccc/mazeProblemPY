mazeMap =[  ["#","#","#","#","#","#","#","#"], #outside the maze are all of wall(invalid to move) 
            ["#"," "," "," "," "," ","#","#"], #create a map
            ["#","#","#","#","#","#"," ","#"], #1 is wall 
            ["#","#"," "," "," "," ","#","#"], #0 is valid space
            ["#"," ","#","#","#","#","#","#"], #2 is goal
            ["#","#"," "," "," "," ","#","#"], #maze is the question in the Powerpoint page 57
            ["#","#","#","#","#","#"," ","#"],
            ["#","#"," "," "," "," ","#","#"],
            ["#"," ","#","#","#","#","#","#"],
            ["#","#"," "," "," "," ","G","#"],
            ["#","#","#","#","#","#","#","#"] ]

mazeSize = len(mazeMap) #get the data of maze

def mapPrinter(mazeMap): #define the function to print the map
    for i  in range(11):
        for j in range(8):
            print(mazeMap[i][j],end="")
        print()

def validPos(tup): #check the position if the cell can move into
    (col,row) = tup
    if col < 0 or row < 0 or col > mazeSize or row > mazeSize:
        return False
    return mazeMap[row][col] == " " or mazeMap[row][col] == "G"

def solve(mazeMap, start): #main solver
    Stack = [] #create a empty Stack 
    (col, row)=start #assign the start point
    Stack.append((col, row)) #push(stack active)
    
    while Stack:
        (col,row) = Stack.pop()
        if mazeMap[row][col] == "G": #find the goal
            return  #end of problem
        if mazeMap[row][col] == " ": #mark the path
            mazeMap[row][col] = "."

        if validPos((col-1,row-1)): Stack.append((col-1,row-1)) 
        if validPos((col,row-1)): Stack.append((col,row-1))
        if validPos((col+1,row-1)): Stack.append((col+1,row-1)) 
        if validPos((col-1, row)): Stack.append((col-1, row)) 
        if validPos((col+1, row)): Stack.append((col+1, row))
        if validPos((col-1,row+1)): Stack.append((col-1,row+1)) 
        if validPos((col,row+1)): Stack.append((col,row+1))
        if validPos((col+1,row+1)): Stack.append((col+1,row+1))
        

solve(mazeMap, (1,1)) #trigger the solver

mapPrinter(mazeMap) #print the answer


