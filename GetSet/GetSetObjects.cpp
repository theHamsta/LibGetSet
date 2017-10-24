#include "GetSetObjects.h"

#include "StringUtil.hxx"


#include <type_traits>
#include <stdexcept>

namespace GetSetGui {
	
	ProgressInterface& default_progress_interface() {
		static ProgressInterface progress;
		return progress;
	}
	
	// This is the only place we need direct access to GetSetInternal things. Subclasses need not be aware.
	Object::Object(const Section& section, ProgressInterface *_app)
		: GetSetInternal::Dictionary::Observer(((GetSetInternal::Section&)section).dictionary)
		, app(_app?_app:&default_progress_interface())
		, dictionary(((GetSetInternal::Section&)section).dictionary)
		, path(((GetSetInternal::Section&)section).path())
		, ignore_notify(true)
	{}

	Section Object::gui_section() const { return Section(path,dictionary); }

	void Object::gui_init() {
		auto *obj=dynamic_cast<Configurable*>(this);
		if (obj) obj->gui_declare_section(gui_section());
		gui_ignore_notify(false);
	}

	void Object::gui_ignore_notify(bool ignore) const { ignore_notify=ignore; }
	
	void Object::notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal)
	{
		if (!ignore_notify && signal==GetSetInternal::Dictionary::Signal::Change)
		{
			// notify only if it concerns our section.
			if (hasPrefix(node.super_section,path))
				gui_notify(path==node.super_section?"":node.super_section.substr(path.size()+1), node);
		}
	}
	
} // namespace GetSetGui


// 
// Object Factory (use is optional)
// 

namespace GetSetInternal {
	/// Instance which holds all known objects.
	std::map<std::string, GetSetGui::Object* (*)(const GetSetGui::Section&, GetSetGui::ProgressInterface&)> factory_registration;

	void factory_register_type(const std::string& class_name, GetSetGui::Object* (*instantiate)(const GetSetGui::Section&, GetSetGui::ProgressInterface&))
	{
		// Make sure that the type class_name is unambigous
		if (factory_registration.find(class_name)!=factory_registration.end()) {
			if (!instantiate) factory_registration.erase(factory_registration.find(class_name));
			else {
                std::string s=std::string(__FUNCTION__) + " : The specified type " + class_name + " has already been registered!";
                throw std::runtime_error( s);
            }
		}
		else
		// Then just register
		factory_registration[class_name]=instantiate;
	}

} // namespace GetSetInternal

namespace GetSetGui {
	
	/// Default ProgressInterface istance and pointer
	ProgressInterface  default_interface;
	ProgressInterface *default_interface_ptr=&default_interface;

	void progress_set_default_interface(ProgressInterface& app) {
		default_interface_ptr=&app;
	}

	ProgressInterface& progress_default_interface() {
		return *default_interface_ptr;
	}

	std::set<std::string> factory_known_types() {
		std::set<std::string> ret;
		for (auto it=GetSetInternal::factory_registration.begin();it!=GetSetInternal::factory_registration.end();++it)
			ret.insert(it->first);
		return ret;
	}

	GetSetGui::Object* factory_create(const std::string& class_name, const Section& section, ProgressInterface& progress_interface) {
		auto   it=GetSetInternal::factory_registration.find(class_name);
		return it==GetSetInternal::factory_registration.end() ? 0x0 : it->second(section, progress_interface);
	}
	
} // namespace GetSetGui
