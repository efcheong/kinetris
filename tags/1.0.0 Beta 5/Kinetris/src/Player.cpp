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

#include "Player.h"

#include "Game.h"
#include "InputManager.h"
#include "VisualMatrix.h"
#include "Tetromino.h"

Player::Player(Game* parent)
	: QObject(parent)
{
	init();
}

Player::~Player()
{
}

void Player::init()
{
	_inputManager = NULL;
	_inputTimer.fill(0.0f, InputManager::INPUT_);

	_matrix = NULL;

	initState();
}

void Player::initState()
{
	_state = STATE_NONE;
	_s1 = STATE_NONE;
}

Player::State Player::getState() const
{
	return _state;
}

void Player::setState(State state)
{
	_s1 = state;
}

InputManager* Player::getInputManager() const
{
	return _inputManager;
}

void Player::setInputManager(InputManager* inputManager)
{
	_inputManager = inputManager;
}

VisualMatrix* Player::getMatrix() const
{
	return _matrix;
}

void Player::setMatrix(VisualMatrix* matrix)
{
	_matrix = matrix;
}

void Player::update(qreal dt)
{
	if (_state != _s1)
	{
		onStateLeave(_state);
		_state = _s1;
		onStateEnter(_state);
	}

	qreal X1 = _inputManager->getState(InputManager::INPUT_X1);
	qreal Y1 = _inputManager->getState(InputManager::INPUT_Y1);
	qreal Z1 = _inputManager->getState(InputManager::INPUT_Z1);

	qreal X2 = _inputManager->getState(InputManager::INPUT_X2);
	qreal Y2 = _inputManager->getState(InputManager::INPUT_Y2);
//	qreal Z2 = _inputManager->getState(InputManager::INPUT_Z2);

	qreal L1 = _inputManager->getState(InputManager::INPUT_L1);
	qreal R1 = _inputManager->getState(InputManager::INPUT_R1);

	if (!_state)
	{
		setState(STATE_WAIT);
	}
	else if (_state == STATE_WAIT)
	{
	}
	else if (_state == STATE_HOME)
	{
	}
	else if (_state == STATE_PLAY)
	{
		if (Z1 >= 0.0f)
		{
			if (X1)
			{
				if (X1 <= -1.0f)
					_matrix->move(-1);
				else if (X1 >= 1.0f)
					_matrix->move(1);
			}

//			if (Y1)
//			{
//				if (Y1 <= -1.0f)
//					_matrix->setPush(true);
//				else if (Y1 > -1.0f)
//					_matrix->setPush(false);
//			}

//			if (X2)
//			{
//				if ((X2 < 0.0f)
//					&& (X1 < 0.0f))
//				{
//					_matrix->move(-1);
//				}
//				else if ((X2 > 0.0f)
//					&& (X1 > 0.0f))
//				{
//					_matrix->move(1);
//				}
//			}

			if (Y2)
			{
				if ((Y2 < 0.0f)
					&& (Y1 < 0.0f))
				{
					_matrix->drop();
				}
				else if ((Y2 > 0.0f)
					&& (Y1 > 0.0f))
				{
					_matrix->hold();
				}
			}

			if (Y1 >= 1.0f)
			{
				_inputTimer[InputManager::INPUT_Y1] += dt;
				if (_inputTimer[InputManager::INPUT_Y1] >= 3000.0f)
				{
					_inputTimer[InputManager::INPUT_Y1] = 0.0f;

					emit evQuit();
				}
			}
			else
			{
				if (_inputTimer[InputManager::INPUT_Y1] > 0.0f)
					_inputTimer[InputManager::INPUT_Y1] = 0.0f;
			}
		}
		else if (Z1 < 0.0f)
		{
			if (L1)
			{
				_matrix->turn(-L1);
			}

			if (R1)
			{
				_matrix->turn(R1);
			}
		}
	}
	else if (_state == STATE_MENU)
	{
	}
	else if (_state == STATE_QUIT)
	{
		if (X2)
		{
			if ((X2 < 0.0f)
				&& (X1 < 0.0f))
			{
				emit evQuit(true);
			}
			else if ((X2 > 0.0f)
				&& (X1 > 0.0f))
			{
				emit evPlay();
			}
		}
	}
}

void Player::onStateEnter(State state)
{
	if (!state)
	{
	}
	else if (state == STATE_WAIT)
	{
	}
	else if (state == STATE_HOME)
	{
	}
	else if (state == STATE_PLAY)
	{
	}
	else if (state == STATE_MENU)
	{
	}
	else if (state == STATE_QUIT)
	{
	}
}

void Player::onStateLeave(State state)
{
	// Prevent "unreferenced formal parameter" warning
	state;
}
