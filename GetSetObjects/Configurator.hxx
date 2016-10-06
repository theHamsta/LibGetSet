//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by Andr� Aichert (aaichert@gmail.com) and Thomas Koehler 
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

#ifndef __Configurator_h
#define __Configurator_h

#include "../GetSet/GetSet.hxx"

namespace GetSetObjects {
	/// Configurator for the object factory
	class Configurator {
	protected:
		GetSetDictionary&	dict;		//< Dictionary where configuration will be stored
		std::string			section;	//< Prefix for this instance, non-empty and ending in '/'

	public:
		Configurator(const std::string& _section="", GetSetDictionary& _dict=GetSetDictionary::global())
			: dict(_dict)
			, section(_section)
		{
			if (!section.empty() && section.back()!='/')
				section.push_back('/');
		}

		template <typename T=std::string>
		void declare(const std::string& param, const T& default_value) {
			std::string path=section+param;
			if (dict.exists(path))
				GetSet<T>(path,dict);
			else
				GetSet<T>(path,dict)=value;
		}

		template <typename T=std::string>
		GetSet<T> declare(const std::string& param, const std::string& default_value) {
			std::string path=section+param;
			bool set_default=dict.exists(path);
			GetSet<T> value(path,dict);
			if (set_default) value=default_value;
			return value;
		}

		GetSetGui::Button declareFunction(const std::string& param, void *user_data, void (*callback)(const std::string&, void* ) )
		{
			std::string path=section+param;
			GetSetGui::Button button(path,dict);
			button.setString(param);
			button.setCallback(callback, param, user_data);
			return button;
		}

		template <typename T>
		GetSet<T> param(const std::string& param_name)
		{
			std::string path=section+param;
			return GetSet<T>(path,dict);
		}

		Configurator operator()(const std::string& subsection)
		{
			std::string path=section+subsection;
			return Configurator(path,dict);
		}

	};
	
	template <class FactoryClass>
	FactoryClass* CreateAndConfigure(const std::string& instanceName, Configurator& config=Configurator())
	{
		return Factory(config).Create<FactoryClass>(instanceName);
	}

} // namespace GetSetObjects

#endif // __Configurator_h
