
#include "Process.h"
#include "QClientWindow.h"

#include <QMessageBox>

/// Class representing a command line tool that understands the "--xml" argument (FIXME clean up)
class ConfigureProcess : public Process {
public:


	// FIXME write setters and have a simple c-tor
	ConfigureProcess(const std::string& executable_path="",
					const std::string& config_file_path="client.ini",
					const std::string& log_file_path="",
					const std::string& cmdlinearg_run="",
					const std::string& cmdlinearg_cfg="")
		: Process(executable_path)
		, config_file(config_file_path)
		, log_file(log_file_path)
		, config_cmdline_run(cmdlinearg_run)
		, config_cmdline_cfg(cmdlinearg_cfg)
		, window(0x0)
		, handler(configuration,ConfigureProcess::gui)
	{
		_instance=this; // FIXME hack for callbacks
	}

	~ConfigureProcess() {
		closeWindow();
	}
	
	void closeWindow()
	{
		if (window)
		{
			window->close();
			delete window;
		}	
	}

	/// Open Configuration Window
	GetSetSettingsWindow* configure()
	{
		closeWindow();
		if (!setCommanLineArgs("--xml "+config_cmdline_cfg).run())
			return 0x0;
		waitForExit();
		std::istringstream xml(getConsoleOutput());
		GetSetIO::XmlFile xmlConfig(xml,std::cout);
		configuration.load(xmlConfig);
		GetSetIO::load<GetSetIO::IniFile>(config_file,configuration);
		if (configuration.empty())
			return 0x0;
		window=new GetSetSettingsWindow("",configuration, binaryFile);
		return window;
	}

	// This is a real hack. FIXME only one instance at a time is possible
	static ConfigureProcess* _instance;
	static void callback(const std::string&, const std::string& )
	{
		if (_instance)
			_instance->kill();
	}

	static void gui(const std::string& section, const std::string& key)
	{
//		std::string command=GetSet<>(section,key,_instance->configuration).getAttribute("ShellExecute");
//		if (!command.empty())
//		{
//			std::cout << "> " << command << std::endl;
//			ShellExecute(0x0,command.c_str(),0x0,0x0,_instance->working_dir.empty()?0x0:_instance->working_dir.c_str(),SW_SHOW);
//		}
	}

    /// This overload always blocks until termination of child. Implementation is platform specific
    /// (see _WIN32 and __linux__ at bottom of file)
    virtual int run();

	bool handleControlCommand(const std::string& command)
	{
		// Command format: ("###" already removed by called)
		// ### <type> - <identifier> : <action> - <data> 
		// where identifier is the window title
		// Examples:
		//  Display text
		//    ### Info - My Window : - Text here
		//    ### Info - My Window : hide -
		//  Display progress bar
		//    ### Progress - My Window : status - 5/10
		//    ### Progress - My Window : hide -
		//  Displaying error messages (please use sparingly when program is about to exit)
		//    ### Message - Error Loading Image : error - Failed to open input file
		//    ### Message - Solved : info - Results have been saved!
		//  Closing a window (hides anything that was shown using identifier "My Window")
		//    ### Window - My Window : hide -

		std::istringstream str(command);
		std::string type, identifier, action, data;
		
		getline(str,type,'-');
		getline(str,identifier,':');
		trim(type);
		trim(action);
		getline(str,action,'-');
		getline(str,data,'\0');

		trim(type);
		trim(identifier);
		trim(action);
		trim(data);
		
		if (type=="Window" && action=="hide")
		{
			if (client_gui.find(identifier)!=client_gui.end() && client_gui[identifier]!=0x0)
			{
				delete client_gui[identifier];
				client_gui[identifier]=0x0;
			}
			return true;
		}

		if (type=="Message")
			if (action=="error")
				return -1!=QMessageBox::warning(0x0,identifier.c_str(),data.c_str(),QMessageBox::Ok);
			else
				return -1!=QMessageBox::information(0x0,identifier.c_str(),data.c_str(),QMessageBox::Ok);

		QClientWindow *w=0x0;
		if (client_gui.find(identifier)!=client_gui.end() && client_gui[identifier]!=0x0)
		{
			w=dynamic_cast<QClientWindow*>(client_gui[identifier]);
			if (!w) return false;
		}
		else
		{
			client_gui[identifier]=w=new QClientWindow(ConfigureProcess::callback);
			w->setWindowTitle(identifier.c_str());
			w->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint);
			w->show();
		}

		if (type=="Progress")
		{
			w->progress_bar->show();
			if (action=="status")
			{
				std::vector<int> status=stringToVector<int>(data,'/');
				if (status.size()>0) w->progress_bar->setValue(status[0]);
				if (status.size()>1) w->progress_bar->setMaximum(status[1]);
			}
			if (action=="hide")
				w->progress_bar->hide();
			return true;
		}

