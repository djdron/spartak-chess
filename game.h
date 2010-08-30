#ifndef __GAME_H__
#define __GAME_H__

#pragma once

class eDialog;
class eBoard;
class eDesktop;

class eGame
{
public:
	eGame();
	~eGame();

	void New();
	bool Move(const char* move = NULL);
	enum eApplyCellResult { AC_ERROR, AC_RESET, AC_MOVE_BEGIN, AC_MOVE_END, AC_SELECT_PIECE };
	eApplyCellResult ApplyCell(const char* cell = NULL);
	bool SelectPiece(char piece);
	bool Command(char cmd);

	eDesktop& Desktop() const { return *desktop; }

protected:
	void UpdateBoardPosition();
	bool Finished() const;
	const char* GameState() const;
	eDesktop* desktop;
	eBoard* board;
	eDialog* piece_selector;
	eDialog* game_status;
	char move[6];
	bool move_side;

	enum eGameState { GS_NONE, GS_CHECK, GS_MATE, GS_STALEMATE, GS_DRAW };
	eGameState game_state;
};

#endif//__GAME_H__
