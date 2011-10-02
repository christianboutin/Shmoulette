#include "stdafx.h"
#include "main.h"
#include "cegui\ceguiinputevent.h"
#include "cegui\ceguisystem.h"

namespace Shmoulette{
	
	InputBridge::InputBridge(){
	}

	KeyboardOnlyInputBridge::KeyboardOnlyInputBridge(){
		mVirtualMouse = Ogre::Vector2(0,0);
	}

	void KeyboardOnlyInputBridge::activate(){
		Core::getSingleton()->getInputManager()->addKeyListener(this);
	}
	void KeyboardOnlyInputBridge::deactivate(){
		Core::getSingleton()->getInputManager()->removeKeyListener(this);
	}
	#define MAX_AXIS 3

	bool KeyboardOnlyInputBridge::keyPressed( const OIS::KeyEvent &arg ){
		if (arg.key == OIS::KC_A){
			mVirtualMouse.x += MAX_AXIS;
			if (mVirtualMouse.x > MAX_AXIS) mVirtualMouse.x = MAX_AXIS;
		}
		if (arg.key == OIS::KC_D){
			mVirtualMouse.x += -MAX_AXIS;
			if (mVirtualMouse.x < -MAX_AXIS) mVirtualMouse.x = -MAX_AXIS;
		}
		if (arg.key == OIS::KC_W){
			mVirtualMouse.y += -MAX_AXIS;
			if (mVirtualMouse.y < -MAX_AXIS) mVirtualMouse.y = -MAX_AXIS;
		}
		if (arg.key == OIS::KC_S){
			mVirtualMouse.y += MAX_AXIS;
			if (mVirtualMouse.y > MAX_AXIS) mVirtualMouse.y = MAX_AXIS;
		}
		Level::getSingleton()->getPlayerShip()->setDeltaMovement(mVirtualMouse);
		if (arg.key == OIS::KC_J){
			Level::getSingleton()->getPlayerShip()->activatePrimary();
		}
		if (arg.key == OIS::KC_K){
			Level::getSingleton()->getPlayerShip()->activateSecondary();
		}
		if (arg.key == OIS::KC_L){
			Level::getSingleton()->getPlayerShip()->activateSuper();
		}
		return false;
	}
	bool KeyboardOnlyInputBridge::keyReleased( const OIS::KeyEvent &arg ){
		if (arg.key == OIS::KC_A){
			mVirtualMouse.x -= MAX_AXIS;
			if (mVirtualMouse.x < -MAX_AXIS) mVirtualMouse.x = -MAX_AXIS;
		}
		if (arg.key == OIS::KC_D){
			mVirtualMouse.x -= -MAX_AXIS;
			if (mVirtualMouse.x > MAX_AXIS) mVirtualMouse.x = MAX_AXIS;
		}
		if (arg.key == OIS::KC_W){
			mVirtualMouse.y -= -MAX_AXIS;
			if (mVirtualMouse.y > MAX_AXIS) mVirtualMouse.y = MAX_AXIS;
		}
		if (arg.key == OIS::KC_S){
			mVirtualMouse.y -= MAX_AXIS;
			if (mVirtualMouse.y < -MAX_AXIS) mVirtualMouse.y = -MAX_AXIS;
		}
		Level::getSingleton()->getPlayerShip()->setDeltaMovement(mVirtualMouse);
		if (arg.key == OIS::KC_J){
			Level::getSingleton()->getPlayerShip()->deactivatePrimary();
		}
		if (arg.key == OIS::KC_K){
			Level::getSingleton()->getPlayerShip()->deactivateSecondary();
		}
		return false;
	}

	MouseAndKeyboardInputBridge::MouseAndKeyboardInputBridge(){
		mMovementThisFrame = false;
	}

	void MouseAndKeyboardInputBridge::activate(){
		Shmoulette::InputManager* mng = Core::getSingleton()->getInputManager();

		mng->addKeyListener(this);
		mng->addMouseListener(this);
	}
	void MouseAndKeyboardInputBridge::deactivate(){
		Shmoulette::InputManager* mng = Core::getSingleton()->getInputManager();

		mng->removeKeyListener(this);
		mng->removeMouseListener(this);
	}
		
