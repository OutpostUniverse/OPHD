// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "FactoryReport.h"


extern Font* MAIN_FONT;	// yuck


FactoryReport::FactoryReport()
{
	init();
}


FactoryReport::~FactoryReport()
{}


void FactoryReport::init()
{
	
	ComboBox* _cb = static_cast<ComboBox*>(addControl("cboTestBox", &cboFilterByProduct, 10, 10));

	_cb->font(*MAIN_FONT);
	_cb->position(100, 100);
	_cb->size(200, 20);

	_cb->addItem("Clothing");
	_cb->addItem("Maintenance Supplies");
	_cb->addItem("Medicine");
	_cb->addItem("Robodigger");
	_cb->addItem("Robodozer");
	_cb->addItem("Roboexplorer");
	_cb->addItem("Robominer");
	_cb->addItem("Road Materials");
	_cb->addItem("Truck");
}


void FactoryReport::update()
{
	UIContainer::update();
	//btnButton1.update();
	//cboFilterByProduct.update();
}