		if (type=="Info")
		{
			if (action=="set" || action.empty())
				w->info->setText(data.c_str());
			if (action=="hide")
				w->info->hide();			else
				w->info->show();
			return true;
		}

		return false;
	}



protected:
	bool good;									//< True if the XML description was retreived successfully
	std::string				working_dir;		//< Path to working directory
	std::string				config_file;		//< Path to an ini-file to be written and passed to client
	std::string				log_file;			//< Path to a file to which stdout of client will be piped
	std::string				config_cmdline_cfg;	//< Arguments passed to client along with the --xml flag
	std::string				config_cmdline_run;	//< Additional command line arguments passed to the client 
	GetSetDictionary		configuration;		//< Dictionary created from the XML description and GUI
	GetSetSettingsWindow*	window;				//< The GUI to configure the client process.
	std::map<std::string,QWidget*> client_gui;	//< Runtime client GUI (progess bars, dialogs etc.)
	GetSetHandler			handler;			//< call the gui(...) callback when configuration changes
};


ConfigureProcess* ConfigureProcess::_instance=0x0;


#ifdef __linux__
#include <QThread>

/// This overload always blocks until termination of child. requires windows.h... FIXME not a very smart implementation either
int ConfigureProcess::run()
{
    if (window)
        window->hide();
    GetSetIO::save<GetSetIO::IniFile>(config_file,configuration);
    if (!setCommanLineArgs(std::string("\"")+config_file+"\" "+config_cmdline_run).run())
        std::cout << "Failed to run process!\n";
    else
    {
        std::ofstream log(log_file.c_str());
        stdOutput.clear();
        std::string line;
        int nfound=0;
        while (child && *child)
        {
            int available=child->rdbuf()->in_avail();
            if (!available)
            {
                QApplication::processEvents();
                QThread::msleep(30);
                continue;
            }
            std::string str;
            *child >> str;
            if (log.good()) log << str;

            int i=0;
            while (str[i]!=0 && (str[i]==' '||str[i]=='\t')) i++;
            if (str[i]=='#'&&str[i+1]=='#'&&str[i+2]=='#')
            {
                handleControlCommand(line);
                line.clear();
            }
            std::cout << line;
            line.clear();
        }
    }
    int ret=waitForExit();
    for (std::map<std::string,QWidget*>::iterator it=client_gui.begin();it!=client_gui.end();++it)
        if (it->second) delete it->second;
    client_gui.clear();
    if (window)	window->show();
    return ret;
}
#endif


#ifdef _WIN32
#include <windows.h>

/// This overload always blocks until termination of child. requires windows.h... FIXME not a very smart implementation either
int ConfigureProcess::run()
{
    if (window)
        window->hide();
    GetSetIO::save<GetSetIO::IniFile>(config_file,configuration);
    if (!setCommanLineArgs(std::string("\"")+config_file+"\" "+config_cmdline_run).run())
        std::cout << "Failed to run process!\n";
    else if (stdoutReadHandle)
    {
        std::ofstream log(log_file.c_str());
        stdOutput.clear();
        DWORD bytes_read;
        char tBuf[256];
        std::string line;
        int nfound=0;
        while (1)
        {
            DWORD available=0;
            if (0==PeekNamedPipe(stdoutReadHandle,0x0,0x0,0x0,&available,0x0) && available==0)
                break; // eof
            if (!available)
            {
                QApplication::processEvents();
                Sleep(30);
                continue;
            }
            ReadFile(stdoutReadHandle, tBuf, 255, &bytes_read, NULL);
            tBuf[bytes_read]=0;
            if (log.good()) log << tBuf;
            for (int i=0;i<(int)bytes_read;i++)
            {
                line.push_back(tBuf[i]);
                if (nfound==-1)
                {
                    if (tBuf[i]=='\n')
                    {
                        handleControlCommand(line);
                        line.clear();
                        nfound=0;
                    }
                }
                else
                {
                    // "###" indicates a control command
                    if (tBuf[i]=='#') nfound++;
                    if (nfound==3)
                    {
                        line.clear();
                        nfound=-1;
                    }
                    if (tBuf[i]=='\n')
                    {
                        std::cout << line;
                        line.clear();
                    }
                }
            }

        }
        stdoutReadHandle=0x0;
    }
    int ret=waitForExit();
    for (std::map<std::string,QWidget*>::iterator it=client_gui.begin();it!=client_gui.end();++it)
        if (it->second) delete it->second;
    client_gui.clear();
    if (window)	window->show();
    return ret;
}
#endif