	bool MouseAndKeyboardInputBridge::keyPressed( const OIS::KeyEvent &arg ){
		return false;
	}
	bool MouseAndKeyboardInputBridge::keyReleased( const OIS::KeyEvent &arg ){
		return false;
	}
	bool MouseAndKeyboardInputBridge::mouseMoved( const OIS::MouseEvent &arg ){
		mMovementThisFrame = true;
		float xPos = (float)arg.state.X.rel;
		float yPos = (float)arg.state.Y.rel;
		//float dx = xPos/(float)arg.state.width;
		//float dy = yPos/(float)arg.state.height;

		float mx = 0-(xPos)/5;//0-dx*250;
		float my = (yPos)/5;//dy*250;

		/*if (mx < 0-MAX_AXIS) mx = 0-MAX_AXIS;
		if (my < 0-MAX_AXIS) my = 0-MAX_AXIS;
		if (mx > MAX_AXIS) mx = MAX_AXIS;
		if (my > MAX_AXIS) my = MAX_AXIS;*/

		Level::getSingleton()->getPlayerShip()->setDeltaMovement(Ogre::Vector2(mx, my));
		return false;
	}
	bool MouseAndKeyboardInputBridge::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){
		if (id == OIS::MB_Left){
			Level::getSingleton()->getPlayerShip()->activatePrimary();
		}
		if (id == OIS::MB_Right){
			Level::getSingleton()->getPlayerShip()->activateSecondary();
		}
		if (id == OIS::MB_Middle){
			Level::getSingleton()->getPlayerShip()->activateSuper();
		}
		return false;
	}
	bool MouseAndKeyboardInputBridge::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){
		if (id == OIS::MB_Left){
			Level::getSingleton()->getPlayerShip()->deactivatePrimary();
		}
		if (id == OIS::MB_Right){
			Level::getSingleton()->getPlayerShip()->deactivateSecondary();
		}
		if (id == OIS::MB_Middle){
			Level::getSingleton()->getPlayerShip()->deactivateSuper();
		}
		return false;
	}
	void MouseAndKeyboardInputBridge::update(){
		if (!mMovementThisFrame == false){
			/*Ogre::Vector2 delta = Level::getSingleton()->getPlayerShip()->getDeltaMovement();
			Level::getSingleton()->getPlayerShip()->setDeltaMovement(
				delta-(delta*Level::getSingleton()->getTimeDelta())
			);*/
			Level::getSingleton()->getPlayerShip()->setDeltaMovement(Ogre::Vector2(0,0));
		}else{
			mMovementThisFrame = false;
		}
	}


	CEGUI::MouseButton convertOISMouseButtonToCegui(int buttonID)
	{
		switch (buttonID)
		{
		case 0: return CEGUI::LeftButton;
		case 1: return CEGUI::RightButton;
		case 2:	return CEGUI::MiddleButton;
		case 3: return CEGUI::X1Button;
		default: return CEGUI::LeftButton;
		}
	}


	InputManager::InputManager(Ogre::RenderWindow* win){
		OIS::ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		mOIS.mInputManager = OIS::InputManager::createInputSystem( pl );

		try{
			mOIS.mKeyboard = static_cast<OIS::Keyboard*>(mOIS.mInputManager->createInputObject( OIS::OISKeyboard, true ));
		}
		catch(...) {
			throw("No keyboard error");
		}

		try{
			mOIS.mMouse = static_cast<OIS::Mouse*>(mOIS.mInputManager->createInputObject( OIS::OISMouse, true ));
		}catch(...) {
			throw("No mouse error");
		}

		

		unsigned int width, height, depth;
		int top, left;
		win->getMetrics(width, height, depth, left, top);
		const OIS::MouseState &ms = mOIS.mMouse->getMouseState();
		ms.width = width;
		ms.height = height;

		mOIS.mKeyboard->setEventCallback(this);
		mOIS.mMouse->setEventCallback(this);
		mCEGUIForward = false;
	}
	bool InputManager::keyPressed( const OIS::KeyEvent &arg ){
		for (std::vector<OIS::KeyListener*>::iterator it = mKeyListeners.begin();it!=mKeyListeners.end();it++){
			(*it)->keyPressed(arg);
		}
		mKey[arg.key].update(true);
		if (mCEGUIForward){
			CEGUI::System::getSingleton().injectKeyDown( arg.key );
			if (arg.key == OIS::KC_NUMPAD0){
				CEGUI::System::getSingleton().injectChar( 48 );
			}else if (arg.key == OIS::KC_NUMPAD1){
				CEGUI::System::getSingleton().injectChar( 49 );
			}else if (arg.key == OIS::KC_NUMPAD2){
				CEGUI::System::getSingleton().injectChar( 50 );
			}else if (arg.key == OIS::KC_NUMPAD3){
				CEGUI::System::getSingleton().injectChar( 51 );
			}else if (arg.key == OIS::KC_NUMPAD4){
				CEGUI::System::getSingleton().injectChar( 52 );
			}else if (arg.key == OIS::KC_NUMPAD5){
				CEGUI::System::getSingleton().injectChar( 53 );
			}else if (arg.key == OIS::KC_NUMPAD6){
				CEGUI::System::getSingleton().injectChar( 54 );
			}else if (arg.key == OIS::KC_NUMPAD7){
				CEGUI::System::getSingleton().injectChar( 55 );
			}else if (arg.key == OIS::KC_NUMPAD8){
				CEGUI::System::getSingleton().injectChar( 56 );
			}else if (arg.key == OIS::KC_NUMPAD9){
				CEGUI::System::getSingleton().injectChar( 57 );
			}else CEGUI::System::getSingleton().injectChar( arg.text );
		}
		if ((mKey[OIS::KC_LMENU].isDown()||mKey[OIS::KC_RMENU].isDown()) && mKey[OIS::KC_F4].isDown()){
			int h=0;
		}
		return true;
	}
	bool InputManager::keyReleased( const OIS::KeyEvent &arg ){
		for (std::vector<OIS::KeyListener*>::iterator it = mKeyListeners.begin();it!=mKeyListeners.end();it++){
			(*it)->keyReleased(arg);
		}
		mKey[arg.key].update(false);
		if (mCEGUIForward) CEGUI::System::getSingleton().injectKeyUp( arg.key );
		return true;
	}
	bool InputManager::mouseMoved( const OIS::MouseEvent &arg ){
		for (std::vector<OIS::MouseListener*>::iterator it = mMouseListeners.begin();it!=mMouseListeners.end();it++){
			(*it)->mouseMoved(arg);
		}
		float xPos = (float)arg.state.X.rel;
		float yPos = (float)arg.state.Y.rel;
		float zPos = arg.state.Z.rel;

		mMouse.setDX(xPos/(float)arg.state.width);
		mMouse.setDY(yPos/(float)arg.state.height);
		mMouse.setDZ(zPos);
		if (mCEGUIForward){
			CEGUI::System::getSingleton().injectMouseMove( arg.state.X.rel, arg.state.Y.rel );
			CEGUI::System::getSingleton().injectMouseWheelChange(zPos/40);
		}
		return true;
	}

	
	bool InputManager::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){
		for (std::vector<OIS::MouseListener*>::iterator it = mMouseListeners.begin();it!=mMouseListeners.end();it++){
			(*it)->mousePressed(arg, id);
		}
		if (mCEGUIForward) CEGUI::System::getSingleton().injectMouseButtonDown(convertOISMouseButtonToCegui(id));
		mMouse.setButtonState(id, true);
		return true;
	}
	bool InputManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ){
		for (std::vector<OIS::MouseListener*>::iterator it = mMouseListeners.begin();it!=mMouseListeners.end();it++){
			(*it)->mouseReleased(arg, id);
		}
		mMouse.setButtonState(id, false);
		if (mCEGUIForward) CEGUI::System::getSingleton().injectMouseButtonUp(convertOISMouseButtonToCegui(id));
		return true;
	}

	void InputManager::update(){
		mMouse.setDX(0);
		mMouse.setDY(0);
		mMouse.setDZ(0);

		for (int i=0;i<0xED+1;i++){
			mKey[i].setUpdateFlag(false);
		}

		mOIS.mKeyboard->capture();
		mOIS.mMouse->capture();

		for (int i=0;i<0xED+1;i++){
			if (!mKey[i].getUpdateFlag()){
				mKey[i].update(mKey[i].isDown());
			}
		}
	}
	void InputManager::addKeyListener(OIS::KeyListener* kl){
		mKeyListeners.push_back(kl);

	}
	void InputManager::addMouseListener(OIS::MouseListener* ml){
		mMouseListeners.push_back(ml);
	}
	void InputManager::removeKeyListener(OIS::KeyListener* kl){
		for (std::vector<OIS::KeyListener*>::iterator it = mKeyListeners.begin();it!=mKeyListeners.end();it++){
			if ((*it) == kl){
				mKeyListeners.erase(it);
				break;
			}
		}
	}
	void InputManager::removeMouseListener(OIS::MouseListener* ml){
		for (std::vector<OIS::MouseListener*>::iterator it = mMouseListeners.begin();it!=mMouseListeners.end();it++){
			if ((*it) == ml){
				mMouseListeners.erase(it);
				break;
			}
		}
	}

}

/*
bool mouseMoved( const OIS::MouseEvent &arg )
	{
		return true;
	}

	//----------------------------------------------------------------//
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		return true;
	}

	//----------------------------------------------------------------//
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		return true;
	}

	//----------------------------------------------------------------//
	bool keyPressed( const OIS::KeyEvent &arg )
	{
		if( arg.key == OIS::KC_ESCAPE )
			mShutdownRequested = true;
		return true;
	}

	//----------------------------------------------------------------//
	bool keyReleased( const OIS::KeyEvent &arg )
	{
		return true;
	}
	*/