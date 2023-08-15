/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04
* Template for users to create their own bots
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <climits>

using namespace std;
using namespace Desdemona;

class MyBot: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        
        //Following array stores the weights of each board position used for evaluating a board state
        int weight[8][8]={100 , -12 , 8  ,  6 ,  6 , 8  , -12 ,  100,
                            -12 , -25 ,  -4, -4 , -4 , -4 , -25 , -12,
                            8   ,  -4 ,   25,   4,   4,   25,  -4 ,  8,
                            6   ,  -4 ,   4,   0,   0,   4,  -4 ,  6,
                            6   ,  -4 ,   4,   0,   0,   4,  -4 ,  6,
                            8   ,  -4 ,   25,   4,   4,   25,  -4 ,  8,
                            -12 , -25 ,  -4, -4 , -4 , -4 , -25 , -12,
                            100 , -12 , 8  ,  6 ,  6 , 8  , -12 ,  100};
        
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
        virtual int Minmaxprune(OthelloBoard& tboard,Turn t,int depth,int lbound,int ubound);
        virtual int boardeval(OthelloBoard& cboard);
    private:
};

MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn )
{
    
}

Move MyBot::play( const OthelloBoard& board )
{
    list<Move> moves = board.getValidMoves( turn ); //Available valid moves
    list<Move>::iterator it = moves.begin(); //We start with first valid move

    int depth=6; //The amount of lookahead/depth we explore the game from current board state using Min-Max algorithm

    int maxalpha=INT_MIN; //Stores the max evaluation/score obtained till now, initially as no move is explopred it is alloted INT_MIN
    int alpha; //Stores the score returned by Min-Max algorithm for a particular move

    Move advance=*it; //Stores the best move evaluated and returns it at the end

    while(it!=moves.end()) //We explore all valid moves to find the best one
    {
        if(it->x==0||it->x==7) // If this move allows us to occupy a corner position in this round we return this move as the move played
        {
            if(it->y==0||it->y==7)
            {
                return *it;
            }
        }
        
        OthelloBoard temp_board=board; //Temporary board object to store a possible new state once we make a move, initialized with the current board state
        temp_board.makeMove(turn,*it); //Update the current board state stored in temp_board with the move we are evaluating

        if(temp_board.getValidMoves(other(turn)).size()==0) //If playing this move leads to a board state in which opponent has no valid move we return this move as the move to be played this round
        {
            return *it;
        }

        alpha=Minmaxprune(temp_board,other(turn),depth,maxalpha,INT_MAX); //We apply Minmaxprune with depth to evaluate how favourable this move is for us through Min-Max algorithm, the second parameter indicates we Min-max should evaluate opponent's decision next as it will be his turn after this move
        if(alpha>maxalpha) //If this evaluation is better than the best evaluation encountered till now we update our bestmove(advance) and best evaluation
        {
            maxalpha=alpha;
            advance=*it;
        }
        it++; // We go on to evaluate next valid move
    }

    if(advance.x==0&&advance.y==0) //If a corner position is occupied we update the weights of its neighbors to +ve for future evaluation
    {
        weight[0][1]=12;
        weight[1][0]=12;
        weight[1][1]=25;
    }
    
    if(advance.x==0&&advance.y==7) //If a corner position is occupied we update the weights of its neighbors to +ve for future evaluation
    {
        weight[0][6]=12;
        weight[1][7]=12;
        weight[1][6]=25;
    }

    if(advance.x==7&&advance.y==0) //If a corner position is occupied we update the weights of its neighbors to +ve for future evaluation
    {
        weight[7][1]=12;
        weight[6][0]=12;
        weight[6][1]=25;
    }
    
    if(advance.x==7&&advance.y==7) //If a corner position is occupied we update the weights of its neighbors to +ve for future evaluation
    {
        weight[7][6]=12;
        weight[6][7]=12;
        weight[6][6]=25;
    }

    return advance; //Return the best move found as the move to be played by the bot
}

