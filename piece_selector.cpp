#include "board.h"

class ePieceSelector : public eDialog
{
public:
	ePieceSelector(bool _side, eBoard* _board) : side(_side), board(_board), selected(1) {}
	virtual void Paint(eBufferRGBA& buf);
	virtual bool Command(char cmd);
	bool side;
	eBoard* board;
	char selected;
};
void ePieceSelector::Paint(eBufferRGBA& buf)
{
	for(int j = 0; j < buf.Size().y; ++j)
		for(int i = 0; i < buf.Size().x; ++i)
			buf[ePoint2(i, j)] = buf[ePoint2(i, j)] + eRGBA(0, 0, 0, 0x80);
	ePoint2 pos(68, 100);
	font->DrawText(buf, pos, "Select piece:");
	pos.y += 16;
	for(char p = 1; p < 5; ++p)
	{
		eBoard::eCellView view = board->CELL_NORMAL;
		if(selected == p)
			view = board->Flash() ? board->CELL_LIGHT : board->CELL_DARK;
		board->DrawCell(buf, pos, p % 2, view, p + side*6);
		pos.x += board->PIECE_SIZE;
	}
}
bool ePieceSelector::Command(char cmd)
{
	switch(cmd)
	{
	case 'l':
		if(selected > 1)
			--selected;
		return true;
	case 'r':
		if(selected < 4)
			++selected;
		return true;
	case 'a':
		switch(selected)
		{
		case 1:	text = "q";	break;
		case 2:	text = "r";	break;
		case 3:	text = "n";	break;
		case 4:	text = "b";	break;
		}
		return true;
	case 'b':
		text = "-";
		return true;
	}
	return true;
}

eDialog* PieceSelector(bool side, eBoard* board)
{
	eDialog* ps = new ePieceSelector(side, board);
	ps->Create();
	return ps;
}
