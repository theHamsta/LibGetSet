#include "GetSetMouseKeyboardInteraction.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPainter>

namespace GetSetGui {

	GetSetMouseKeyboardInteraction::GetSetMouseKeyboardInteraction(const std::string& name, void (*user_interaction)(const std::string&, const std::string&), GetSetDictionary& dict, QWidget *parent)
		: QLabel(parent)
		, dictionary(dict)
		, callback(user_interaction,dictionary)
		, mouseMoveX				(name+"/Mouse/Move/X",dictionary)
		, mouseMoveY				(name+"/Mouse/Move/Y",dictionary)
		, mouseDragX				(name+"/Mouse/Drag/X",dictionary)
		, mouseDragY				(name+"/Mouse/Drag/Y",dictionary)
		, mousePositionX			(name+"/Mouse/Position X",dictionary)
		, mousePositionY			(name+"/Mouse/Position Y",dictionary)
		, mouseWheelRotateX			(name+"/Mouse/Wheel/Rotate X",dictionary)
		, mouseWheelRotateY			(name+"/Mouse/Wheel/Rotate Y",dictionary)
		, mouseButtonLeft			(name+"/Mouse/Button Left",dictionary)
		, mouseButtonMiddle			(name+"/Mouse/Button Middle",dictionary)	
		, mouseButtonRight			(name+"/Mouse/Button Right",dictionary)
		, keyboardKeysPressed		(name+"/Keyboard/Keys Pressed",dictionary)
		, keyboardFKeysPressed		(name+"/Keyboard/F-Keys Pressed",dictionary)
		, keyboardModifiersShift	(name+"/Keyboard/Modifiers/Shift",dictionary)
		, keyboardModifiersCapsLock	(name+"/Keyboard/Modifiers/Caps Lock",dictionary)
		, keyboardModifiersControl	(name+"/Keyboard/Modifiers/Control",dictionary)
		, keyboardModifiersAlt		(name+"/Keyboard/Modifiers/Alt",dictionary)
		, keyboardSpecialPrint		(name+"/Keyboard/Special/Print",dictionary)
		, keyboardSpecialInsert		(name+"/Keyboard/Special/Insert",dictionary)
		, keyboardSpecialClear		(name+"/Keyboard/Special/Clear",dictionary)
		, keyboardSpecialHome		(name+"/Keyboard/Special/Home",dictionary)
		, keyboardSpecialEnd		(name+"/Keyboard/Special/End",dictionary)
		, keyboardSpecialSpace		(name+"/Keyboard/Special/Space",dictionary)
		, keyboardSpecialReturn		(name+"/Keyboard/Special/Return",dictionary)
		, keyboardSpecialEnter		(name+"/Keyboard/Special/Enter",dictionary)
		, keyboardSpecialEscape		(name+"/Keyboard/Special/Escape",dictionary)
		, keyboardSpecialTab		(name+"/Keyboard/Special/Tab",dictionary)
		, keyboardSpecialBackspace	(name+"/Keyboard/Special/Backspace",dictionary)
		, keyboardSpecialPause		(name+"/Keyboard/Special/Pause",dictionary)
		, keyboardArrowLeft			(name+"/Keyboard/Arrow/Left",dictionary)
		, keyboardArrowUp			(name+"/Keyboard/Arrow/Up",dictionary)
		, keyboardArrowRight		(name+"/Keyboard/Arrow/Right",dictionary)
		, keyboardArrowDown			(name+"/Keyboard/Arrow/Down",dictionary)
		, keyboardArrowPageUp		(name+"/Keyboard/Arrow/Page Up",dictionary)
		, keyboardArrowPageDown		(name+"/Keyboard/Arrow/Page Down",dictionary)
		, specialTimerFire			(name+"/Special/TimerFire",dictionary)
		, specialActive				(name+"/Special/Active",dictionary)
		, specialPaint				(name+"/Special/Paint",dictionary)
		, specialWindowSizeX		(name+"/Special/Window Size X",dictionary)
		, specialWindowSizeY		(name+"/Special/Window Size Y",dictionary)
	{
		connect( &m_timer, SIGNAL( timeout() ), this, SLOT( timer_fires() ) );
		setWindowTitle(name.c_str());
		specialActive=true;
		setMouseTracking(true);
	}

	void GetSetMouseKeyboardInteraction::closeEvent(QCloseEvent *event)
	{
		specialActive=false;
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
	}


	void GetSetMouseKeyboardInteraction::mousePressEvent(QMouseEvent *event)
	{
		setMousePos(event->x(), event->y());
		setMouseButton(event->button(),true);
	}

	void GetSetMouseKeyboardInteraction::mouseReleaseEvent(QMouseEvent *event)
	{
		setMousePos(event->x(), event->y());
		setMouseButton(event->button(),false);
	}

	void GetSetMouseKeyboardInteraction::mouseMoveEvent(QMouseEvent *event)
	{
		setMousePos(event->x(), event->y());
	}

	void GetSetMouseKeyboardInteraction::keyPressEvent(QKeyEvent *event)
	{
		if (!setKey(event->key(),true))
		{
			unsigned char key=(unsigned char) event->text()[0].toLatin1();
			setPrintableKey(key,true);
		}
	}

	void GetSetMouseKeyboardInteraction::keyReleaseEvent(QKeyEvent *event)
	{
		if (!setKey(event->key(),false))
		{
			unsigned char key=(unsigned char) event->text()[0].toLatin1();
			setPrintableKey(key,false);
		}
	}

	void GetSetMouseKeyboardInteraction::wheelEvent(QWheelEvent *event)
	{
		mouseWheelRotateX=event->angleDelta().x()/8.0;
		mouseWheelRotateY=event->angleDelta().y()/8.0;
	}

	void GetSetMouseKeyboardInteraction::resizeEvent(QResizeEvent* event)
	{
		specialWindowSizeX=width();
		specialWindowSizeY=height();
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
