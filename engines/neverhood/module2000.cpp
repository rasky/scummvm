/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "neverhood/module2000.h"
#include "neverhood/gamemodule.h"
#include "neverhood/navigationscene.h"

namespace Neverhood {

Module2000::Module2000(NeverhoodEngine *vm, Module *parentModule, int which)
	: Module(vm, parentModule) {
	
	debug("Create Module2000(%d)", which);

	if (which < 0) {
		switch (_vm->gameState().sceneNum) {
		case 0:
			createScene2001(-1);
			break;
		case 2:
			createScene2003(-1);
			break;
		default:
			createScene2002(-1);
			break;
		}
	} else if (which == 0) {
		createScene2001(3);
	} else if (which == 1) {
		createScene2001(1);
	}

}

Module2000::~Module2000() {
	// TODO Sound1ChList_sub_407A50(0x81293110);
}

void Module2000::createScene2001(int which) {
	_vm->gameState().sceneNum = 0;
	_childObject = new Scene2001(_vm, this, which);
	SetUpdateHandler(&Module2000::updateScene2001);
	_childObject->handleUpdate();
}

void Module2000::createScene2002(int which) {
	_vm->gameState().sceneNum = 1;
	if (getGlobalVar(0x98109F12)) {
		createNavigationScene(0x004B7B48, which);
	} else {
		createNavigationScene(0x004B7B00, which);
	}
	SetUpdateHandler(&Module2000::updateScene2002);
	_childObject->handleUpdate();
}

void Module2000::createScene2003(int which) {
	_vm->gameState().sceneNum = 2;
	setGlobalVar(0x98109F12, 1);
	setSubVar(0x2C145A98, 1, 1);
	createSmackerScene(0x204B2031, true, true, false);
	SetUpdateHandler(&Module2000::updateScene2003);
	_childObject->handleUpdate();
}

void Module2000::updateScene2001() {
	_childObject->handleUpdate();
	if (_done) {
		_done = false;
		delete _childObject;
		_childObject = NULL;
		if (_field20 == 1) {
			_parentModule->sendMessage(0x1009, 0, this);
		} else {
			createScene2002(0);
		}
	}
}

void Module2000::updateScene2002() {
	_childObject->handleUpdate();
	if (_done) {
		_done = false;
		delete _childObject;
		_childObject = NULL;
		if (_field20 == 0) {
			if (getGlobalVar(0x98109F12)) {
				createScene2002(0);
			} else {
				createScene2003(-1);
			}
		} else if (_field20 == 1) {
			createScene2002(1);
		} else if (_field20 == 2) {
			createScene2001(0);
		}
	}
}

void Module2000::updateScene2003() {
	_childObject->handleUpdate();
	if (_done) {
		_done = false;
		delete _childObject;
		_childObject = NULL;
		createScene2002(0);
	}
}

// Scene2001

Scene2001::Scene2001(NeverhoodEngine *vm, Module *parentModule, int which)
	: Scene(vm, parentModule, true) {

	_surfaceFlag = true;
	SetMessageHandler(&Scene2001::handleMessage);

	_background = addBackground(new DirtyBackground(_vm, 0xA6417244, 0, 0));
	_palette = new Palette(_vm, 0xA6417244);
	_palette->usePalette();
	_mouseCursor = addSprite(new Mouse433(_vm, 0x17240A6C, NULL));

	_class401 = addSprite(new StaticSprite(_vm, 0x0D641724, 1100));

	if (which < 0) {
		_klayman = new KmScene2001(_vm, this, 300, 345);
		setMessageList(0x004B3538);
		sendMessage(0x2000, 0, this);
	} else if (which == 1) {
		_klayman = new KmScene2001(_vm, this, 116, 345);
		_klayman->sendMessage(0x2000, 1, this);
		setMessageList(0x004B3540);
		sendMessage(0x2000, 1, this);
	} else if (which == 2) {
		_klayman = new KmScene2001(_vm, this, 116, 345);
		_klayman->sendMessage(0x2000, 1, this);
		setMessageList(0x004B35F0);
		sendMessage(0x2000, 1, this);
	} else if (which == 3) {
		_klayman = new KmScene2001(_vm, this, 116, 345);
		_klayman->sendMessage(0x2000, 1, this);
		setMessageList(0x004B3550);
		sendMessage(0x2000, 1, this);
	} else {
		_klayman = new KmScene2001(_vm, this, 390, 345);
		setMessageList(0x004B3530);
		sendMessage(0x2000, 0, this);
		_klayman->setDoDeltaX(1);
	}
	addSprite(_klayman);
	
	_klayman->getSurface()->getClipRect().x1 = _class401->getSurface()->getDrawRect().x;
	_klayman->getSurface()->getClipRect().y1 = 0;
	_klayman->getSurface()->getClipRect().x2 = 640;
	_klayman->getSurface()->getClipRect().y2 = 480;
	
}

uint32 Scene2001::handleMessage(int messageNum, const MessageParam &param, Entity *sender) {
	Scene::handleMessage(messageNum, param, sender);
	switch (messageNum) {
	case 0x2000:
		if (param.asInteger()) {
			setRectList(0x004B3680);
			_klayman->setKlaymanTable3();
		} else {
			setRectList(0x004B3670);
			_klayman->setKlaymanTable1();
		}
	}	
	return 0;
}

} // End of namespace Neverhood
