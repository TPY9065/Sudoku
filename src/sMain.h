#pragma once
#include "wx/wx.h"
#include <random>
#include <ctime>
#include <vector>
#include <bitset>
#include <array>
#include <tuple>

class sMain : public wxFrame
{
public:
	sMain();
	~sMain();
private:
	void SetSelectedGrid(wxCommandEvent& event);
	void SetGridNumber(wxCommandEvent& event);
	void Initialize(wxCommandEvent& event);
	void SolveSolution(wxCommandEvent& event);
	void Exit(wxCommandEvent& event);
	void CreateGamePanel();
	void NewGame();
	void GenerateSolution();
	void ShowBoard(bool showAll = false);
	bool TrySolve(int rowStart, int colStart, 
		std::array<std::bitset<9>, 9>& rows, std::array<std::bitset<9>, 9>& cols, std::array<std::bitset<9>, 9>& grids);
	std::tuple<int, int, int> GetEmptyGrid(int row, int col);
	int GetRowID(wxWindowID windowID);
	int GetColID(wxWindowID windowID);
	int GetGridID(wxWindowID windowID);
	void CheckIfGameFinished();
private:
	enum class MODE { GAME, SOLVE };
	const wxSize BOX_SIZE = wxSize(50, 50);
	const wxColor FOCUSING_COLOR = wxColor(200, 200, 200);
	const wxColor NON_FOCUSING_COLOR = wxColor(240, 240, 240);
	int filled = 0;
	MODE mode = MODE::GAME;
	bool solved = false;
	std::random_device rd;
	std::default_random_engine rng;
	std::uniform_int_distribution<> distrib;
	wxButton* selectedGrid = nullptr;
	std::vector<wxButton*> buttons;
	std::vector<wxString> solutions;
};
