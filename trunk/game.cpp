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
#include <stdio.h>
#include <algorithm>
#include "game.h"
#include "io.h"
#include "ui/board.h"

namespace xUi
{
eDialog* PieceSelector(bool side, eBoard* board);
eDialog* Menu();
void MenuUpdateItem(eDialog* m, const std::string& text, const std::string& id);
}
//namespace xUi

eGame::eGame()
	: desktop(NULL), board(NULL), piece_selector(NULL), game_status(NULL), splash(NULL), menu(NULL)
	, move_count(0), game_state(GS_NONE), state(S_NONE), start_time(0), difficulty(D_EASY)
{
	desktop = new xUi::eDesktop;
	desktop->Create();
}

eGame::~eGame()
{
	Done();
}

void eGame::Init()
{
	board = new xUi::eBoard;
	board->Create();
	game_status = new xUi::eDialog;
	game_status->Create();
	game_status->Bound(eRect(ePoint2(240, 0), ePoint2(319, 16)));
	Application::initialize();
	New();
	Load();
}
void eGame::Done()
{
	if(state > S_SPLASH)
	{
		Application::free_resources();
		Store();
	}
	desktop->Destroy();
	delete desktop;
	desktop = NULL;
}
void eGame::New(bool side)
{
	move_count = 0;
	moves.clear();
	game_state = GS_NONE;
	ApplyCell(NULL);
	board->Cursor(side ? "d6" : "e3");
	board->Flip(side);
	UCI_Command uci;
	uci.Execute("ucinewgame");
//	uci.Execute("position fen k7/7P/8/8/8/8/6p1/K w -");
//	move_side = true;
	if(side)
		Move(NULL);
	UpdateBoardPosition();
}
bool eGame::Move(const char* _move)
{
	if(Finished())
		return false;
	using namespace std;
	UCI_Command uci;
	string move;
	if(!_move)
	{
		std::string go = "go";
		if(move_count > 10)
		{
			switch(difficulty)
			{
			case D_EASY:	go += " depth 2";	break;
			case D_NORMAL:	go += " depth 5";	break;
			case D_HARD:	go += " depth 10";	break;
			}
		}
		string r = uci.Execute(go);
		string::size_type i = r.find("bestmove");
		if(i == string::npos)
			return false;
		i += 9;
		string::size_type j = r.find(" ", i);
		if(j == string::npos)
			j = r.find("\n", i);
		if(j == string::npos)
			return false;
		move = r.substr(i, j - i);
	}
	else
	{
		move = _move;
	}
	uci.Execute("position moves " + move);
	if(uci.res.find("illegal") == string::npos)
	{
		move_count += count(move.begin(), move.end(), ' ') + 1;
		if(!moves.empty())
			moves += " ";
		moves += move;
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
	game_status->Text(GameState());
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
			if(p >= 0 && p <= 5 && MoveSide())
				return AC_ERROR;
			if(p >= 6 && p <= 11 && !MoveSide())
				return AC_ERROR;
			strcat(move, cell);
			board->Selected(cell);
			return AC_MOVE_BEGIN;
		}
	case 2:
		strcat(move, cell);
		if(!MoveSide() && move[1] == '7' && move[3] == '8' && board->Piece() == 5) // white pawn reached end
			return AC_SELECT_PIECE;
		else if(MoveSide() && move[1] == '2' && move[3] == '1' && board->Piece() == 11) // black pawn
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
		ProcessDialogs();
		return true;
	}
	if(state == S_GAME)
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
				piece_selector = xUi::PieceSelector(MoveSide(), board);
				desktop->Insert(piece_selector);
				desktop->Focus(piece_selector);
			}
			break;
		default:
			break;
		}
		return true;
	case 'b':
		if(*board->Selected())
			ApplyCell(NULL);
		else
			OpenMenu();
		return true;
	case 'f':
		OpenMenu();
		return true;
	}
	return false;
}
void eGame::OpenMenu()
{
	menu = xUi::Menu();
	desktop->Insert(menu);
	desktop->Focus(menu);
	UpdateMenu();
}
void eGame::UpdateMenu()
{
	std::string d = difficulty_names[difficulty];
	std::string text = "Difficulty (" + d + ")";
	xUi::MenuUpdateItem(menu, text, "d");
}
bool eGame::Update()
{
	switch(state)
	{
	case S_NONE:
		splash = xUi::Picture("res/splash.png", ePoint2(320, 240));
		desktop->Insert(splash);
		state = S_SPLASH0;
		break;
	case S_SPLASH0:
		state = S_SPLASH;
		break;
	case S_SPLASH:
		start_time = Clock();
		Init();
		state = S_INIT;
		break;
	case S_INIT:
		if(Clock() - start_time > 1000)
		{
			CloseDialog(&splash);
			desktop->Clear();
			desktop->Insert(board);
			desktop->Focus(board);
			desktop->Insert(game_status);
			state = S_GAME;
		}
		break;
	case S_GAME:
		break;
	case S_QUIT:
		break;
	}
	return state != S_QUIT;
}
void eGame::ProcessDialogs()
{
	if(piece_selector && !piece_selector->Text().empty())
	{
		if(piece_selector->Text() != "-")
		{
			if(SelectPiece(piece_selector->Text()[0]))
				Move(NULL);
			else
				ApplyCell(NULL);
		}
		CloseDialog(&piece_selector);
	}
	else if(menu && !menu->Text().empty())
	{
		if(menu->Text() == "nw")
			New(false);
		else if(menu->Text() == "nb")
			New(true);
		else if(menu->Text() == "q")
			state = S_QUIT;
		else if(menu->Text() == "d")
		{
			switch(difficulty)
			{
			case D_EASY:	difficulty = D_NORMAL;	break;
			case D_NORMAL:	difficulty = D_HARD;	break;
			case D_HARD:	difficulty = D_EASY;	break;
			}
			UpdateMenu();
			menu->Text(std::string());
			return;
		}
		CloseDialog(&menu);
	}
}
void eGame::CloseDialog(xUi::eDialog** d)
{
	desktop->Remove(*d);
	desktop->Focus(board);
	(*d)->Destroy();
	delete *d;
	*d = NULL;
}
using namespace std;
static const char* config_name = "spartak-chess.cfg";
static string GetLine(FILE* f)
{
	char l[4096];
	fgets(l, 4096, f);
	int len = strlen(l);
	if(len && l[len - 1] == '\n')
		l[len - 1] = '\0';
	return l;
}
static void PutLine(const string& _l, FILE* f)
{
	string l = _l;
	l += "\n";
	fputs(l.c_str(), f);
}
static void Write(const string& name, const string& value, FILE* f)
{
	string l;
	l += name;
	l += ": ";
	l += value;
	PutLine(l, f);
}
static void Read(const string& name, string& value, FILE* f)
{
	value.clear();
	string l = GetLine(f);
	if(l.find(name + ": ") == 0)
	{
		value = l.substr(name.length() + 2);
	}
}
const char* eGame::difficulty_names[] = { "easy", "normal", "hard", NULL };
void eGame::Load()
{
	FILE* f = fopen(xIo::Resource(config_name), "rb");
	if(!f)
		return;
	string s;
	Read("moves", s, f);
	if(!s.empty())
		Move(s.c_str());
	Read("side", s, f);
	board->Flip(s == "black");
	Read("difficulty", s, f);
	difficulty = D_EASY;
	for(const char** d = difficulty_names; *d; ++d)
	{
		if(s == *d)
			difficulty = (eDifficulty)(d - difficulty_names);
	}
	fclose(f);
}
void eGame::Store()
{
	FILE* f = fopen(xIo::Resource(config_name), "wb");
	if(!f)
		return;
	Write("moves", moves, f);
	Write("side", board->Flip() ? "black" : "white", f);
	Write("difficulty", difficulty_names[difficulty], f);
	fclose(f);
}
