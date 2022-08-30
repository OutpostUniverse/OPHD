#pragma once

#include <string>


void doNonFatalErrorMessage(const std::string& title, const std::string& msg);
void doAlertMessage(const std::string& title, const std::string& msg);
bool doYesNoMessage(const std::string& title, const std::string msg);
