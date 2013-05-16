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

class Board:
    def __init__(self):
        self.board = []
        for y in range(SIZE_BOARD):
            row = [0] * SIZE_BOARD
            self.board.append(row)

    def load(self, board_array):
        for y in range(SIZE_BOARD):
            for x in range(SIZE_BOARD):
                self.board[y][x] = board_array[y][x]

    def _get_candidates(self, x, y):
        numbers = set()
        for i in range(SIZE_BOARD):
            if i != x and self.board[y][i] != 0:
                numbers.add(self.board[y][i])
            if i != y and self.board[i][x] != 0:
                numbers.add(self.board[i][x])
        area_x = int(x / SIZE_BLOCK)
        area_y = int(y / SIZE_BLOCK)
        for i in range(area_y * SIZE_BLOCK, (area_y + 1) * SIZE_BLOCK):
            for j in range(area_x * SIZE_BLOCK, (area_x + 1) * SIZE_BLOCK):
                if x != i or y != j and self.board[i][j] != 0:
                    numbers.add(self.board[i][j])
        return set(range(1, 10)) - numbers

    def get_candidates(board):
        candidates = []
        # 候補集合の初期化
        for y in range(SIZE_BOARD):
            candidates.append([])
            for x in range(SIZE_BOARD):
                candidates[y].append([])
                candidates[y][x] = [1,2,3,4,5,6,7,8,9]

        for y in range(0, SIZE_BOARD):
            for x in range(0, SIZE_BOARD):
                if board[y][x] > 0:
                    candidates[y][x] = []
                    continue
                else:
                    for i in range(0, SIZE_BOARD):
                        if board[y][i] in candidates[y][x]:
                            candidates[y][x].remove(board[y][i])
                        if board[i][x] in candidates[y][x]:
                            candidates[y][y].remove(board[i][y])
            area_x = int(x / SIZE_BLOCK)
            area_y = int(y / SIZE_BLOCK)
            for i in range(area_y * SIZE_BLOCK, (area_y + 1) * SIZE_BLOCK):
                for j in range(area_x * SIZE_BLOCK, (area_x + 1) * SIZE_BLOCK):
                    if x == i or y == j: continue
                    if board[i][j] in candidates[x][y]:
                        candidates[x][y].remove(board[i][j])
        return candidates

    def is_solved(self):
        for y in range(SIZE_BOARD):
            for x in range(SIZE_BOARD):
                if self.board[x][y] == 0:
                    return False
        return True

    def solve(self):
        if self.is_solved():
            return True

        num_cand = SIZE_BOARD + 1
        for y in range(0, SIZE_BOARD):
            for x in range(0, SIZE_BOARD):
                if self.board[y][x] != 0:
                    continue
                candidates = self._get_candidates(x, y)
                length = len(candidates)
                if not candidates:
                    return False
                elif length < num_cand:
                    num_cand = length
                    min_x = x
                    min_y = y
                    min_cand = candidates
                    if length == 1:
                        break
        if num_cand == SIZE_BOARD + 1:
            return False
        while min_cand:
            n = min_cand.pop()
            self.board[min_y][min_x] = n
            if self.solve():
                return True
            self.board[min_y][min_x] = 0

        return False

    def __str__(self):
        lines = []
        for y in range(SIZE_BOARD):
            row = []
            for x in range(SIZE_BOARD):
                if self.board[y][x] == 0:
                    row.append('.')
                else:
                    row.append(str(self.board[y][x]))
            lines.append(' '.join(row))
        return '\n'.join(lines)

def main():
    board = Board()
    board.load(sample_board)
    print board
    if board.solve():
        print 'solved!'
        print board
    else:
        print 'failed...'

if __name__ == '__main__':
    main()

