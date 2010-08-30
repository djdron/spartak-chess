#ifndef __BOARD_H__
#define __BOARD_H__

#include "dialog.h"

#pragma once

class eBoard : public eDialog
{
public:
	eBoard();
	void	Position(const char* fen);
	void	Cursor(const char* pos);
	const char*	Cursor();
	void	Selected(const char* pos);
	char	Piece(const char* pos = NULL) const;
	enum
	{
		PIECE_SIZE = 26, PIECES_W = PIECE_SIZE*6, PIECES_H = PIECE_SIZE*2,
		BOARD_W = PIECE_SIZE*3, BOARD_H = PIECE_SIZE*2,
		BOARD_DIM = 8,
	};
	virtual void Paint(eBufferRGBA& buf);
	virtual bool Command(char cmd);

	enum eCellView { CELL_NORMAL, CELL_LIGHT, CELL_DARK };
	void DrawCell(eBufferRGBA& buf, const ePoint2& pos, bool black, eCellView view, char piece);
	eBufferRGBA chess_pieces;
	eBufferRGBA chess_board;
	bool Flash() const { return flash; }

protected:
	eBuffer<char> position;
	char cursor[3];
	char selected[3];
	bool changed;
	int frame;
	bool flash;
};

#endif//__BOARD_H__
