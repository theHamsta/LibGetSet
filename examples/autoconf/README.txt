This is an example for the use of the GetSet config and AutoConf utilities.

AutoConf
A utility to edit ini-files. It searches for an xml-File of the same name as the ini-File. If it exists, it uses the types and standard values from the xml-File.
Notice the "--button caption" argument to add a button to the configuration window. There is a seperate example for AutoConf in the GetSet/examples/AutoConf directory.

config
A utility to edit ini-Files and to parse strings and find and replace "GetSet[key]" variables. Every occurence will be replaced with the value of the respective variable from the ini-File.
Notice specifically the "run" verb, which parses and executes a short command supplied to it.


How to use these Utilities to create a  GUI for a command line tool:

1) Create an xml-file containing all the variables that are needed to configure the CLI tool
2) Write a batch file which
	2.1) first executes AutoConf with the respective ini-File as argument
	2.2) second calls "config run" with the ini-File and a call to the CLI-tool with correct command line syntax
	2.3) optionally, uses some scripting to allow more modes and handles a "Run" button in AutoConf correctly.

___________________________________________________________________

AN EVEN SIMPLER EXAMPLE : ping.exe on windows

create the following XML file ping.xml

<?xml version="1.0" encoding="UTF-8" ?>
<Section Name="Config">
  <Key Name="IP" Type="string" Value="127.0.0.1"/>
  <Key Name="Number of Pings" Type="int" Value="3"/>
</Section>

You can now use AutoConf to create and edit the ini-File ping.ini
Then create a windows batch file ping_gui.bat .

First of all, we could like to run ping.exe with the properties from ping.ini file
config run ping.ini "ping GetSet[Config/IP] -n GetSet[Config/Number of Pings]"

This should take the value "Config/IP" and "Config/Number of Pings" from ping.ini
And replace the respective GetSet[...] passages. You end up with something like:

ping 127.0.0.1 -n 3

If executed, this should ping local host 3 times.
___________________________________________________________________
