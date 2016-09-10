#include "GetSetMouseKeyboardInteraction.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPainter>

namespace GetSetGui {

	GetSetMouseKeyboardInteraction::GetSetMouseKeyboardInteraction(QWidget *parent)
		: QLabel(parent)
		, mouseMoveX				("Mouse/Move X",dictionary)
		, mouseMoveY				("Mouse/Move Y",dictionary)
		, mouseDragX				("Mouse/Drag X",dictionary)
		, mouseDragY				("Mouse/Drag Y",dictionary)
		, mousePositionX			("Mouse/Position X",dictionary)
		, mousePositionY			("Mouse/Position Y",dictionary)
		, mouseWheelRotateX			("Mouse/Wheel X",dictionary)
		, mouseWheelRotateY			("Mouse/Wheel Y",dictionary)
		, mouseButtonLeft			("Mouse/Button Left",dictionary)
		, mouseButtonMiddle			("Mouse/Button Middle",dictionary)	
		, mouseButtonRight			("Mouse/Button Right",dictionary)
		, keyboardKeysPressed		("Keyboard/Keys Pressed",dictionary)
		, keyboardFKeysPressed		("Keyboard/F-Keys Pressed",dictionary)
		, keyboardModifiersShift	("Keyboard Modifiers/Shift",dictionary)
		, keyboardModifiersCapsLock	("Keyboard Modifiers/Caps Lock",dictionary)
		, keyboardModifiersControl	("Keyboard Modifiers/Control",dictionary)
		, keyboardModifiersAlt		("Keyboard Modifiers/Alt",dictionary)
		, keyboardSpecialPrint		("Keyboard Special/Print",dictionary)
		, keyboardSpecialInsert		("Keyboard Special/Insert",dictionary)
		, keyboardSpecialClear		("Keyboard Special/Clear",dictionary)
		, keyboardSpecialHome		("Keyboard Special/Home",dictionary)
		, keyboardSpecialEnd		("Keyboard Special/End",dictionary)
		, keyboardSpecialSpace		("Keyboard Special/Space",dictionary)
		, keyboardSpecialReturn		("Keyboard Special/Return",dictionary)
		, keyboardSpecialEnter		("Keyboard Special/Enter",dictionary)
		, keyboardSpecialEscape		("Keyboard Special/Escape",dictionary)
		, keyboardSpecialTab		("Keyboard Special/Tab",dictionary)
		, keyboardSpecialBackspace	("Keyboard Special/Backspace",dictionary)
		, keyboardSpecialPause		("Keyboard Special/Pause",dictionary)
		, keyboardArrowLeft			("Keyboard Arrow/Left",dictionary)
		, keyboardArrowUp			("Keyboard Arrow/Up",dictionary)
		, keyboardArrowRight		("Keyboard Arrow/Right",dictionary)
		, keyboardArrowDown			("Keyboard Arrow/Down",dictionary)
		, keyboardArrowPageUp		("Keyboard Arrow/Page Up",dictionary)
		, keyboardArrowPageDown		("Keyboard Arrow/Page Down",dictionary)
		, specialTimerFire			("Special/TimerFire",dictionary)
		, specialActive				("Special/Active",dictionary)
		, specialPaint				("Special/Paint",dictionary)
		, specialWindowSizeX		("Special/Window Size X",dictionary)
		, specialWindowSizeY		("Special/Window Size Y",dictionary)
	{
		connect( &m_timer, SIGNAL( timeout() ), this, SLOT( timer_fires() ) );
		specialActive=true;
		// setMouseTracking(true);
	}
	
	GetSetDictionary& GetSetMouseKeyboardInteraction::getState()
	{
		return dictionary;
	}

	void GetSetMouseKeyboardInteraction::closeEvent(QCloseEvent *event)
	{
		specialActive=false;
		QLabel::closeEvent(event);
	}

	void GetSetMouseKeyboardInteraction::timer_fires()
	{
		specialTimerFire=true;
	}

	void GetSetMouseKeyboardInteraction::startTimer(int milliseconds, bool repeat)
	{
		m_timer.setSingleShot(!repeat);
		m_timer.start(milliseconds);
	}

	void GetSetMouseKeyboardInteraction::stopTimer()
	{
		m_timer.stop();
		specialTimerFire=false;
	}

	void GetSetMouseKeyboardInteraction::enterEvent(QEvent * event)
	{
		m_firstMouse=true;
		QLabel::enterEvent(event);
	}


	void GetSetMouseKeyboardInteraction::mousePressEvent(QMouseEvent *event)
	{
		setMousePos(event->x(), event->y());
		setMouseButton(event->button(),true);
		QLabel::mousePressEvent(event);
	}

	void GetSetMouseKeyboardInteraction::mouseReleaseEvent(QMouseEvent *event)
	{
		setMousePos(event->x(), event->y());
		setMouseButton(event->button(),false);
		QLabel::mouseReleaseEvent(event);
	}

	void GetSetMouseKeyboardInteraction::mouseMoveEvent(QMouseEvent *event)
	{
		setMousePos(event->x(), event->y());
		QLabel::mouseMoveEvent(event);
	}

	void GetSetMouseKeyboardInteraction::keyPressEvent(QKeyEvent *event)
	{
		if (event->isAutoRepeat()) return;

		if (!setKey(event->key(),true))
		{
			unsigned char key=(unsigned char) event->text()[0].toLatin1();
			key=(unsigned char)tolower(key);
			setPrintableKey(key,true);
		}
		QLabel::keyPressEvent(event);
	}

