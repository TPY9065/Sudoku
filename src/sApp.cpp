#include "sApp.h"
#include "sMain.h"

wxIMPLEMENT_APP(sApp);

sApp::sApp()
{
}

sApp::~sApp()
{
}

bool sApp::OnInit()
{
	sMain* mainFrame = new sMain();
	mainFrame->Show();
	return true;
}
