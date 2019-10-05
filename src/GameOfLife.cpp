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


GameOfLife::GameOfLife(unsigned int length) :

    // To configure the length
    _length(length),

    // To instanciate the Gecode::IntVarArray table,
    // which represents the board to produce. It will
    // containt the next-round GoL solution.
    //
    // As an array, the length is specified. The 0 and the 1
    // represent the domain [0, 1] in N, for each cell.
    _board(*this, length * length, 0, 1),

    // To instanciate the Gecode::IntVarArray table,
    // which represent the initial values set
    //
    // As an array, the length is specified. The 0 and the 1
    // represent the domain [0, 1] in N, for each cell.
    _localBoard(*this, length * length, 0, 1) {

}

void
GameOfLife::activate(unsigned int x, unsigned int y) {

    // To use a matrix view based on the array (type Gecode::IntVarArray),
    // in order to simplify the constaint posting. The matrix has
    // a length * length format.
    Gecode::Matrix<Gecode::IntVarArray>
        baseBoard(_localBoard, _length, _length);

    // CONSTRAINT : baseBoard at [x][y] has to be equal to 1.
    //              The cell is now considered as "activated"
    Gecode::rel(*this, baseBoard(x, y) == 1);

    // Registering the cell coordinates as a (x * _length + y) value
    _setValues.push_back(x * _length + y);
}

void
GameOfLife::activateFromPreviousBoard(GameOfLife* previousBoard) {

    // To copy the previousBoard
    Gecode::IntVarArray
        previousBoardCopy( previousBoard->_board );

    // To use the initial board and the to-produce board
    // as a matrix, using the view feature.
    // The matrix has a length * length format.
    Gecode::Matrix<Gecode::IntVarArray>
        localBoardMatrixView   (_localBoard,       _length, _length),
        previousBoardMatrixView(previousBoardCopy, _length, _length);

    // For each cells
    for(unsigned int i=1; i<_length-1; i++) {
        for(unsigned int j=1; j<_length-1; j++) {

            // The initial board is built by constraining the initial values
            // to be equal to the previous board's values, for each cell.
            Gecode::rel(
                *this,
                localBoardMatrixView(i, j) == previousBoardMatrixView(i, j)
            );

            // Registering the actives cells coordinates as a (x * _length + y) value
            if( previousBoardMatrixView(i, j).val() != 0 ) {
                _setValues.push_back( i * _length + j );
            }
        }
    }
}

#if GECODE_VERSION_NUMBER > 500100

GameOfLife::GameOfLife(GameOfLife& gameOfLife, unsigned int length) :
    Gecode::Space(share, gameOfLife) {

    // Updating means "copying with current instance". During the search phase, this constructor
    // is used to create a new instance having the "in-search" values in this
    // new search space, in order to find the solution, for each iteration
    _localBoard.update(*this, gameOfLife._localBoard);
    _board.update(*this, gameOfLife._board);

    // Taking the length of the board
    _length = length;
}

#else

GameOfLife::GameOfLife(bool share, GameOfLife& gameOfLife, unsigned int length) :
    Gecode::Space(share, gameOfLife) {

    // Updating means "copying the current instance's current values". During the search phase, this constructor
    // is used to create a new instance having the "in-search" values in this
    // new search space, in order to find the solution, for each iteration
    _localBoard.update(*this, share, gameOfLife._localBoard);
    _board.update(*this, share, gameOfLife._board);

    // Taking the length of the board
    _length = length;
}
#endif

