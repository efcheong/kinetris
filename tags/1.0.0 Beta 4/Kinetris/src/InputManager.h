/**
 * This file is part of Kinetris.
 * 
 * Kinetris ("this program") is Copyright (C) 2011 Conan Chen.
 * Contact: Conan Chen <http://conanchen.com/>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KINETRIS_INPUTMANAGER_H
#define KINETRIS_INPUTMANAGER_H

#include <QtCore/QtCore>

class Game;

class InputManager : public QObject
{
	Q_OBJECT

public:

	enum Input
	{
		INPUT_NONE = 0,
		INPUT_X1,
		INPUT_Y1,
		INPUT_Z1,
		INPUT_X2,
		INPUT_Y2,
		INPUT_Z2,
		INPUT_L1,
		INPUT_R1,
		INPUT_PLAY,
		INPUT_QUIT,
		INPUT_
	};

//	enum Controller
//	{
//		CONTROLLER_NONE = 0,
//		CONTROLLER_KEYBOARD,
//		CONTROLLER_MOUSE,
//		CONTROLLER_JOYSTICK,
//		CONTROLLER_GAMEPAD,
//		CONTROLLER_TOUCH,
//		CONTROLLER_GESTURE,
//		CONTROLLER_VOICE,
//		CONTROLLER_
//	};

	InputManager(Game* parent);
	virtual ~InputManager();

	qreal getState(Input which) const;
	void setState(Input which, qreal value, bool clear = false);

	qreal getStatePrevious(Input which) const;
	bool getApplied(Input which) const;
	bool getCleared(Input which) const;

	void update(qreal dt);

protected:

	QVector<qreal> _state;
	QVector<qreal> _s0;
	QVector<qreal> _s1;

	QStack<Input> _clear;

	void init();
};

#endif // KINETRIS_INPUTMANAGER_H
