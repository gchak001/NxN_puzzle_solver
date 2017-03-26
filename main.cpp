/*====================================================================
Name: Galust Chakmakchyan
SID: 861044260
Assignement: Project 1 (8-puzzle)
Class: CS 170 
Date: 11/6/2016
Instructor: Oben Moses Tataw

**I certify that the following code is my original work**
===============================================================================*/

#include <iostream>
#include <vector>
#include <algorithm>  
#include <queue>
#include <time.h>     

using namespace std;

const unsigned int BOARD_SIZE = 9;
const unsigned int ROW_SIZE =  3;
unsigned int blank_row;
unsigned int blank_column;
char board[ROW_SIZE][ROW_SIZE];
char curr_state[ROW_SIZE][ROW_SIZE];
char prev_state[ROW_SIZE][ROW_SIZE];
unsigned int depth = 0;
unsigned int node_count = 0;
unsigned int queue_size = 0;
unsigned int algorithm_choice = 0;

char solution[][ROW_SIZE] = {{'1','2','3'}, {'4','5','6'}, {'7','8','0'}};

struct MyTree_node
{
  struct MyTree_node *child1;
  struct MyTree_node *child2;
  struct MyTree_node *child3;
  struct MyTree_node *child4;
  struct MyTree_node *parent;
  char state[ROW_SIZE][ROW_SIZE];
  int cost;
  int total_cost;
};

//comparitor for priority queue
struct comp 
{
   bool operator()(const MyTree_node * a, const MyTree_node * b) const
   {
      return a->total_cost > b->total_cost;
   }
};

queue <MyTree_node *> my_q;
priority_queue <MyTree_node *, vector<MyTree_node *>, comp>my_pq;
vector <MyTree_node *> visited;
MyTree_node * my_tree = new MyTree_node;

//compares if two states are the same
bool state_cmp(char a[][ROW_SIZE], char b[][ROW_SIZE])
{
  for(int i = 0; i < ROW_SIZE; ++i)
			for(int j = 0; j < ROW_SIZE; ++j)
        		if(a[i][j] != b[i][j])
        			return false;
   return true;
}

//returns TRUE if given state has already been visited
bool find(char b[][ROW_SIZE])
{
  for(int i = 0; i < visited.size(); ++i)
    if(state_cmp(b, visited.at(i)->state))
      return true;
  return false;
}

//makes b the same as a ('=' operator for states)
void assign_state(char (&a)[ROW_SIZE][ROW_SIZE], char (&b)[ROW_SIZE][ROW_SIZE])
{
		for(int i = 0; i < ROW_SIZE; ++i)
			for(int j = 0; j < ROW_SIZE; ++j)
				b[i][j] = a[i][j];
}

//returns the number of misplaced tiles for a given state
int misplaced_heuristic(char b[][ROW_SIZE])
{
		unsigned int count = 0;
		for(int i = 0; i < ROW_SIZE; ++i)
			for(int j = 0; j < ROW_SIZE; ++j)
			{
				if(b[i][j] != solution[i][j])
				++count;
			}
		return count;
}

//returns the manhattan distance for a given state
int manhattan_heuristic(char b[][ROW_SIZE])
{
	int distance = 0;
	for(int i = 0; i < ROW_SIZE; i++)
		for(int j = 0; j < ROW_SIZE; j++)
			if(b[i][j] != '0')
				for(int m = 0; m < ROW_SIZE; m++)
					for(int n = 0; n < ROW_SIZE; n++)
						if(solution[m][n] == b[i][j])
							distance = distance + (abs(i-m) + abs(j-n));
					
	return distance;
}

//outputs given state
void output_board(char b[][ROW_SIZE])
{
	for(int i = 0; i < ROW_SIZE; ++i)
		for(int j = 0; j < ROW_SIZE; ++j)
		{
				cout << b[i][j];
				if (j != ROW_SIZE-1)
					cout << " | ";
				else if (j == ROW_SIZE-1)
					cout << endl;
		}
}

