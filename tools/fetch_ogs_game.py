#!/usr/bin/env python
#
# This script downloads a game from OGS and produces a .game file
# that can be used by our test estimator.
#

import requests
import sys


def fetch_game(game_id):
    res = requests.get(
        'https://ggs.online-go.com/napi/game-state/%d/' % game_id
    )
    if res.status_code != 200:
        sys.stderr.write('Unable to fetch game\n')
        return None
    data = res.json()
    board = data['board']
    board = [
                [-1 if x == 2 else x for x in row]
                for row in board
            ]
    last_move = data['last_move']
    player_to_move = 0
    if last_move['y'] == -1:
        player_to_move = -board[last_move['y']][last_move['x']]
    if player_to_move == 0:
        player_to_move = 1
    return board, player_to_move


def print_game(output, board, player_to_move):
    output.write('# 1=black -1=white 0=open\n')
    output.write('height %d\n' % len(board))
    output.write('width %d\n' % len(board[0]))
    output.write('player_to_move %d\n' % player_to_move)
    for y in range(len(board)):
        output.write(' '.join('%2d' % x for x in board[y]) + '\n')

if __name__ == "__main__":
    if len(sys.argv) != 2 or int(sys.argv[1]) <= 0:
        sys.stderr.write("Usage: ./fetch_ogs_game.py <game-id>\n")
    else:
        game_id = int(sys.argv[1])
        filename = '%d.game' % game_id
        with open(filename, 'w') as output:
            board, player_to_move = fetch_game(game_id)
            print_game(output, board, player_to_move)
        print('Wrote %s' % filename)
