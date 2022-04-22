#include "sMain.h"
#include "wx/grid.h"
#include "wx/sizer.h"
#include <string>
#include <algorithm>

sMain::sMain() : wxFrame(nullptr, wxID_ANY, "Sudoku", wxDefaultPosition, wxSize(1024, 768)), 
	rd(), rng(rd()), distrib(0, 80), buttons(81), solutions(81, wxEmptyString)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	CreateGamePanel();

	this->SetSizer(sizer);
	this->Layout();

	this->Center();

	NewGame();
}

sMain::~sMain()
{

}

void sMain::SetSelectedGrid(wxCommandEvent& event)
{
	if (selectedGrid == nullptr)
	{
		selectedGrid = (wxButton*)(event.GetEventObject());
		selectedGrid->SetBackgroundColour(FOCUSING_COLOR);
	}
	else if (selectedGrid->GetId() == event.GetId())
	{
		selectedGrid->SetBackgroundColour(NON_FOCUSING_COLOR);
		selectedGrid = nullptr;
	}
	else
	{
		selectedGrid->SetBackgroundColour(NON_FOCUSING_COLOR);
		selectedGrid = (wxButton*)(event.GetEventObject());
		selectedGrid->SetBackgroundColour(FOCUSING_COLOR);
	}
}

void sMain::SetGridNumber(wxCommandEvent& event)
{
	if (selectedGrid != nullptr)
	{
		wxString number = ((wxButton*)(event.GetEventObject()))->GetLabel();
		if (selectedGrid->GetLabel().IsSameAs(wxEmptyString))
		{
			filled += 1;
		}
		selectedGrid->SetLabel(number);
		if (filled == 81)
		{
			CheckIfGameFinished();
		}
	}
}

void sMain::Initialize(wxCommandEvent& event)
{
	mode = MODE::GAME;
	NewGame();
	GenerateSolution();
	ShowBoard();
}

void sMain::SolveSolution(wxCommandEvent& event)
{
	if (mode == MODE::GAME)
	{
		mode = MODE::SOLVE;
		NewGame();
		return;
	}
	GenerateSolution();
	ShowBoard(true);
}

void sMain::Exit(wxCommandEvent& event)
{
	this->Destroy();
}

void sMain::CreateGamePanel()
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* game = new wxPanel(this, wxID_ANY);
	wxBoxSizer* gameSizer = new wxBoxSizer(wxHORIZONTAL);

	// puzzle
	wxPanel* puzzle = new wxPanel(game, wxID_ANY, wxDefaultPosition, BOX_SIZE * 9, wxBORDER_SIMPLE);
	wxGridSizer* largeGrid = new wxGridSizer(3);
	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 3; c++)
		{
			wxPanel* middleGrid = new wxPanel(puzzle, wxID_ANY, wxDefaultPosition, BOX_SIZE * 3, wxBORDER_SIMPLE);
			wxGridSizer* smallGrid = new wxGridSizer(3);
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					wxWindowID id = (i * 9 + j) + (c * 3) + (r * 27);
					buttons[id] = new wxButton(middleGrid, id, wxEmptyString, wxDefaultPosition, BOX_SIZE);
					buttons[id]->SetBackgroundColour(NON_FOCUSING_COLOR);
					buttons[id]->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &sMain::SetSelectedGrid, this);
					smallGrid->Add(buttons[id], 1, wxEXPAND | wxALL);
				}
			}
			middleGrid->SetSizerAndFit(smallGrid);
			middleGrid->Layout();
			largeGrid->Add(middleGrid, 1, wxEXPAND | wxALL);
		}
	}
	puzzle->SetSizerAndFit(largeGrid);
	puzzle->Layout();
	gameSizer->Add(puzzle, 1, wxEXPAND | wxALL);

	// choices
	wxPanel* choices = new wxPanel(game, wxID_ANY, wxDefaultPosition, BOX_SIZE * 3, wxBORDER_SIMPLE);
	wxGridSizer* choiceSizer = new wxGridSizer(3);
	for (int i = 0; i < 9; i++)
	{
		wxButton* btn = new wxButton(choices, wxID_ANY, std::to_string(i + 1), wxDefaultPosition, BOX_SIZE);
		btn->SetBackgroundColour(NON_FOCUSING_COLOR);
		btn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &sMain::SetGridNumber, this);
		choiceSizer->Add(btn, 1, wxEXPAND | wxALL);
	}
	choices->SetSizerAndFit(choiceSizer);
	choices->Layout();
	gameSizer->Add(choices, 0, wxALIGN_CENTER | wxLEFT, 10);

	game->SetSizerAndFit(gameSizer);
	game->Layout();

	// buttons
	wxPanel* buttonsPanel = new wxPanel(this, wxID_ANY);
	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton* start = new wxButton(buttonsPanel, wxID_ANY, "New Game", wxDefaultPosition, wxSize(100, 50));
	wxButton* solve = new wxButton(buttonsPanel, wxID_ANY, "Solve", wxDefaultPosition, wxSize(100, 50));
	wxButton* exit = new wxButton(buttonsPanel, wxID_ANY, "Exit", wxDefaultPosition, wxSize(100, 50));

	start->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &sMain::Initialize, this);
	solve->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &sMain::SolveSolution, this);
	exit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &sMain::Exit, this);

	buttonSizer->Add(start, 1, wxALIGN_CENTER);
	buttonSizer->Add(solve, 1, wxALIGN_CENTER);
	buttonSizer->Add(exit, 1, wxALIGN_CENTER);

	buttonsPanel->SetSizerAndFit(buttonSizer);
	buttonsPanel->Layout();

	sizer->Add(game, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);
	sizer->Add(buttonsPanel, 1, wxEXPAND | wxALL);

	this->SetSizerAndFit(sizer);
	this->Layout();
}