int MyBot::Minmaxprune(OthelloBoard& tboard,Turn t,int depth,int lbound,int ubound) //Implements the MinMax algorithm with Alpha-Beta pruning
{
    if(depth==0) //If we have reached the end of our lookahead evaluate the board state and return the evaluation
    {
        return boardeval(tboard);
    }

    if(t==turn) //We are evaluating bot's turn and we implement the Max portion of Min-Max algorithm here as bot tries to play optimally
    {
        list<Move> moves = tboard.getValidMoves(t); //Evaluate valid moves available for this board state
        list<Move>::iterator it = moves.begin();
        
        int max_val=INT_MIN; //Stores the max evaluation/score obtained till now, initially as no move is explopred it is alloted INT_MIN
        int val; //Stores the score returned by Min-Max algorithm for a particular move

        while(it!=moves.end()) //We explore all valid moves to find the best one
        {
            OthelloBoard temp_board=tboard; //Temporary board object to store a possible new state once we make a move, initialized with the current board state
            temp_board.makeMove(t,*it); //Update the current board state stored in temp_board with the move we are evaluating
            val=Minmaxprune(temp_board,other(t),depth-1,lbound,ubound); //We apply Minmaxprune with (depth-1) to evaluate how favourable this move is for us through Min-Max algorithm, the second parameter indicates we Min-max should evaluate opponent's decision next as it will be his turn after this move
            max_val=max(val,max_val); //If this evaluation is better than the best evaluation encountered till now we update the best evaluation
            lbound=max(lbound,val); // We update the bounds used to check pruning
            if(ubound<lbound) //If ubound<lbound there is no use exploring further moves and we prune the tree 
            {
                break;
            }
            it++;
        }

        return max_val; //Return the max evaluation found as we/bot will play that move optimally
    }

    else //We are evaluating opponent's turn and we implement the Min portion of Min-Max algorithm here as opponent try to play optimally
    {
        list<Move> moves = tboard.getValidMoves(t); //Evaluate valid moves available for this board state
        list<Move>::iterator it = moves.begin();
        
        int min_val=INT_MAX; //Stores the min evaluation/score obtained till now as opponent plays move minimizing our score, initially as no move is explopred it is alloted INT_MAX
        int val; //Stores the score returned by Min-Max algorithm for a particular move of opponent

        while(it!=moves.end())
        {
            OthelloBoard temp_board=tboard; //Temporary board object to store a possible new state once we make a move, initialized with the current board state
            temp_board.makeMove(t,*it); //Update the current board state stored in temp_board with the move we are evaluating
            val=Minmaxprune(temp_board,other(t),depth-1,lbound,ubound); //We apply Minmaxprune with (depth-1) to evaluate how favourable this move is for us through Min-Max algorithm, the second parameter indicates we Min-max should evaluate bot's decision next as it will be our turn after this move
            min_val=min(val,min_val); //Since if this evaluation is worse than the worst evaluation encountered till now the opponent will try to play this move and we update the worst/min evaluation
            ubound=min(ubound,val); // We update the bounds used to check pruning
            if(ubound<lbound) //If ubound<lbound there is no use exploring further moves and we prune the tree
            {
                break;
            }
            it++;
        }

        return min_val; //Return the min/worst evaluation found as opponent will play that move optimally
    }
}

int MyBot::boardeval(OthelloBoard& cboard) //Evaluates the board state provided
{
    int eval=0; //Stores the score/evaluation, initialized as 0
    Turn bot=turn; //Bot's color
    Turn opp=other(turn); //Opponent's color
    
    for(int i=0;i<8;i++) //We go through all board positions
    {
        for(int j=0;j<8;j++)
        {
            if(cboard.get(i,j)==bot) //If disc at this position is bot's
            {
                eval+=weight[i][j]; //We add weight of this position to our evaluation
            }
            else if(cboard.get(i,j)==opp) //If disc at this position is opponent's
            {
                eval-=weight[i][j]; //We subtract weight of this position to our evaluation
            }
        }
    }
    return eval; //Return the evaluation
}


// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}


