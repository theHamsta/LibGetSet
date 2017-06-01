#include <iostream>
#include <functional>

#include <GetSet/GetSet.hxx>
#include <GetSetGui/GetSetGui.h>
#include <GetSetGui/GetSetTabWidget.h>

// #include <GetSet/GetSetLocalDictionary.hxx>

/// Application
GetSetGui::GetSetApplication g_app("Test");

// Managing saving/loading parametzers and automatic GUI
#include <GetSetGui/GetSetTabWidget.h>

void gui(const std::string& section, const std::string& key)
{
	std::cout << key << std::endl;

	if (section=="Test/Range")
	{
		std::cout << "Reset range.\n";
		GetSetGui::RangedDouble("Test/Value")
			.setMin(GetSet<int>("Test/Range/Min"))
			.setMax(GetSet<int>("Test/Range/Max"))
			;
	}

	g_app.saveSettings();
}

int main(int argc, char **argv)
{

	GetSet<int>("Test/Range/Min")=5;
	GetSet<int>("Test/Range/Max")=9;

	GetSetGui::RangedDouble("Test/Value")
		.setMin(GetSet<int>("Test/Range/Min"))
		.setMax(GetSet<int>("Test/Range/Max"))
		;
	g_app.init(argc,argv,gui);
	g_app.window().addDefaultFileMenu();
	g_app.window().aboutText()=
		"<h4>Tracking unknown objects under fluoroscopy.</h4>\n\n"
		"Copyright 2014-2017 by <a href=\"mailto:aaichert@gmail.com?Subject=[Epipolar Consistency]\">Andre Aichert</a> <br>"
		"<h4>Epipolar Consistency:</h4>\n\n"
		"Any two ideal transmission images with perfectly known projection geometry contain redundant information. "
		"Any inconsistencies, e.g., motion, truncation, scatter radiation or beam-hardening can be observed using Epipolar Consistency. "
		"<br>"
		"<br>"
		"See also: "
		"<br>"
		"<a href=\"https://www5.cs.fau.de/research/software/epipolar-consistency/\">Pattern Recognition Lab at Technical University of Erlangen-Nuremberg</a> "
		"<br>"
		"<h4>Licensed under the Apache License, Version 2.0 (the \"License\")</h4>\n\n"
		"You may not use this file except in compliance with the License. You may obtain a copy of the License at "
		"<a href=\"http://www.apache.org/licenses/LICENSE-2.0\">http://www.apache.org/licenses/LICENSE-2.0</a><br>"
		;
	return g_app.exec();
}