void sMain::NewGame()
{
	selectedGrid = nullptr;
	filled = 0;

	for (int i = 0; i < 81; i++)
	{
		buttons[i]->SetLabel(wxEmptyString);
		buttons[i]->SetBackgroundColour(NON_FOCUSING_COLOR);
		buttons[i]->Enable(true);
		solutions[i] = wxEmptyString;
	}
}

void sMain::GenerateSolution()
{
	std::array<std::bitset<9>, 9> rows{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<9>, 9> cols{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<9>, 9> grids{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	// record the existing numbers on board
	if (mode == MODE::SOLVE)
	{
		for (int r = 0; r < 9; r++)
		{
			for (int c = 0; c < 9; c++)
			{
				if (!buttons[r * 9 + c]->GetLabel().IsSameAs(wxEmptyString))
				{
					int number;
					buttons[r * 9 + c]->GetLabel().ToInt(&number);
					rows[r].set(number - 1);
					cols[c].set(number - 1);
					grids[r / 3 * 3 + c / 3].set(number - 1);
					solutions[r * 9 + c] = buttons[r * 9 + c]->GetLabel();
				}
			}
		}
	}

	solved = TrySolve(0, 0, rows, cols, grids);
}

void sMain::ShowBoard(bool showAll)
{
	if (!solved)
	{
		wxMessageBox("There is no solution!");
		return;
	}

	if (showAll)
	{
		for (int i = 0; i < 81; i++)
		{
			buttons[i]->SetLabel(solutions[i]);
			buttons[i]->SetBackgroundColour(FOCUSING_COLOR);
			buttons[i]->Enable(false);
		}
		filled = 81;
		return;
	}

	while (filled < 17)
	{
		int pos = distrib(rng);
		if (buttons[pos]->GetLabel().IsSameAs(wxEmptyString))
		{
			buttons[pos]->SetLabel(solutions[pos]);
			buttons[pos]->SetBackgroundColour(FOCUSING_COLOR);
			buttons[pos]->Enable(false);
			filled += 1;
		}
	}
}

bool sMain::TrySolve(int rowStart, int colStart, 
	std::array<std::bitset<9>, 9>& rows, std::array<std::bitset<9>, 9>& cols, std::array<std::bitset<9>, 9>& grids)
{
	int row = -1, col = -1, grid = -1;
	std::tie(row, col, grid) = GetEmptyGrid(rowStart, colStart);
	
	if (row == 9)
		return true;

	std::bitset<9> contains = (rows[row] | cols[col] | grids[grid]);
	if (contains.all())
		return false;

	std::vector<int> numbers{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::shuffle(numbers.begin(), numbers.end(), rng);
	for (int number : numbers)
	{
		wxString numberStr = std::to_string(number);
		if (!contains[number - 1])
		{
			solutions[row * 9 + col] = numberStr;
			rows[row].set(number - 1);
			cols[col].set(number - 1);
			grids[grid].set(number - 1);
			if (TrySolve(row, col, rows, cols, grids))
			{
				return true;
			}
			rows[row].reset(number - 1);
			cols[col].reset(number - 1);
			grids[grid].reset(number - 1);
		}
	}
	solutions[row * 9 + col] = wxEmptyString;
	return false;
}

std::tuple<int, int, int> sMain::GetEmptyGrid(int row, int col)
{
	for (int r = row; r < 9; r++)
	{
		for (int c = 0; c < 9; c++)
		{
			if (solutions[r * 9 + c].IsSameAs(wxEmptyString))
			{
				wxWindowID windowID = buttons[r * 9 + c]->GetId();
				return std::tuple<int, int, int>(GetRowID(windowID), GetColID(windowID), GetGridID(windowID));
			}
		}
	}
	return std::tuple<int, int, int>(row + 1, 0, row / 3 * 3 + col / 3);
}

int sMain::GetRowID(wxWindowID windowID)
{
	return windowID / 9;
}

int sMain::GetColID(wxWindowID windowID)
{
	return windowID % 9;
}

int sMain::GetGridID(wxWindowID windowID)
{
	int rowID = GetRowID(windowID);
	int colID = GetColID(windowID);
	return rowID / 3 * 3 + colID / 3;
}

void sMain::CheckIfGameFinished()
{
	std::array<std::bitset<9>, 9> rows{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<9>, 9> cols{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<std::bitset<9>, 9> grids{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	for (int r = 0; r < 9; r++)
	{
		for (int c = 0; c < 9; c++)
		{
			int number;
			buttons[r * 9 + c]->GetLabel().ToInt(&number);
			rows[r].set(number - 1);
			cols[c].set(number - 1);
			grids[r / 3 * 3 + c / 3].set(number - 1);
		}
	}
	
	std::bitset<9> contains;
	for (int n = 0; n < 9; n++)
	{
		contains = rows[n] & cols[n] & grids[n];
		if (!contains.all())
			return;
	}

	wxMessageBox("Congratulations!");
}


