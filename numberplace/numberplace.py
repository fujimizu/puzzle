#!/usr/bin/env python
# -*- coding: utf8 -*-

import sys
#import psyco; psyco.full()

SIZE_BOARD = 9
SIZE_BLOCK = 3

sample_board = [
    [0,0,3,0,0,0,7,0,0],
    [0,2,0,4,0,6,0,8,0],
    [1,0,0,0,5,0,0,0,9],
    [0,4,0,0,0,0,0,1,0],
    [0,0,6,0,0,0,2,0,0],
    [0,1,0,0,0,0,0,3,0],
    [8,0,0,0,1,0,0,0,4],
    [0,6,0,9,0,2,0,5,0],
    [0,0,7,0,0,0,6,0,0]
]

def get_candidates(board):
    candidates = []
    # 候補集合の初期化
    for x in range(0, SIZE_BOARD):
        candidates.append([])
        for y in range(0, SIZE_BOARD):
            candidates[x].append([])
            candidates[x][y] = [1,2,3,4,5,6,7,8,9]
    
    for x in range(0, SIZE_BOARD):
        for y in range(0, SIZE_BOARD):
            if board[x][y] > 0:
                candidates[x][y] = []
                continue
            else:
                for i in range(0, SIZE_BOARD):
                    if board[x][i] in candidates[x][y]:
                        candidates[x][y].remove(board[x][i])
                    if board[i][y] in candidates[x][y]:
                        candidates[x][y].remove(board[i][y])
        area_x = int(x / SIZE_BLOCK)
        area_y = int(y / SIZE_BLOCK)
        for i in range(area_x * SIZE_BLOCK, (area_x + 1) * SIZE_BLOCK):
            for j in range(area_y * SIZE_BLOCK, (area_y + 1) * SIZE_BLOCK):
                if x == i or y == j: continue
                if board[i][j] in candidates[x][y]:
                    candidates[x][y].remove(board[i][j])
    return candidates

def is_solved(board):
    for x in range(0, SIZE_BOARD):
        for y in range(0, SIZE_BOARD):
            if board[x][y] == 0:
                return False
    return True

def solve(board):
    #print_board(board)

    if is_solved(board):
        print "solved!"
        print_board(board)
        sys.exit();

    candidates = get_candidates(board)
    num_cand = SIZE_BOARD + 1
    for x in range(0, SIZE_BOARD):
        for y in range(0, SIZE_BOARD):
            if len(candidates[x][y]) == 0: 
                continue
            elif len(candidates[x][y]) < num_cand:
                min_x = x
                min_y = y
                num_cand = len(candidates[x][y])
    if num_cand == SIZE_BOARD + 1:
        return

    for cand in candidates[min_x][min_y]:
        org = board[min_x][min_y]
        board[min_x][min_y] = cand
        solve(board)
        board[min_x][min_y] = org

def print_board(board):
    for row in sample_board:
        for i in row:
            if i == 0:
                print '.',
            else:
                print i,
        print
    print

def main():
    board = sample_board
    print_board(board)
    solve(board)

if __name__ == '__main__':
    main()