//initializes the starting state (root node)
void makeRoot(MyTree_node * node, char start_state[][ROW_SIZE])
{
		for(int i = 0; i < ROW_SIZE; ++i)
			for(int j = 0; j < ROW_SIZE; ++j)
			{
				node->state[i][j] = start_state[i][j];
			}
	if(algorithm_choice == 2)
	node->cost = misplaced_heuristic(node->state);  
	else
	node->cost = manhattan_heuristic(node->state);  
    node->total_cost = node->cost;
    node->child1 = NULL; 
    node->child2 = NULL; 
    node->child3 = NULL; 
    node->child4 = NULL;
    node->parent = NULL;
}

//creates a new node and return a pointer to the created node
MyTree_node * create_node(MyTree_node * parent_node, char current_state[][ROW_SIZE], int child_num)
{
		MyTree_node * t1 = new MyTree_node;
		for(int i = 0; i < ROW_SIZE; ++i)
			for(int j = 0; j < ROW_SIZE; ++j)
			{
				t1->state[i][j] = current_state[i][j];
			}
			
		if(algorithm_choice == 2)
		{
			t1->cost = misplaced_heuristic(parent_node->state);  
		}
		else
		{
			t1->cost = manhattan_heuristic(parent_node->state);  
		}
		t1->total_cost = (((parent_node->total_cost) - (parent_node->cost)) + 1 + t1->cost);
		t1->child1 = NULL; 
		t1->child2 = NULL; 
		t1->child3 = NULL; 
		t1->child4 = NULL;
		t1->parent = parent_node;
		
		if(child_num == 1)
		parent_node->child1 = t1;
		else if(child_num == 2)
		parent_node->child2 = t1;
		else if(child_num == 3)
		parent_node->child3 = t1;
		else if(child_num == 4)
		parent_node->child4 = t1;
		
		return t1;
}

