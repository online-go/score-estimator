from typing import List
import ctypes

_estimator_so = ctypes.cdll.LoadLibrary('./score_estimator.so')

# Color.h
EMPTY = 0
BLACK = 1
WHITE = -1

def estimate(width, height, data, player_to_move, trials, tolerance):
  """Estimate the score and area using the OGS score estimator.

  The `data` argument must be a `height` * `width` iterable indicating
  where player stones are.

  Return value is the difference between black score and white score
  (positive means back has more on the board).

  The `data` argument is modified in-place and will indicate the player
  that the position.
  """
  arr = ((width * height) * ctypes.c_int)()
  for i, v in enumerate(data):
    arr[i] = v
  score = _estimator_so.estimate(
      width, height, arr, player_to_move, trials,
      ctypes.c_float(tolerance)
  )
  data[:] = arr
  return score
estimate.__annotations__ = {
    'width': int, 'height': int, 'data': List[int], 'player_to_move': int,
    'trials': int, 'tolerance': float, 'return': int,
}
