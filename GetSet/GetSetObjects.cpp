#include "GetSetObjects.h"

#include "StringUtil.hxx"

namespace GetSetObjects {
	
	Object::Object(GetSetInternal::Section& section, GetSetGui::ProgressInterface& _app)
		: GetSetInternal::Dictionary::Observer(section.dictionary)
		, app(_app)
		, dictionary(section.dictionary)
		, path(section.path())
		, ignore_notify(false)
	{}

	GetSetGui::Section Object::gui_section() { return GetSetGui::Section(path,dictionary); }

	void Object::gui_ignore_notify(bool ignore) { ignore_notify=ignore; }
	
	void Object::notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
	{
		if (!ignore_notify && signal==GetSetInternal::Dictionary::Signal::Change)
		{
			// notify only if it concerns our section.
			if (hasPrefix(node.super_section,path))
				gui_notify(path==node.super_section?"":node.super_section.substr(path.size()+1), node);
		}
	}

} // namespace GetSetObjects