void
GameOfLife::setContraints() {

    // To use a matrix view based on the array (type Gecode::IntVarArray),
    // in order to simplify the constaint posting. The matrix has
    // a length * length format.
    Gecode::Matrix<Gecode::IntVarArray>
        baseBoard(_localBoard, _length, _length),
        nextBoard(_board,      _length, _length);

    // For each cell
    for(unsigned int i=0; i<_length; i++) {
        for(unsigned int j=0; j<_length; j++) {

            // if the cell is on a border, the cell is
            // constrained to stay at 0
            if( i==0 || j==0 || i==_length-1 || j==_length-1 ) {
                Gecode::rel(*this, baseBoard(i, j)==0 );
                Gecode::rel(*this, nextBoard(i, j)==0 );
            } else {

                bool isContained = false;
                
                // Checking whether the cell is part of
                // the initialy activated cells for the baseBoard
                for(auto& value : _setValues) {
                    if( i * _length + j == value ) {
                        isContained = true;
                    }
                }

                // If the cell is not part of the initialy activated
                // on the baseBoard, then the cell of the baseboard is
                // contrained to be equal to 0
                if( !isContained ) {
                    Gecode::rel(
                        *this,
                        baseBoard(i, j) == 0
                    );
                }

                ///
                // This constraint is the core of the Game of Life rules.
                //
                // Line 1 : if the baseBoard cell has less than 2 neighbours, then the nextBoard cell is
                //          constrained to be equal to 0
                //
                // Line 2 : if the baseBoard cell has exactly 2 neighbours, then the nextBoard cell is
                //          constrained to take the initial value (it could be 0 or 1)
                //
                // Line 3 : if the baseBoard cell has exactly 3 neighbours, then the nexetBoard cell is
                //          constrained to be activated (equal to 1)
                //
                // Line 4 : if the baseBoard cell has more than 3 neighbours, then the nexetBoard cell is
                //          constrained to be desactivated (equal to 0)
                ///
                // All those constraints are made as one constraint by using the "XOR" binary operator.
                //
                // This represents the whole expression of the Game of Life rules.
                Gecode::rel(
                    *this,
                    ( sum(baseBoard.slice(i-1, i+2, j-1, j+2) ) - baseBoard(i, j) <  2 && nextBoard(i, j) == 0 ) ^
                    ( sum(baseBoard.slice(i-1, i+2, j-1, j+2) ) - baseBoard(i, j) == 2 && nextBoard(i, j) == baseBoard(i, j) ) ^
                    ( sum(baseBoard.slice(i-1, i+2, j-1, j+2) ) - baseBoard(i, j) == 3 && nextBoard(i, j) == 1 ) ^
                    ( sum(baseBoard.slice(i-1, i+2, j-1, j+2) ) - baseBoard(i, j) >  3 && nextBoard(i, j) == 0 )
                );
            }
        }
    }
}

void
GameOfLife::setBranch() {

    // To Branch a search space consists in indicating how to choose the next node.
    // Here, the branchment first chooses the node having the maximum range variable (Gecode::INT_VAR_MAX_MAX() ),
    // and having the maximum value (Gecode::INT_VAL_MAX() ).
    Gecode::branch(*this, _localBoard, Gecode::INT_VAR_MAX_MAX(), Gecode::INT_VAL_MAX() );

    // Same as previously
    Gecode::branch(*this, _board, Gecode::INT_VAR_MAX_MAX(), Gecode::INT_VAL_MAX() );
}

void
GameOfLife::printLocalBoard(std::ostream& o) const {
    print(o, _localBoard);
}

void
GameOfLife::printBoard(std::ostream& o) const {
    print(o, _board);
}

void
GameOfLife::print(std::ostream& o, const Gecode::IntVarArray& varArray) const {

    // Printing the board to the specified output
    // in a length * length format

    o << std::endl << ">> [BOARD] <------" << std::endl;
    unsigned int lineCounter( 0 );

    for(auto& e : varArray) {

        if( e.assigned() ) {
            o << e.val() << " ";
        } else {
            o << "E" << " ";
        }

        lineCounter++;

        if( lineCounter == _length ) {
            lineCounter = 0;
            o << std::endl;
        }
    }
}

#if GECODE_VERSION_NUMBER > 500100

Gecode::Space*
GameOfLife::copy() {

    // This method is used by Gecode to know how to instanciate the
    // nodes of the search space. It consists in calling the constructor
    // with the current instance as parameter to "continue" searching with
    // the already assigned variables
    return new GameOfLife(*this, _length);
}

#else

Gecode::Space*
GameOfLife::copy(bool share) {

    // This method is used by Gecode to know how to instanciate the
    // nodes of the search space. It consists in calling the constructor
    // with the current instance as parameter to "continue" searching with
    // the already assigned variables
    return new GameOfLife(share, *this, _length);
}

#endif

GameOfLife::~GameOfLife() {

}
