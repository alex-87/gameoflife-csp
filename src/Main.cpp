/**
 * MIT License
 * 
 * Copyright (c) 2019 Alexis LG
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "GameOfLife.hh"

#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>
#include <gecode/driver.hh>

#include <iostream>
#include <memory>
#include <functional>


int main(int argc, char *argv[]) {

    // Usage in case of no parameters set
    if(argc < 3) {
        std::cout << "Usage : " << argv[0] << " <square-root length> <rounds>" << std::endl;
        std::cout << "Example : " << argv[0] << " 10 30" << std::endl;
        std::cout << "          will result in a 10*10 board, playing 30 rounds." << std::endl;
        return 0;
    }

    // Getting the square-root length of the GoL board
    const unsigned int length ( std::atoi(argv[1]) );

    // Getting the number of rounds to play
    const unsigned int nbRound( std::atoi(argv[2]) );


    // Check for valid length
    if( length==0 ) {
        std::cerr << "Error : the <square-root length> has to be a valid integer > 0." << std::endl;
        return 1;
    }

    // Initializing the Game of Life Gecode space search
    // with the specified length
    std::unique_ptr<GameOfLife> gamePtr(
        new GameOfLife(length)
    );

    // Getting the initial board configuration
    // by reading "0" or "1" values passed in the
    // Standard Input.
    unsigned int u(0), v(0);
    while(u * length + v < length * length) {

        int tValue = 0;
        std::cin >> tValue;

        if( tValue != 0 ) {
            // If the value != 0, then the current cell
            // is considered as activated (equal to 1)
            gamePtr->activate(u, v);
        }

        v++;
        if( v==length ) {
            u++;
            v = 0;
        }
    }

    // To set the Game of Life constraints on the space search
    gamePtr->setContraints();

    // Branching by specifying the node browsering strategy
    gamePtr->setBranch();

    // Initializing the searcher with the created search space.
    // DFS (Deep-First Search) is part of the Gecode provided searchers
    std::shared_ptr<Gecode::DFS<GameOfLife>> solutionSearcher(
        new Gecode::DFS<GameOfLife>( gamePtr.get() )
    );

    // For each round
    for(int i=0; i < nbRound; i++) {

        // The Game of Life solution
        std::shared_ptr<GameOfLife> solution;

        // If a solution has been found (if this round has correctly been completed
        // in accordance with initial situation and GoL constraints)
        if( solution = std::shared_ptr<GameOfLife>( solutionSearcher->next() ) ) {

            // Displaying the board in the Standard Output
            solution->printBoard(std::cout << std::endl << "Round " << i+1);
        } else {
            // If not any solution has been found, Exit.
            std::cout << "Not any solution found." << std::endl;
            return 0;
        }

        // Creating the space search of the next round with the new situation
        gamePtr = std::unique_ptr<GameOfLife>( new GameOfLife(length) );

        // Initializing activated cells according to the new situation
        gamePtr->activateFromPreviousBoard( solution.get() );

        // Posting GoL constraints to the new search space
        gamePtr->setContraints();

        // Branching
        gamePtr->setBranch();

        // Searching for the solution
        solutionSearcher = std::shared_ptr<Gecode::DFS<GameOfLife>>(
            new Gecode::DFS<GameOfLife>( gamePtr.get() )
        );

    }

    return 0;
}
