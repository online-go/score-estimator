#pragma once

#define MAX_WIDTH 25
#define MAX_HEIGHT 25
#define MAX_SIZE (MAX_WIDTH*MAX_HEIGHT)

#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#ifdef DEBUG
static const char board_letters[] = "abcdefghjklmnopqrstuvwxyz";
#endif
