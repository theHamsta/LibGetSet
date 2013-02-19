#include "GetSetXML.h"

namespace GetSetInternal
{
	/// A rudimentary non-standard conforming XML representation/parser.
	class UglyXML {
	public:

		struct Tag {
			int from;
			int to;
			std::string tag;
			std::map<std::string,std::string> attrib;
			std::string text;
			std::vector<Tag> tags;
		};
		std::vector<Tag> tags;

		// Try to parse an xml (definition see below)
		bool parse(const std::string& value);

		UglyXML(const std::string& value) { if (!parse(value)) std::cerr << "UglyXML failed.\n"; }
	};

	bool parse(const std::string& value);

} // GetSetInternal


namespace GetSetIO
{

	//
	// XmlFile
	//
	XmlFile::XmlFile(std::istream& input, std::ostream& output) : GetSetInternal::GetSetInOut(input, output) {}


	//
	// XmlFile saving (depends only on STL)
	//

	void XmlFile::write() const
	{
		std::string xml="<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<!--GetSet Description of Property Tree-->\n";
		// A stack of open tags
		std::vector<std::string> openTags;
		std::string indent;
		// Iterate over all keys in contents and write them to xml
		for (MapStrMapStrStr::const_iterator p=contents.begin();p!=contents.end();++p)
		{
			// Find relative path and navigate
			std::vector<std::string> path=stringToVector<std::string>(p->first,'/');
			int match=0;
			for (;match<(int)openTags.size()&&match<(int)path.size();match++)
				if (path[match]!=openTags[match]) break;
			// Close Tags
			int closeTags=(int)openTags.size()-match;
			for (int i=0;i<closeTags;i++)
			{
				indent.erase(indent.end()-2,indent.end());
				xml+=indent+"</Section>\n";
				openTags.pop_back();
			}
			// Open Tags
			for (int open_tags=match;open_tags<(int)path.size()-1;open_tags++)
			{
				openTags.push_back(path[open_tags]);
				xml+=indent+"<Section Name=\""+path[open_tags]+"\"> <!--"+vectorToString(openTags,"/")+"-->\n";
				indent+="  ";
			}
			// Write Key
			{
				xml+=indent+"<Key Name=\""+path.back()+"\"";
				std::string value;
				// concatenate attributes in a attrib="value" style string
				for (MapStrStr::const_iterator attr=p->second.begin();attr!=p->second.end();++attr)
					if (attr->first=="Value") value=attr->second;
					else xml+=" " + attr->first + "=\"" + attr->second + "\"";
				if (value.empty()) xml+="/>\n";
				else xml+=">"+value+"</Key>\n";
			}
		}
		// Close remaining tags (if any)
		while (!openTags.empty())
		{
			indent.erase(indent.end()-2,indent.end());
			xml+=indent+"</Section>\n";
			openTags.pop_back();
		}
		ostr << xml;
	}


	//
	// XmlFile loading (this uses UglyXML)
	//

	// local helper (recursive search of the XML DOM)
	bool walkXml(std::vector<GetSetInternal::UglyXML::Tag>& tags, std::string path, std::map<std::string, std::map<std::string, std::string> > &contents)
	{
		for (int i=0;i<(int)tags.size();i++)
		{
			std::string key=path.empty()?tags[i].attrib["Name"]:path+"/"+tags[i].attrib["Name"];
			if (tags[i].tags.empty())
			{
				if (tags[i].tag!="Key") return false;
				contents[key]=tags[i].attrib;
				contents[key]["Value"]=tags[i].text;
				contents[key].erase(contents[key].find("Name"));
			}
			else if (tags[i].tag!="Section" || !walkXml(tags[i].tags,key,contents))
				return false;
		}
		return true;
	}

	void XmlFile::read()
	{	
		std::string all;
		getline(istr,all,'\0');
		GetSetInternal::UglyXML xml(all);
		if (!walkXml(xml.tags,"",contents))
		{
			contents.clear();
			std::cerr << "XmlFile failed to load. XML syntax error.\n";
		}
	}

} // namespace GetSetIO



//
// Implementation of XML parser:
//


namespace GetSetInternal
{

	bool UglyXML::parse(const std::string& value)
	{
		std::vector<Tag> openTags;
		int start,end=0;
		for (;;)
		{
			// Find next xml tag:
			start=value.find("<",end);
			end=value.find(">",end+1);
			// If no new tag could be found
			if (start<0||end<0)
			{
				// eof was reached, so we better not expect any more close tags
				if (openTags.empty() && start==end) return true;
				tags.clear();
				return 0;
			}
			if (value[start+1]=='?')
				continue; // Skip declarations
			if (value.substr(start+1,3)=="!--")
			{
				/// Skip comments
				end = value.find("-->",start+4);
				if (end>0) end+=3;
				continue;
			}
			if (value[start+1]=='/')
			{
				// This is a close tag.
				std::string closedTag=value.substr(start+2,end-start-2);
				// We expect no more tags but there is another close tag
				if (openTags.empty())
				{
					tags.clear();
					return false;
				}
				// The tag MUST match the most recently opened tag.
				if (openTags.back().tag==closedTag)
				{
					openTags.back().to=start;
					if (openTags.back().tags.empty())
						openTags.back().text=value.substr(openTags.back().from,openTags.back().to-openTags.back().from);
					if (openTags.size()==1)
					{
						tags.push_back(openTags.front());
						openTags.clear();
					}
					else
					{
						openTags[openTags.size()-2].tags.push_back(openTags.back());
						openTags.pop_back();
					}
				}
				else
				{
					// Closed tag does not match opened tag
					tags.clear();
					return false;
				}
			}
			else
			{
				// Open tag: find end of tag text
				int endOfText=value.find_first_of(" \t>",start);
				openTags.push_back(Tag());
				openTags.back().tag=value.substr(start+1,endOfText-start-1);
				openTags.back().from=end+1;
				std::string attribstr=value.substr(endOfText,end-endOfText);
				parseAttribs(attribstr,openTags.back().attrib);
				if (!attribstr.empty() && attribstr.back()=='/')
				{
					// empty element tag <Tag arg="value"/> style
					openTags.back().to=openTags.back().from;
					if (openTags.size()==1)
					{
						tags.push_back(openTags.front());
						openTags.clear();
					}
					else
					{
						openTags[openTags.size()-2].tags.push_back(openTags.back());
						openTags.pop_back();
					}
				}
			}
		}
	}

} // namespace GetSetInternal
