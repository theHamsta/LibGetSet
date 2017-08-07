#include "GetSetObjects.h"

namespace GetSetObjects {
	
	Resident::Resident(GetSetInternal::Section& section)
		: GetSetInternal::Dictionary::Observer(section.dictionary)
		, dictionary(section.dictionary)
		, path(section.path())
		, ignore_notify(false)
	{}

	GetSetGui::Section Resident::gui_section() { return GetSetGui::Section(path,dictionary); }

	void Resident::gui_ignore_notify(bool ignore) { ignore_notify=ignore; }
	
	void Resident::notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
	{
		if (!ignore_notify && signal==GetSetInternal::Dictionary::Signal::Change)
		{
			// Is node located somewhere below our section? If so, not our business.
			if (node.super_section.size()<=path.size()) return; 
			// check if the node is located in our section by checking is path is a prefix of the super_section
			bool prefix_ok=true;
			for (int i=0;i<(int)path.size();i++, prefix_ok)
				prefix_ok&=path[i]==node.super_section[i];
			// If, so, we are hereby notified.
			if (prefix_ok) gui_notify(node.super_section.substr(path.size()), node);
		}
	}

	Object::Object(GetSetGui::Section& section)
		: Resident(section)
	{}
	
	void Object::gui_declare() {
		gui_declare_section(gui_section());
	}

	void Object::gui_retreive() {
		gui_retreive_section(gui_section());
	}

} // namespace GetSetObjects
