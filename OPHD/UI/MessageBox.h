#pragma once

#include <string>


void doNonFatalErrorMessage(const std::string& title, const std::string& message);
void doAlertMessage(const std::string& title, const std::string& message);
bool doYesNoMessage(const std::string& title, const std::string& message);
