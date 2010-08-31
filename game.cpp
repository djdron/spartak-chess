/*
Spartak Chess based on stockfish engine.
Copyright (C) 2010 djdron

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stockfish/application.h"
#include "stockfish/uci.h"
#include <string.h>
#include "game.h"
#include "board.h"

eDialog* PieceSelector(bool side, eBoard* board);

eGame::eGame() : piece_selector(NULL), game_status(NULL)
{
	desktop = new eDesktop;
	desktop->Create();
	board = new eBoard;
	board->Create();
	desktop->Insert(board);
	desktop->Focus(board);
	game_status = new eDialog;
	game_status->Create();
	desktop->Insert(game_status);
	game_status->bound.beg = ePoint2(240, 0);
	game_status->bound.end = ePoint2(319, 16);
	Application::initialize();
	New();
}

eGame::~eGame()
{
	Application::free_resources();
	desktop->Destroy();
	delete desktop;
}

void eGame::New()
{
	move_side = false;
	game_state = GS_NONE;
	ApplyCell(NULL);
	board->Cursor("e3");
	board->Flip(false);
	UCI_Command uci;
	uci.Execute("ucinewgame");
//	uci.Execute("position fen k7/7P/8/8/8/8/6p1/K w -");
//	move_side = true;
	UpdateBoardPosition();
}
bool eGame::Move(const char* move)
{
	if(Finished())
		return false;
	using namespace std;
	UCI_Command uci;
	if(!move)
	{
		string r = uci.Execute("go");
		string::size_type i = r.find("bestmove");
		if(i == string::npos)
			return false;
		i += 9;
		string::size_type j = r.find(" ", i);
		if(j == string::npos)
			j = r.length();
		uci.Execute("position moves " + r.substr(i, j - i));
	}
	else
	{
		uci.Execute("position moves " + string(move));
	}
	if(uci.res.find("illegal") == string::npos)
	{
		move_side = !move_side;
		if(uci.res.find("mate") != string::npos)
			game_state = GS_MATE;
		else if(uci.res.find("check") != string::npos)
			game_state = GS_CHECK;
		else if(uci.res.find("draw") != string::npos)
			game_state = GS_DRAW;
		else
			game_state = GS_NONE;
		UpdateBoardPosition();
		ApplyCell(NULL);
		return true;
	}
	ApplyCell(NULL);
	return false;
}

void eGame::UpdateBoardPosition()
{
	UCI_Command uci;
	uci.Execute("d");
	using namespace std;
	string::size_type p = uci.res.find("Fen is: ");
	if(p != string::npos)
	{
		p += 8;
		string::size_type p1 = uci.res.find(' ', p);
		if(p1 != string::npos)
		{
			string pos = uci.res.substr(p, p1 - p);
			board->Position(pos.c_str());
		}
	}
	game_status->text = GameState();
}
const char* eGame::GameState() const
{
	switch(game_state)
	{
	case GS_MATE:		return "mate";
	case GS_STALEMATE:	return "stalemate";
	case GS_DRAW:		return "draw";
	case GS_CHECK:		return "check";
	default:			return "";
	}
}
bool eGame::Finished() const
{
	switch(game_state)
	{
	case GS_MATE:
	case GS_STALEMATE:
	case GS_DRAW:
		return true;
	default:
		return false;
	}
}
eGame::eApplyCellResult eGame::ApplyCell(const char* cell)
{
	if(!cell || Finished())
	{
		strcpy(move, "");
		board->Selected(move);
		if(Finished())
			board->Cursor(move);
		return AC_RESET;
	}
	if(strlen(cell) != 2)
		return AC_ERROR;
	switch(strlen(move))
	{
	case 0:
		{
			char p = board->Piece(cell);
			if(p >= 0 && p <= 5 && move_side)
				return AC_ERROR;
			if(p >= 6 && p <= 11 && !move_side)
				return AC_ERROR;
			strcat(move, cell);
			board->Selected(cell);
			return AC_MOVE_BEGIN;
		}
	case 2:
		strcat(move, cell);
		if(!move_side && move[1] == '7' && move[3] == '8' && board->Piece() == 5) // white pawn reached end
			return AC_SELECT_PIECE;
		else if(move_side && move[1] == '2' && move[3] == '1' && board->Piece() == 11) // black pawn
			return AC_SELECT_PIECE;
		if(Move(move))
			return AC_MOVE_END;
		return AC_ERROR;
	default:
		return AC_ERROR;
	}
}
bool eGame::SelectPiece(char p)
{
	if(strlen(move) != 4)
		return false;
	move[4] = p;
	move[5] = '\0';
	if(Move(move))
		return true;
	return false;
}
bool eGame::Command(char cmd)
{
	bool ok = desktop->Command(cmd);
	if(ok)
	{
		if(piece_selector && !piece_selector->text.empty())
		{
			if(piece_selector->text != "-")
			{
				if(SelectPiece(piece_selector->text[0]))
					Move(NULL);
				else
					ApplyCell(NULL);
			}
			desktop->Remove(piece_selector);
			desktop->Focus(board);
			piece_selector->Destroy();
			delete piece_selector;
			piece_selector = NULL;
		}
		return true;
	}
	switch(cmd)
	{
	case 'a':
		switch(ApplyCell(board->Cursor()))
		{
		case AC_MOVE_BEGIN:
			if(board->Piece() == -1)
				ApplyCell(NULL);
			break;
		case AC_MOVE_END:
			Move(NULL);
			break;
		case AC_ERROR:
			ApplyCell(NULL);
			break;
		case AC_SELECT_PIECE:
			{
				piece_selector = PieceSelector(move_side, board);
				desktop->Insert(piece_selector);
				desktop->Focus(piece_selector);
			}
			break;
		default:
			break;
		}
		return true;
	case 'b':
		ApplyCell(NULL);
		return true;
	case 'n':
		New();
		return true;
	case 'g':
		ApplyCell(NULL);
		Move(NULL);
		return true;
	}
	return false;
}