//moves the '0' left for the given state and returns 1 if move is possible, and 0 otherwise
int move_left(char (&b)[ROW_SIZE][ROW_SIZE])
{
	if (blank_column != 0)
	{
		b[blank_row][blank_column] = b[blank_row][blank_column - 1];
		b[blank_row][blank_column - 1] = '0';
		
		return 1;
	}
	
	return 0;
}
//moves the '0' right for the given state and returns 1 if move is possible, and 0 otherwise
int move_right(char (&b)[ROW_SIZE][ROW_SIZE])
{
	if (blank_column != ROW_SIZE - 1)
	{
		b[blank_row][blank_column] = b[blank_row][blank_column + 1];
		b[blank_row][blank_column + 1] = '0';
		
		return 1;
	}
	
	return 0;
}
//moves the '0' up for the given state and returns 1 if move is possible, and 0 otherwise
int move_up (char (&b)[ROW_SIZE][ROW_SIZE])
{
	if(blank_row != 0)
	{	
		b[blank_row][blank_column] = b[blank_row - 1][blank_column];
		b[blank_row - 1][blank_column] = '0';
		
		return 1;
	}
	
	return 0;
}
//moves the '0' down for the given state and returns 1 if move is possible, and 0 otherwise
int move_down (char (&b)[ROW_SIZE][ROW_SIZE])
{
	if(blank_row != ROW_SIZE - 1)
	{
		b[blank_row][blank_column] = b[blank_row + 1][blank_column];
		b[blank_row + 1][blank_column] = '0';
		
		return 1;
	}
	
	return 0;
}
//expands a node for uniform cost(creates children of possible moves)
void expand_node(MyTree_node * parent_node, char current_state[][ROW_SIZE])
{
		int child_num = 1;
		MyTree_node * temp;
		
    	if(move_right(curr_state)  && !(find(curr_state)))
    	{
    		temp = create_node(parent_node, curr_state, child_num);
    		my_q.push(temp);
    		visited.push_back(temp);
    		++child_num;
    	}
    assign_state(board, curr_state);
    	if(move_down(curr_state) && !(find(curr_state)))
    	{
    		temp = create_node(parent_node, curr_state, child_num);
    		my_q.push(temp);
    		visited.push_back(temp);
    		++child_num;
    	}
    assign_state(board, curr_state);
    	if(move_up(curr_state) && !(find(curr_state)))
    	{
    		temp = create_node(parent_node, curr_state, child_num);
    		my_q.push(temp);
    		visited.push_back(temp);
    		++child_num;
    	}
   	assign_state(board, curr_state);
    	if(move_left(curr_state) && !(find(curr_state)))
    	{
			temp = create_node(parent_node, curr_state, child_num);
    		my_q.push(temp);
    		visited.push_back(temp);
    		++child_num;
    	}
    assign_state(board, curr_state);
}
//outputs a trace to the solution
void output_path(MyTree_node * child)
{
	while(child->parent != NULL)
	{
		output_board(child->state);
		cout << endl;
		child = child->parent;
		++depth;
	}
 
 output_board(child->state);
}
//expands a node for a heuristic cost(creates children of possible moves)
void expand_node_heuristic(MyTree_node * parent_node, char current_state[][ROW_SIZE])
{
	int child_num = 1;
	MyTree_node * temp;
	
    if(move_down(curr_state) && !(find(curr_state)))
    	{
    		
    		temp = create_node(parent_node, curr_state, child_num);
    		my_pq.push(temp);
    		visited.push_back(temp);
    		++child_num;
    		
    		if(temp->cost == 0)
    		{
		    	cout << "Solution!!" << endl;
		    	output_path(my_pq.top());
				cout << endl << "Depth: " << depth << endl;
				cout << "Nodes Expanded: " << node_count << endl;
				cout << "Max Queue Size: " << queue_size << endl;
    			exit(1);
    		}
    	}
    assign_state(board, curr_state);
    
    if(move_right(curr_state)  && !(find(curr_state)))
    	{
    		temp = create_node(parent_node, curr_state, child_num);
    		if(temp->cost == 0)
    		{
    			cout << "Solution!!" << endl;
		    	output_path(my_pq.top());
				cout << endl << "Depth: " << depth << endl;
				cout << "Nodes Expanded: " << node_count << endl;
				cout << "Max Queue Size: " << queue_size << endl;
    			exit(1);
    		}
    		my_pq.push(temp);
    		visited.push_back(temp);
    		++child_num;
    	}
    assign_state(board, curr_state);
    
    if(move_left(curr_state) && !(find(curr_state)))
    	{
    		temp = create_node(parent_node, curr_state, child_num);
    		if(temp->cost == 0)
    		{
    			cout << "Solution!!" << endl;
		    	output_path(my_pq.top());
				cout << endl << "Depth: " << depth << endl;
				cout << "Nodes Expanded: " << node_count << endl;
				cout << "Max Queue Size: " << queue_size << endl;
    			exit(1);
    		}
    		my_pq.push(temp);
    		visited.push_back(temp);
    		++child_num;
    	}
    	
      assign_state(board, curr_state);
      
    if(move_up(curr_state) && !(find(curr_state)))
    	{
    		temp = create_node(parent_node, curr_state, child_num);
    		if(temp->cost == 0)
    		{
    			cout << "Solution!!" << endl;
		    	output_path(my_pq.top());
				cout << endl << "Depth: " << depth << endl;
				cout << "Nodes Expanded: " << node_count << endl;
				cout << "Max Queue Size: " << queue_size << endl;
    			exit(1);
    		}
    		my_pq.push(temp);
    		visited.push_back(temp);
    		++child_num;
    	}
   
    	assign_state(board, curr_state);
}
//algorithm (BFS) for solving a puzzle with a uniform cost
void solve_uniform(MyTree_node * root)
{
	my_q.push(root);
	visited.push_back(root);
	    while (1)
	    {
	      if(state_cmp(my_q.front()->state, solution))
	      {
		    	cout << "SOLUTION:" << endl;
		    	output_path(my_q.front());
					cout << endl << "Depth: " << depth << endl;
					cout << "Nodes Expanded: " << node_count << endl; 
					cout << "Max Queue Size: " << queue_size;
		    	break;
	      }
	      
			 expand_node(my_q.front(), my_q.front()->state);
			 ++node_count;
			 if(my_q.size() > queue_size)
			 queue_size = my_q.size();
			 my_q.pop();
			 for(int i = 0; i < ROW_SIZE; ++i)
			    for(int j = 0; j < ROW_SIZE; ++j)
			    {
						if(my_q.front()->state[i][j] == '0')
						{
						    blank_row = i; 
						    blank_column = j;
						}
					}
			assign_state(my_q.front()->state, board);
			assign_state(board, curr_state);
	     
	    }
}
//algorithm (A*) for solving a puzzle with a heuristic cost
void solve_heuristic(MyTree_node * root)
{
	  my_pq.push(root);
		visited.push_back(root);
	    while (1)
	    {
			 expand_node_heuristic(my_pq.top(), my_pq.top()->state);
			 ++node_count;
			 if(my_pq.size() > queue_size)
			 queue_size = my_pq.size();
			 my_pq.pop();
			 for(int i = 0; i < ROW_SIZE; ++i)
			    for(int j = 0; j < ROW_SIZE; ++j)
			    {
						if(my_pq.top()->state[i][j] == '0')
						{
						    blank_row = i; 
						    blank_column = j;
						}
					}
			assign_state(my_pq.top()->state, board);
			assign_state(board, curr_state);
	   }
}

