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

#ifndef __GetSetMouseKeyboardInteraction_h
#define __GetSetMouseKeyboardInteraction_h

#include "../GetSet/GetSet.hxx"


#include <QLabel>
#include <QTimer>

namespace GetSetGui {

	/// Emit events through GetSet keys. You should use  GetSetHandler on the dictionary Note that the actual current state may be incorrect, since not all events may pass through this object.
	class GetSetMouseKeyboardInteraction : public QLabel
	{
		Q_OBJECT

	public:
		GetSetMouseKeyboardInteraction(QWidget  *parent=0x0);

		// Access to current state of the window
		GetSetDictionary& getState();

		// Extra drawing only accessible when specialPaint is set
		QPainter* getPainter(); 

		bool timerActive();
		void timerStart(int milliseconds, bool repeat);
		void timerStop();

		// Events for call-back mechanism (	etMouseTracking(true); to receive move events )
		virtual void closeEvent(QCloseEvent *event);
		virtual void enterEvent(QEvent * event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void keyReleaseEvent(QKeyEvent *event);
		virtual void wheelEvent(QWheelEvent *event);
		virtual void resizeEvent(QResizeEvent* event);
		virtual void paintEvent(QPaintEvent *event);
	public slots:
		void timer_fires();

	protected:
		GetSetDictionary	dictionary;

		QTimer				m_timer;
		bool				m_firstMouse;
		QPainter			*m_painter;

	public:
		// Mouse
		GetSet<int>								mouseMoveX;
		GetSet<int>								mouseMoveY;
		GetSet<int>								mouseDragX;
		GetSet<int>								mouseDragY;
		GetSet<int>								mousePositionX;
		GetSet<int>								mousePositionY;
		GetSet<double>							mouseWheelRotateX;
		GetSet<double>							mouseWheelRotateY;
		GetSet<bool>							mouseButtonLeft;
		GetSet<bool>							mouseButtonMiddle;	
		GetSet<bool>							mouseButtonRight;

		// Keyboard
		GetSet<std::vector<unsigned char> >		keyboardKeysPressed; // Printable characters: A-Z 0-9 / * - + etc.
		GetSet<std::vector<int>	>				keyboardFKeysPressed; // F1-F35
		GetSet<bool>							keyboardModifiersShift;
		GetSet<bool>							keyboardModifiersCapsLock;
		GetSet<bool>							keyboardModifiersControl;
		GetSet<bool>							keyboardModifiersAlt;
		GetSet<bool>							keyboardSpecialPrint;
		GetSet<bool>							keyboardSpecialInsert;
		GetSet<bool>							keyboardSpecialDelete;
		GetSet<bool>							keyboardSpecialClear;
		GetSet<bool>							keyboardSpecialHome;
		GetSet<bool>							keyboardSpecialEnd;
		GetSet<bool>							keyboardSpecialSpace;
		GetSet<bool>							keyboardSpecialReturn;
		GetSet<bool>							keyboardSpecialEnter;
		GetSet<bool>							keyboardSpecialEscape;
		GetSet<bool>							keyboardSpecialTab;
		GetSet<bool>							keyboardSpecialBackspace;
		GetSet<bool>							keyboardSpecialPause;
		GetSet<bool>							keyboardSpecialSemicolon;
		GetSet<bool>							keyboardArrowLeft;
		GetSet<bool>							keyboardArrowUp;
		GetSet<bool>							keyboardArrowRight;
		GetSet<bool>							keyboardArrowDown;
		GetSet<bool>							keyboardArrowPageUp;
		GetSet<bool>							keyboardArrowPageDown;
		
		// Special
		GetSet<bool>							specialTimerFire;
		GetSet<bool>							specialActive;
		GetSet<bool>							specialPaint;
		GetSet<int>								specialWindowSizeX;
		GetSet<int>								specialWindowSizeY;

	private:
		void setMousePos(int x, int y);
		void setMouseButton(int button, bool state);
		bool setKey(int qkey, bool state);
		void setPrintableKey(unsigned char key, bool state);
	};

} // namespace GetSetGui

#endif // __GetSetMouseKeyboardInteraction_h
