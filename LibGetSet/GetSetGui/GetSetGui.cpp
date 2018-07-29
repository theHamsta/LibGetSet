//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//
//  Copyright (c) by André Aichert (aaichert@gmail.com)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "GetSetGui.h"
#include "GetSetTabWidget.h"
#include "GetSetProgressWindow.h"
#include "../GetSet/GetSetLog.hxx"
#include "../GetSet/GetSetIO.h"
#include "../GetSet/GetSetScripting.h"

#include <QApplication>
#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>
#include "SimpleCommandPaletteWidget.hpp"
#include "FuzzyCommandPaletteEngine.hpp"
#include <QLayout>
#include <QInputDialog>

void gui_update( const std::string&, void* )
{
	QApplication::processEvents();
}

namespace GetSetGui
{

	Application::Application( std::string _appname, GetSetInternal::Dictionary& _dict )
		: dict( _dict )
		, cmd( dict )
		, qt_app( 0x0 )
		, callback( 0x0 )
		, main_window( 0x0 )
		, progress_window( 0x0 )
		, log( new GetSetLog( _appname + ".log" ) )
	{
		GetSet<>( "Application", dict ) = _appname;
		GetSet<>( "ini-File", dict ) = _appname + ".ini";
	}

	Application::~Application()
	{
		delete log;

		if ( callback ) delete callback;

		//		if (main_window) delete main_window; // Crashed on delete on windows (why?)
		//		if (qt_app) delete qt_app; // Crashed on delete on windows (why?)
	}

	GetSetIO::CmdLineParser& Application::commandLine()
	{
		return cmd;
	}

	bool Application::init( int& argc, char** argv, std::function<void( const GetSetInternal::Node& )> gui )
	{
		std::string appname = GetSet<>( "Application", dict );
		qt_app = new QApplication( argc, argv );
		bool single_unhandled_arg = false;

		if ( argc == 2 && ( std::string( argv[1] ) == "--help" || std::string( argv[1] ) == "-h" ) ) {
			std::cerr <<
					  "Usage:\n"
					  "   " << appname << " " << appname << ".ini\n";

			if ( !cmd.getFlags().empty() )
				std::cerr <<
						  "   " << appname << " --parameter value\n"
						  "With:\n"
						  << cmd.getSynopsis();

			return false;
		}
		// Default is to try and load ini-file or run a script
		else if ( argc <= 2 ) {
			std::string arg = argc > 1 ? argv[1] : "";
			std::string ext = splitRight( arg, "." );
			std::transform( ext.begin(), ext.end(), ext.begin(), ::tolower );

			if ( ext == "ini" )
				GetSet<>( "ini-File", dict ) = argv[1];

			loadSettings();
			callback = gui ? new GetSetHandler( gui, dict ) : 0x0;

			// Run script
			if ( ext == "getset" ) {
				std::string script = fileReadString( argv[1] );

				if ( script.empty() ) {
					std::cerr <<
							  "Failed to parse command line arguments!\n"
							  "Try:\n"
							  "   " << appname << " --help\n";
					return false;
				} else return parseScript( script );
			} else
				single_unhandled_arg = ext != "ini";
		}

		// We have multiple command line args or we did not understand the first one.
		if ( argc > 2 || single_unhandled_arg ) {
			if ( !cmd.parse( argc, argv ) ) {
				std::cerr <<
						  "Failed to parse command line arguments!\n"
						  "Try:\n"
						  "   " << appname << " --help\n";
				return false;
			} else if ( cmd.getUnhandledArgs().size() > 1 ) {
				std::cerr << "Unrecognized command line arguments:\n";

				for ( auto it = cmd.getUnhandledArgs().begin(); it != cmd.getUnhandledArgs().end(); ++it )
					std::cout << "   " << it->first << "\t" << it->second << std::endl;

				return false;
			}
		}

		return true;
	}

	void Application::ignoreNotifications( bool ignore )
	{
		if ( callback ) callback->ignoreNotifications( ignore );
	}

