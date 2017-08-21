#include "GetSetInternal.h"

namespace GetSetInternal {

	//
	// Node
	//

	Node::Node(Section& _section, const std::string& _name)
		: dictionary(this==&_section?*static_cast<Dictionary*>(this):_section.dictionary)
		, super_section(_section.path())
		, name(_name)
	{}

	std::string Node::path() const
	{
		if (this==&dictionary) return "";
		return super_section.empty()?name:super_section+"/"+name;
	}

	Section& Node::super() const
	{
		Section *super=dynamic_cast<Section*>(dictionary.nodeAt(super_section));
		return super?*super:dictionary;
	}

	void Node::signalChange()
	{
		dictionary.signal(*this, Dictionary::Signal::Change);
	}

	const std::map<std::string,std::string>& Node::getAttributes() const { return attributes; }

	//
	// Section
	//

	Section::Section(Section& super, const std::string& _name)
		: Node(super,_name)
	{}

	Section::~Section() { clear(); }

	const Section::NodesByName& Section::getChildren() const { return children; }


	void Section::removeNode(const std::string& relative_path)
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
		auto it=super_section.children.find(key_name);
		dictionary.signal(*it->second,Dictionary::Signal::Destroy);
		delete it->second;
		super_section.children.erase(it);
	}

	void Section::clear()
	{
		// Delete all children
		for (NodesByName::iterator it=children.begin();it!=children.end();++it)
		{
			Section* section=dynamic_cast<Section*>(it->second);
			if (section) section->clear();
			if (this!=&dictionary)
			{
				dictionary.signal(*it->second,Dictionary::Signal::Destroy);
				delete it->second;
			}
		}
		children.clear();
	}

	void Section::insertNode(Node& new_node)
	{
		auto it=children.find(new_node.name);
		// If an old node exists, store its value and destroy
		std::string old_value;
		if (it!=children.end()) {
			Section * section=dynamic_cast<Section*>(it->second);
			if (section)
				section->clear();
			else if (it->second) old_value=it->second->getString();
			dictionary.signal(*it->second,Dictionary::Signal::Destroy);
			delete it->second;
			children.erase(it);
		}
		children[new_node.name]=&new_node;
		dictionary.signal(new_node, Dictionary::Signal::Create);
		if (!old_value.empty())
			new_node.setString(old_value);
	}

	std::string Section::getType() const { return "Section"; }

	void Section::setString(const std::string& new_value) {}

	std::string Section::getString() const
	{
		if (children.empty()) return "<null>";
		NodesByName::const_iterator it=children.begin();
		std::string ret=it->first; // it->second->getType()
		for (++it;it!=children.end();++it)
			ret+=";"+it->first; // it->second->getType()
		return ret;
	}

	Node* Section::nodeAt(const std::string& relative_path) const
	{
		return nodeAt(stringToVector<>(relative_path,'/',true));
	}

	Node* Section::nodeAt(const std::vector<std::string>& path, int i) const
	{
		if (path.empty()) return &dictionary;
		// Find next key in this section.
		auto it=children.find(path[i]);
		// If it does not exist, path is invalid.
		if (it==children.end()) return 0x0;
		// If it does exist and we are at the end of path, return the node.
		if (i+1>=path.size()) return it->second;
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
		if (i>=path.size()) return *this;
		// Find next key in this section.
		const std::string& next_key=path[i];
		auto it=children.find(next_key);
		// If the next key exist in this section
		if (it!=children.end())
		{
			// We check if the key we found is a section.
			Section* subsection=dynamic_cast<Section*>(it->second);
			// If it is, we recurse
			if (subsection) return subsection->createSection(path,i+1);
			// If not, we delete whatever is there.
			dictionary.signal(*it->second,Dictionary::Signal::Destroy);
			delete it->second;
			children.erase(it);
		}
		// If the key does not exist (anymore) we create a new section and recurse
		Section *subsection=new Section(*this, next_key);
		insertNode(*subsection);
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
			if (*it) (*it)->dictionary=0x0;
	}

	Dictionary::Observer::Observer(const Dictionary& dict) : dictionary(0x0) { attachTo(&dict); }

	void Dictionary::Observer::attachTo(const Dictionary* new_dict) {
		if (new_dict!=dictionary && dictionary)
			dictionary->registered_observers.erase(this);
		dictionary=new_dict;
		if (new_dict)
			dictionary->registered_observers.insert(this);
	}

	Dictionary::Observer::~Observer() { attachTo(0x0); }

	Dictionary& Dictionary::global() {
		/// The instance that holds the global() dictionary. Used whenever a Dictionary is not explicitly specified.
		static Dictionary *_instance=0x0;
		if (!_instance) _instance=new Dictionary();
		return *_instance;
	}

	void Dictionary::signal(const GetSetInternal::Node& node, Dictionary::Signal signal)
	{
		if (registered_observers.size()>0)
			for (auto it=registered_observers.begin();it!=registered_observers.end();++it)
				if (*it) (*it)->notify(node,signal);
	}

} // namespace GetSetInternal

void GetSetHandler::ignoreNotifications(bool ignore)
{
	ignore_notify=ignore;
}

GetSetHandler::GetSetHandler(std::function<void(const GetSetInternal::Node&)> change , const GetSetInternal::Dictionary& subject)
: GetSetInternal::Dictionary::Observer(subject)
	, ignore_notify(false)
	, change_handler_node(change)
{}

void GetSetHandler::notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
{
	// This handler can be deactivated by calling ignoreNotifications(true)
	if (ignore_notify) return;
	// Always ignore signals other than Change.
	if (signal != GetSetInternal::Dictionary::Change) return;
	// Callback
	if (change_handler_node) change_handler_node(node);
}
