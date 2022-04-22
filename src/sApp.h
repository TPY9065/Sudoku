#pragma once
#include "wx/wx.h"

class sApp : public wxApp
{
public:
	sApp();
	~sApp();
public:
	virtual bool OnInit();
};

