
#include <iostream>
#include <fstream>
#include <streambuf>

namespace GetSetInternal
{
	// Yay to Dietmar Kühl for this simple and valuable snippet
	// https://groups.google.com/forum/#!msg/comp.lang.c++/AqglPADy_vQ/gQ3qJg8clB0J

	// Write to two output streams without buffering.
	class teebuf: public std::streambuf {
	public:
		typedef std::char_traits<char> traits_type;
		typedef traits_type::int_type  int_type;

		teebuf(std::streambuf* _sb1, std::streambuf* _sb2)
			: sb1(_sb1)
			, sb2(_sb2)
		{}
		
	private:
		std::streambuf* sb1;
		std::streambuf* sb2;

		int_type overflow(int_type c)
		{
			if (!traits_type::eq_int_type(c, traits_type::eof()))
			{
				c = sb1->sputc(c);
				if (!traits_type::eq_int_type(c, traits_type::eof()))
					c = sb2->sputc(c);
				return c;
			}
			else return traits_type::not_eof(c);
		}
		
		int sync()
		{
			int rc = sb1->pubsync();
			if (rc != -1)
				rc = sb2->pubsync();
			return rc;
		}
		
	};
} // namespace GetSetInternal

/// A utility class for creating log files. Also includes a SignalDebugger for detailed logs.
class GetSetLog {
	private:	
	std::ofstream			log_file;
	std::streambuf*			cout_sbuf;
	std::streambuf*			cerr_sbuf;
	GetSetInternal::teebuf	tee_buf_err;
	GetSetInternal::teebuf	tee_buf_out;
	public:

	/// A utility class for debugging GetSet applications
	class SignalDebugger : public GetSetInternal::Dictionary::Observer {
	public:
		SignalDebugger(const GetSetInternal::Dictionary& dict=GetSetInternal::Dictionary::global()) : GetSetInternal::Dictionary::Observer(dict) {}
		void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
		{
			if (signal==GetSetInternal::Dictionary::Signal::Change)
				std::cout << node.path() << " = " << node.getString() << std::endl;
			else if (signal!=GetSetInternal::Dictionary::Signal::Attrib)
				std::cout << (signal==GetSetInternal::Dictionary::Signal::Create?"+ Create ":"- Destroy") << "  (" << node.getType() << ")\t" << node.path() << std::endl;
		}
	};
	
	GetSetLog(const std::string& file) 
	 : log_file(file)
	 , cout_sbuf(std::cout.rdbuf())
	 , cerr_sbuf(std::cerr.rdbuf())
	 , tee_buf_out(log_file.rdbuf(), cout_sbuf)
	 , tee_buf_err(log_file.rdbuf(), cerr_sbuf)
	{
		if (!log_file || !log_file.is_open())
		{
			std::cerr << "Failed to open logfile " << file << ".\n";
			cout_sbuf=0x0;
		}
		else
		{
			std::cout.rdbuf(&tee_buf_out); // redirect 'cout'
			std::cerr.rdbuf(&tee_buf_err); // redirect 'cerr'
		}
	}
	
	static void debugSignals() {
		static SignalDebugger* log=0x0;
		if (!log) log=new SignalDebugger();
	}

	~GetSetLog()
	{
		if (cout_sbuf)
		{
			cout_sbuf->pubsync();
			cerr_sbuf->pubsync();
			// restore the original stream buffers
			std::cout.rdbuf(cout_sbuf); 
			std::cerr.rdbuf(cerr_sbuf);
		}
	}
};
