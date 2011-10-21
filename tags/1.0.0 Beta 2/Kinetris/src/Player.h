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

#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QtGui>

#include "Pair.h"

class Game;
class InputManager;
class VisualMatrix;
class Tetromino;

class Player : public QObject
{
	Q_OBJECT

signals:

	void evPlay();
	void evQuit(bool force = false);

public:

	enum State
	{
		STATE_NONE = 0,
		STATE_WAIT,
		STATE_HOME,
		STATE_PLAY,
		STATE_MENU,
		STATE_QUIT
	};

	Player(Game* parent);
	virtual ~Player();

	State getState() const;
	void setState(State state);

	InputManager* getInputManager() const;
	void setInputManager(InputManager* inputManager);

	VisualMatrix* getMatrix() const;
	void setMatrix(VisualMatrix* matrix);

	void update(qreal dt);

protected:

	State _state;
	State _s1;

	InputManager* _inputManager;
	QVector<qreal> _inputTimer;

	VisualMatrix* _matrix;

	void init();
	void initState();

	void onStateEnter(State state);
	void onStateLeave(State state);
};

#endif // PLAYER_H
