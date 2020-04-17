#include <stdint.h>

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	X,
	Y,
	A,
	B,
	L,
	R,
	ZL,
	ZR,
	LCLICK,
	RCLICK,
	MINUS,
	PLUS,
	HOME,
	CAPTURE,
	THROW,
	NOTHING,
	TRIGGERS
} Buttons_t;

typedef struct {
	Buttons_t button;
	uint16_t duration;
} command;

static const command step[] = {
	{LEFT, 1000},
	{RIGHT, 500},
	{NOTHING, 100},
	{R, 55},
	{A, 1300},
	{A, 1003},
	{B, 55},
	{A, 40},
	{B, 55},
	{A, 40},
	{B, 55},
	{A, 40},
	{B, 55},
	{A, 40},
	{B, 55},
	{A, 40}
};
