#include "GetSetInternal.h"

namespace GetSetInternal {

	//
	// Node
	//

	Node::Node(Dictionary& _dictionary, const std::string& _super_section, const std::string& _name)
		: dictionary(_dictionary)
		, super_section(_super_section)
		, name(_name)
	{
		dictionary.signal(*this, Dictionary::Signal::Create);
	}
	
	Node::~Node() {
		dictionary.signal(*this, Dictionary::Signal::Destroy);
	}

	std::string Node::path() const
	{
		return super_section.empty()?name:super_section+"/"+name;
	}

	void Node::signalChange()
	{
		dictionary.signal(*this, Dictionary::Signal::Destroy);
	}

	void Node::signalAttribChange()
	{
		dictionary.signal(*this, Dictionary::Signal::Destroy);
	}
 
	//
	// Section
	//

	const Section::NodesByName& Section::getSection() const
	{
		return properties;
	}

	Section::~Section()
	{
		// Delete all children
		for (NodesByName::iterator it=properties.begin();it!=properties.end();++it)
			delete it->second;
		properties.clear();
	}

		
	void Section::remove(const std::string& relative_path)
	{
		// Get relative path in vector form
		auto path=stringToVector<>(relative_path,'/',true);
		// Make sure that path is non empty
		if (path.empty()) return;
		// Make sure that relative_path points to a Node
		Node* node=nodeAt(path);
		if (!node) return;
		// Get key name
		std::string key_name=path.back();
		// Get an existing section, which contains key_name
		path.pop_back();
		Section& super_section=createSection(path);
		auto it=super_section.properties.find(key_name);
		delete it->second;
		super_section.properties.erase(it);
	}
		
	bool Section::exists(const std::string relative_path) const
	{
		return nodeAt(relative_path)!=0x0;
	}
		
	bool Section::isValue(const std::string relative_path) const
	{
		// Make sure that relative_path points to a Node
		Node* node=nodeAt(relative_path);
		// If it does exist it is not a value
		if (!node) return false;
		// If it is a section, it does not have a value.
		if (dynamic_cast<Section*>(node)) return false;
		//  Button, StaticText etc. have set attribute isValue=false, so they are not saved to ini-Files
		return node->getAttribute<bool>("isValue");
	}

	std::string Section::absolutePath(const std::string relative_path) const
	{
		return (this==&dictionary)?relative_path:path()+"/"+relative_path;
	}
		
	std::string Section::getType() const {return "Section";}

	void Section::setString(const std::string& new_value) {}

	std::string Section::getString() const
	{
		if (properties.empty()) return "<null>";
		NodesByName::const_iterator it=properties.begin();
		std::string ret=it->first; // it->second->getType()
		for (++it;it!=properties.end();++it)
			ret+=";"+it->first; // it->second->getType()
		return ret;
	}

	Node* Section::nodeAt(const std::string& relative_path) const
	{
		return nodeAt(stringToVector<>(relative_path,'/',true));
	}

	Node* Section::nodeAt(const std::vector<std::string>& path, int i) const
	{
		// Find next key in this section.
		auto it=properties.find(path[i]);
		// If it does not exist, path is invalid.
		if (it==properties.end()) return 0x0;
		// If it does exist and we are at the end of path, return the node.
		if (i+1>=properties.size()) return it->second;
		// Otherwise, we check if the key we found is a section.
		Section* subsection=dynamic_cast<Section*>(it->second);
		// If it is not, then path is also invalid.
		if (!subsection) return 0x0;
		// If it is, we continue following the path in the subsection.
		return subsection->nodeAt(path,i+1);
	}

	Section& Section::createSection(const std::string& relative_path)
	{
		return createSection(stringToVector<>(relative_path,'/',true));
	}
	
	Section& Section::createSection(const std::vector<std::string>& path, int i)
	{
		// If we have reached the end of path, we are done.
		if (i>=properties.size()) return *this;
		// Find next key in this section.
		const std::string& next_key=path[i];
		auto it=properties.find(next_key);
		// If the next key exist in this section
		if (it!=properties.end())
		{
			// We check if the key we found is a section.
			Section* subsection=dynamic_cast<Section*>(it->second);
			// If it is, we recurse
			if (subsection) return subsection->createSection(path,i+1);
			// If not, we delete whatever is there.
			delete it->second;
			properties.erase(it);
		}
		// If the key does not exist (anymore) we create a new section and recurse
		Section *subsection=new Section(*this, next_key);
		properties[next_key]=subsection;
		return subsection->createSection(path, i+1);
	}
		

	//
	// Dictionary
	//

	Dictionary::Dictionary() : GetSetInternal::Section(*this,"") {}

	Dictionary::~Dictionary()
	{
		// Disable observers.
		for (auto it=registered_observers.begin();it!=registered_observers.end();++it)
			(*it)->dictionary=0x0;
	}

	Dictionary::Observer::Observer(const Dictionary& dict) : dictionary(&dict)
	{
		dictionary->registered_observers.insert(this);
	}

	Dictionary::Observer::~Observer()
	{
		if (dictionary) dictionary->registered_observers.erase(this);
	}

	void Dictionary::clear() {
		for (auto it=properties.begin();it!=properties.end();++it)
			if (it->second) delete it->second;
		properties.clear();
	}

	Dictionary& Dictionary::global() {
		/// The instance that holds the global() dictionary. Used whenever a Dictionary is not explicitly specified.
		static Dictionary *_instance=0x0;
		if (!_instance) _instance=new Dictionary();
		return *_instance;
	}

	void Dictionary::signal(const GetSetInternal::Node& node, Dictionary::Signal signal)
	{
		for (auto it=registered_observers.begin();it!=registered_observers.end();++it)
			if (*it) (*it)->notify(node,signal);
	}


} // namespace GetSetInternal

void GetSetHandler::ignoreNotifications(bool ignore)
{
	ignore_notify=ignore;
}

GetSetHandler::GetSetHandler(void (*change)(const GetSetInternal::Node&), const GetSetInternal::Dictionary& subject)
	: GetSetInternal::Dictionary::Observer(subject)
	, change_handler_node(change)
	, change_handler_section_key(0x0)
{}

GetSetHandler::GetSetHandler(void (*change)(const std::string& section, const std::string& key), const GetSetInternal::Dictionary& subject)
	: GetSetInternal::Dictionary::Observer(subject)
	, change_handler_node(0x0)
	, change_handler_section_key(change)
{}

void GetSetHandler::notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
{
	if (change_handler_section_key)
		change_handler_section_key(node.super_section,node.name);
	if (change_handler_node)
		change_handler_node(node);
}