	GetSetTabWidget& Application::window()
	{
		if ( !main_window ) {
			main_window = new GetSetTabWidget( 0x0, dict );
			main_window->setWindowFlags( Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint );
		}

		return *main_window;
	}

	GetSetProgressWindow& Application::progress()
	{
		if ( !progress_window ) {
			progress_window = new GetSetProgressWindow();
			progress_window->setWindowTitle( GetSet<>( "Application", dict ).getString().c_str() );
		}

		return *progress_window;
	}

	void Application::progressStart( const std::string& name, const std::string& info, int maximum, bool* cancel_clicked )
	{
		window().hide();
		progress().progressStart( name, info, maximum, cancel_clicked );
	}

	void Application::progressUpdate( int i )
	{
		progress().progressUpdate( i );
	}

	void Application::progressEnd()
	{
		progress().progressEnd();
		window().show();
	}

	void Application::info( const std::string& who, const std::string& what, bool show_dialog )
	{
		progress().info( who, what, show_dialog );
		std::cout << who << ": " << what << std::endl;
	}

	void Application::warn( const std::string& who, const std::string& what, bool non_fatal )
	{
		progress().warn( who, what, non_fatal );
	}

	void Application::saveSettings() const
	{
		GetSetIO::save<GetSetIO::IniFile>( GetSet<>( "ini-File", dict ) );
	}

	void Application::loadSettings()
	{
		GetSetIO::load<GetSetIO::IniFile>( GetSet<>( "ini-File", dict ) );
	}

	bool Application::parseScript( const std::string& script )
	{
		GetSetScriptParser parser( dict );
		parser.addErrorCallback( 0x0, gui_update );
		parser.parse( script, "Script", this );
		return parser.good();
	}

	int Application::exec()
	{
		if ( !qt_app ) {
			std::string appname = GetSet<>( "Application", dict );
			int argc = 1;
			char* argv[] = {&appname[0], 0x0};
			qt_app = new QApplication( argc, argv );
		}

		window().resize( window().sizeHint() );
		window().show();

		// for ( auto& d : dict.getChildren() ) {
		// 	std::cout << d.first << std::endl;
		// }

		std::vector<GetSetInternal::Section::NodesByName> stack;
		stack.push_back( dict.getChildren() );

		auto commandPalette = new SimpleCommandPaletteWidget( &window() );
		// ui->mainToolBar->addWidget( commandPalette );
		commandPalette->setCommandPaletteEngine( new FuzzyCommandPaletteEngine() );
		commandPalette->addActionsFromMenu( window().menuBar() );
		commandPalette->setShortcut( "Ctrl+Shift+P" );
		commandPalette->setPlaceholderText( "Press <Ctrl+Shift+P> to search for commands!" );
		window().layout()->addWidget( commandPalette );

		while ( !stack.empty() ) {
			auto current = stack.back();
			stack.pop_back();

			for ( auto n : current ) {
				// std::cout << n.first << std::endl;
				std::cout << n.second->path() << std::endl;
				std::cout << "\t" << n.second->getString() << std::endl;
				// std::cout << "\t" << n.second->type() << std::endl;
				auto secPointer = dynamic_cast<GetSetInternal::Section*>( n.second );

				if ( secPointer ) {
					stack.push_back( secPointer->getChildren() );
				} else {
					QAction* action = new QAction( "Set " + QString::fromStdString( n.second->path() ) );
					window().connect( action, &QAction::triggered, [ = ]() {
						std::string currentValue = GetSet<>( n.second->path() );
						bool ok;
						QString text = QInputDialog::getText( &window(),   "Set "  + QString::fromStdString( n.second->name  ),
															  "Set " + QString::fromStdString( n.second->path() ) + ":",
															  QLineEdit::Normal,
															  QString::fromStdString( currentValue ), &ok );

						if ( ok && !text.isEmpty() )
							GetSet<>( n.second->path() ) = text.toStdString();
					} );
					commandPalette->addAction( action );
				}
			}
		}



		return qt_app->exec();
	}

} // namespace GetSetGui
