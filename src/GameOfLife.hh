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

#ifndef _GAME_OF_LIFE_HH
#define _GAME_OF_LIFE_HH

#include <iostream>

#include <gecode/int.hh>
#include <gecode/float.hh>
#include <gecode/minimodel.hh>


/**
 * Game of Life research space model
 */
class GameOfLife : public Gecode::Space {

public:

    /**
     * First contructor
     * 
     * @param square-root length of the board
     */
    GameOfLife(unsigned int);

#if GECODE_VERSION_NUMBER > 500100
    /**
     * In-search used constructor
     * 
     * @param instance to copy from
     * @param square-root length parameter
     */
    GameOfLife(GameOfLife&, unsigned int);
#else
    /**
     * In-search used constructor
     * 
     * @param shared
     * @param instance to copy from
     * @param square-root length parameter
     */
    GameOfLife(bool, GameOfLife&, unsigned int);
#endif

    /**
     * To turn on a cell (set to 1)
     * 
     * @param x coordinate
     * @param y coordinate
     */
    void activate(unsigned int, unsigned int);

    /**
     * To turn on cells (set to 1) as on the previous board
     * 
     * @param Game of Life previous instance
     */
    void activateFromPreviousBoard(GameOfLife*);

    /**
     * To set constraints
     */
    void setContraints();

    /**
     * To branch the space search
     */
    void setBranch();

    /**
     * Destructor
     */
    ~GameOfLife();


public:

    /**
     * To print the initial board to the specified output
     * 
     * @param output
     */
    void printLocalBoard(std::ostream&) const;

    /**
     * To print the produced board to the specified output
     * 
     * @param output
     */
    void printBoard(std::ostream&) const;


protected:

    /**
     * To print a board to the specified output
     * 
     * @param output
     * @param board
     */
    void print(std::ostream&, const Gecode::IntVarArray&) const;


public:

#if GECODE_VERSION_NUMBER > 500100
    /**
     * To copy the instance during the search phase
     * (required by Gecode)
     * 
     * @param shared
     * @return a copy of the pace search
     */
    Gecode::Space * copy();
#else
    /**
     * To copy the instance during the search phase
     * (required by Gecode)
     * 
     * @return a copy of the pace search
     */
    Gecode::Space * copy(bool);
#endif

public:

    /**
     * The Game of Life board to produce
     */
    Gecode::IntVarArray _board;

    /**
     * The Game of Life board to start from
     */
    Gecode::IntVarArray _localBoard;

    /**
     * Vector containing the (x * length + y) values of the
     * cells turned to 1 during instance configuration
     */
    std::vector<unsigned int> _setValues;

    /**
     * Board's length
     */
    unsigned int _length;

};

#endif // _GAME_OF_LIFE_HH