	void GetSetMouseKeyboardInteraction::keyReleaseEvent(QKeyEvent *event)
	{
		if (event->isAutoRepeat()) return;

		if (!setKey(event->key(),false))
		{
			unsigned char key=(unsigned char) event->text()[0].toLatin1();
			key=(unsigned char)tolower(key);
			setPrintableKey(key,false);
		}
		QLabel::keyReleaseEvent(event);
	}

	void GetSetMouseKeyboardInteraction::wheelEvent(QWheelEvent *event)
	{
		mouseWheelRotateX=event->angleDelta().x()/8.0;
		mouseWheelRotateY=event->angleDelta().y()/8.0;
		QLabel::wheelEvent(event);
	}

	void GetSetMouseKeyboardInteraction::resizeEvent(QResizeEvent* event)
	{
		specialWindowSizeX=width();
		specialWindowSizeY=height();
		QLabel::resizeEvent(event);
	}

	QPainter* GetSetMouseKeyboardInteraction::getPainter()
	{
		return m_painter;
	}

	void GetSetMouseKeyboardInteraction::paintEvent(QPaintEvent *event)
	{
		QPainter painter;
		painter.begin(this);
		painter.setRenderHint(QPainter::Antialiasing);
		m_painter=&painter;
		specialPaint=true;
		m_painter=0x0;
		painter.end();
		QLabel::paintEvent(event);
	}

	void GetSetMouseKeyboardInteraction::setMousePos(int x, int y)
	{
		int delta[]={x-mousePositionX, y-mousePositionY};
		if (delta[0]==0 && delta[1]==0) return;
		mousePositionX=x;
		mousePositionY=y;
		if (m_firstMouse)
		{
			// do not send move / drag events the first time.
			m_firstMouse=false;
			return;
		}
		if (mouseButtonLeft || mouseButtonMiddle || mouseButtonRight)
		{
			mouseDragX=delta[0];
			mouseDragY=delta[1];
		}
		else
		{
			mouseMoveX=delta[0];
			mouseMoveY=delta[1];
		}
	}

	void GetSetMouseKeyboardInteraction::setMouseButton(int button, bool state)
	{
		if (button==1) mouseButtonLeft=state;
		else if (button==2) mouseButtonMiddle=state;
		else if (button==4) mouseButtonRight=state;
	}

	bool GetSetMouseKeyboardInteraction::setKey(int qkey, bool state)
	{
		if (false) ;
		else if (qkey==Qt::Key_Shift)		keyboardModifiersShift=state;
		else if (qkey==Qt::Key_CapsLock)	keyboardModifiersCapsLock=state;
		else if (qkey==Qt::Key_Control)		keyboardModifiersControl=state;
		else if (qkey==Qt::Key_Alt)			keyboardModifiersAlt=state;
		else if (qkey==Qt::Key_Print)		keyboardSpecialPrint=state;
		else if (qkey==Qt::Key_Insert)		keyboardSpecialInsert=state;
		else if (qkey==Qt::Key_Clear)		keyboardSpecialClear=state;
		else if (qkey==Qt::Key_Home)		keyboardSpecialHome=state;
		else if (qkey==Qt::Key_End)			keyboardSpecialEnd=state;
		else if (qkey==Qt::Key_Space)		keyboardSpecialSpace=state;
		else if (qkey==Qt::Key_Return)		keyboardSpecialReturn=state;
		else if (qkey==Qt::Key_Enter)		keyboardSpecialEnter=state;
		else if (qkey==Qt::Key_Escape)		keyboardSpecialEscape=state;
		else if (qkey==Qt::Key_Tab)			keyboardSpecialTab=state;
		else if (qkey==Qt::Key_Backspace)	keyboardSpecialBackspace=state;
		else if (qkey==Qt::Key_Pause)		keyboardSpecialPause=state;
		else if (qkey==Qt::Key_Left)		keyboardArrowLeft=state;
		else if (qkey==Qt::Key_Up)			keyboardArrowUp=state;
		else if (qkey==Qt::Key_Right)		keyboardArrowRight=state;
		else if (qkey==Qt::Key_Down)		keyboardArrowDown=state;
		else if (qkey==Qt::Key_PageUp)		keyboardArrowPageUp=state;
		else if (qkey==Qt::Key_PageDown)	keyboardArrowPageDown=state;
		else if (qkey>=Qt::Key_F1 && qkey<=Qt::Key_F35)
		{
			std::vector<int> fkeys=keyboardFKeysPressed;
			std::set<int> setofkeys(fkeys.begin(), fkeys.end());
			int fkey=1+qkey-Qt::Key_F1;
			if (state) setofkeys.insert(fkey);
			else setofkeys.erase(fkey);
			keyboardFKeysPressed=std::vector<int>(setofkeys.begin(),setofkeys.end());
		}
		else return false;
		return true;
	}

	void GetSetMouseKeyboardInteraction::setPrintableKey(unsigned char key, bool state)
	{
		std::vector<unsigned char> keys=keyboardKeysPressed;
		std::set<int> setofkeys(keys.begin(), keys.end());
		if (state) setofkeys.insert(key);
		else setofkeys.erase(key);
		keyboardKeysPressed=std::vector<unsigned char>(setofkeys.begin(),setofkeys.end());
	}

} // namespace GetSetGui
