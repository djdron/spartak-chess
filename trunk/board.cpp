#include "board.h"
#include <string.h>
#include <assert.h>

eBoard::eBoard()
	: chess_pieces(ePoint2(PIECES_W, PIECES_H))
	, chess_board(ePoint2(BOARD_W, BOARD_H))
	, position(ePoint2(BOARD_DIM, BOARD_DIM))
	, changed(true), frame(0), flash(false)
{
	Position("");
	Cursor("");
	Selected("");
	ReadPNG("res/chess_pieces.png",	chess_pieces.Data(), PIECES_W, PIECES_H, 4);
	ReadPNG("res/chess_board.png",	chess_board.Data(), BOARD_W, BOARD_H, 4);
}
void eBoard::Position(const char* fen)
{
	static const std::string pieces_chars = "KQRNBPkqrnbp";
	changed = true;
	const char* piece = fen;
	int skip = 0;
	for(int j = 0; j < BOARD_DIM; ++j)
	{
		for(int i = 0; i < BOARD_DIM; ++i)
		{
			char p = -1;
			if(skip)
				--skip;
			if(!skip && *piece)
			{
				if(*piece >= '1' && *piece <= '8')
					skip = *piece - '0';
				else
				{
					std::string::size_type i = pieces_chars.find(*piece);
					if(i != std::string::npos)
						p = i;
				}
				++piece;
				if(*piece == '/')
					++piece;
			}
			position[ePoint2(i, j)] = p;
		}
	}
}
void eBoard::Cursor(const char* pos)
{
	if(*pos)
	{
		assert(strlen(pos) == 2);
	}
	strcpy(cursor, pos);
	changed = true;
	frame = 0;
	flash = false;
}
const char* eBoard::Cursor()
{
	return cursor;
}
void eBoard::Selected(const char* pos)
{
	if(*pos)
	{
		assert(strlen(pos) == 2);
	}
	strcpy(selected, pos);
	changed = true;
	frame = 0;
	flash = false;
}
char eBoard::Piece(const char* pos) const
{
	if(!pos)
		pos = selected;
	if(!*pos)
		return -1;
	int i = pos[0] - 'a';
	int j = '8' - pos[1];
	return position[ePoint2(i, j)];
}
void eBoard::DrawCell(eBufferRGBA& buf, const ePoint2& pos, bool black, eCellView view, char piece)
{
	ePoint2 cell_pos(0, 0);
	if(black)
		cell_pos.y = PIECE_SIZE;
	switch(view)
	{
	case CELL_NORMAL: 	break;
	case CELL_LIGHT:	cell_pos.x = PIECE_SIZE; break;
	case CELL_DARK:		cell_pos.x = 2*PIECE_SIZE; break;
	}
	if(piece >= 0)
	{
		ePoint2 piece_pos(0, 0);
		if(piece >= 6)
		{
			piece -= 6;
			piece_pos.y = PIECE_SIZE;
		}
		piece_pos.x = piece*PIECE_SIZE;
		for(int x = 0; x < PIECE_SIZE; ++x)
		{
			for(int y = 0; y < PIECE_SIZE; ++y)
			{
				eRGBA board_col = chess_board[cell_pos + ePoint2(x, y)];
				eRGBA piece_col = chess_pieces[piece_pos + ePoint2(x, y)];
				buf[pos + ePoint2(x, y)] = board_col + piece_col;
			}
		}
	}
	else
	{
		for(int x = 0; x < PIECE_SIZE; ++x)
		{
			for(int y = 0; y < PIECE_SIZE; ++y)
			{
				buf[pos + ePoint2(x, y)] = chess_board[cell_pos + ePoint2(x, y)];
			}
		}
	}
}
void eBoard::Paint(eBufferRGBA& buf)
{
	if(cursor[0] && ++frame > 10)
	{
		frame = 0;
		flash = !flash;
		changed = true;
	}
	if(!changed)
		return;

	for(int i = 0; i < BOARD_DIM; ++i)
	{
		font->DrawChar(buf, ePoint2(16 + 9 + i*eBoard::PIECE_SIZE, 0), 'a' + i);
		font->DrawChar(buf, ePoint2(16 + 9 + i*eBoard::PIECE_SIZE, eBoard::PIECE_SIZE*eBoard::BOARD_DIM + eFont::CHAR_H), 'a' + i);

		font->DrawChar(buf, ePoint2(4, 16 + 5 + i*eBoard::PIECE_SIZE), '1' + eBoard::BOARD_DIM - i - 1);
		font->DrawChar(buf, ePoint2(16 + 2 + eBoard::PIECE_SIZE*eBoard::BOARD_DIM, 16 + 5 + i*eBoard::PIECE_SIZE), '1' + eBoard::BOARD_DIM - i - 1);
	}

	for(int j = 0; j < BOARD_DIM; ++j)
	{
		for(int i = 0; i < BOARD_DIM; ++i)
		{
			char p = position[ePoint2(i, j)];
			bool black = (i + j) % 2 != 0;
			eCellView view = CELL_NORMAL;
			if(selected[0] == 'a' + i && selected[1] == '1' + (BOARD_DIM - j - 1))
				view = CELL_LIGHT;
			if(cursor[0] == 'a' + i && cursor[1] == '1' + (BOARD_DIM - j - 1))
				view = flash ? CELL_LIGHT : CELL_DARK;
			DrawCell(buf, ePoint2(16 + i*PIECE_SIZE, 16 + j*PIECE_SIZE), black, view, p);
		}
	}
}
bool eBoard::Command(char cmd)
{
	switch(cmd)
	{
	case 'l':
	case 'r':
	case 'u':
	case 'd':
		{
			char pos[3];
			strcpy(pos, Cursor());
			if(!pos[0])
				return true;
			switch(cmd)
			{
			case 'l':
				if(pos[0] != 'a')
				{
					--pos[0];
					Cursor(pos);
				}
				break;
			case 'r':
				if(pos[0] != 'h')
				{
					++pos[0];
					Cursor(pos);
				}
				break;
			case 'u':
				if(pos[1] != '8')
				{
					++pos[1];
					Cursor(pos);
				}
				break;
			case 'd':
				if(pos[1] != '1')
				{
					--pos[1];
					Cursor(pos);
				}
				break;
			}
		}
		return true;
	}
	return false;
}
