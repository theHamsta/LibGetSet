//
//  Library: GetSet
//  c++ library for load/saving *typed* and *named* properties and automatic GUI.
//  
//  Copyright (c) by André Aichert (aaichert@gmail.com)
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

#ifndef __GetSetWidget_h
#define __GetSetWidget_h

#include "../GetSet/GetSet.hxx"

#include <string>

#include <QWidget>
#include <QScrollArea>

class QFormLayout;

namespace GetSetGui {

	/// A QWidget based representation of a GetSetInternal::Dictionary's section
	class GetSetWidget : public QScrollArea, protected GetSetInternal::Dictionary::Observer
	{
		Q_OBJECT

	protected:
		/// the dictionary the section resides in
		GetSetInternal::Dictionary& m_dictionary;

		/// string identifying the GetSet section represented by this widget
		std::string		m_section;

		/// A widget that can be scrolled
		QWidget*		m_content;

		/// The list of properties
		QFormLayout*	m_layout;

		/// Keep track of QObjects created in this window.
		std::map<std::string,QWidget*> m_owned;

		/// suppresses slots (since there was no way to prevent an emission)
		bool m_expectChange;

		/// (re-)build widged contents
		void init();

		/// clean up
		void destroy();


	private slots:
		void trigger();
		void selectFile();
		void selectFolder();
		void openSubSection();
		void collapseGroupedSection();
		void editingFinished();
		void sliderMoved(int value);
		void setValue(int value);
		void setValue(const QString&);
		void setRangeValue(int value);
		void setRangeValue(double value);

	public:
		GetSetWidget(const GetSetInternal::Section& section = GetSetGui::Section(), QWidget *parent=0x0);
		virtual ~GetSetWidget();

		/// Access to the GetSet Section represented by this widget.
		GetSetInternal::Section& getSection() {
			return GetSetGui::Section(m_section,Section(m_dictionary));
		}

		/// Close window on ESC key (if we have no parent)
		void keyPressEvent(QKeyEvent *e);

		// GetSetInternal::Dictionary::Observer
		void notify(const GetSetInternal::Node& node, GetSetInternal::Dictionary::Signal signal);

		void notifyCreate      (const GetSetInternal::Node& node);
		void notifyChange      (const GetSetInternal::Node& node);
		void notifyDestroy     (const GetSetInternal::Node& node);
		void notifyUpdateAttrib(const GetSetInternal::Node& node);
	};

} // namespace GetSetGui

#endif // __GetSetWidget_h