int main()
{
	srand (time(NULL));

	vector<char> vec;
	int puzzle_type = 0;
	char num1 = '0'; char num2 = '0'; char num3 = '0';
	cout << "Welcome to Galust's puzzle solver!!" << endl;
	cout << "Type '1' to use a default puzzle, or '2' to enter your own puzzle: ";	
	cin >> puzzle_type;

	if (puzzle_type == 1)
	{
		for(int i = 0; i < ROW_SIZE; ++i)
			for(int j = 0; j < ROW_SIZE; ++j)
				{
					do{
						board[i][j] = (rand() % BOARD_SIZE) + 48;
				
					}while(find(vec.begin(), vec.end(), board[i][j]) != vec.end());					
			
					if (board[i][j] == '0')
					{
						blank_row = i;
						blank_column = j;
					}
					vec.push_back(board[i][j]);	
				}
					
	}
	else if (puzzle_type == 2)
	{
		cout <<  "Enter the first row, use space or tabs between numbers: ";
		cin >> num1 >> num2 >> num3;
		board[0][0] = num1; board[0][1] = num2; board[0][2] = num3;
		cout  << "Enter the second row, use space or tabs between numbers: ";	
		cin >> num1 >> num2 >> num3;
		board[1][0] = num1; board[1][1] = num2; board[1][2] = num3;
		cout << "Enter the third row, use space or tabs between numbers: ";
		cin >> num1 >> num2 >> num3;
		board[2][0] = num1; board[2][1] = num2; board[2][2] = num3;
		
		for(int i = 0; i < ROW_SIZE; ++i)
		    for(int j = 0; j < ROW_SIZE; ++j)
		    {
				if(board[i][j] == '0')
				{
				    blank_row = i; 
				    blank_column = j;
				}
        	}
	}
	cout << "1. Uniform Cost Search" << endl;
	cout << "2. A* Misplaced Tile Heuristic" << endl;
	cout << "3. A* Manhattan Distance Heuristic" << endl << endl;
	while(algorithm_choice != 1 && algorithm_choice != 2 && algorithm_choice != 3)
	{
		cout << "Please choose your algorithm: " ;
		cin >> algorithm_choice;
	}
    assign_state(board, curr_state);
    makeRoot(my_tree, board);
	if(algorithm_choice == 1)
	solve_uniform(my_tree);
	else 
    solve_heuristic(my_tree);
   
    return 0;
}