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

#include "InputManager.h"

#include "Game.h"

InputManager::InputManager(Game* parent)
	: QObject(parent)
{
	init();
}

InputManager::~InputManager()
{
}

void InputManager::init()
{
	_state.fill(0.0f, INPUT_);
	_s0.fill(0.0f, INPUT_);
	_s1.fill(0.0f, INPUT_);
}

qreal InputManager::getState(Input which) const
{
	return _state[which];
}

void InputManager::setState(Input which, qreal value, bool clear)
{
	_s1[which] = value;

	if ((clear)
		&& (!_clear.contains(which)))
	{
		_clear.push(which);
	}
}

qreal InputManager::getStatePrevious(Input which) const
{
	return _s0[which];
}

bool InputManager::getApplied(Input which) const
{
	return ((!_s0[which]) && (_state[which]));
}

bool InputManager::getCleared(Input which) const
{
	return ((_s0[which]) && (!_state[which]));
}

void InputManager::update(qreal dt)
{
	// Prevent "unreferenced formal parameter" warning
	dt;

	_s0 = _state;
	_state = _s1;
	
	while (_clear.count())
	{
		_s1[_clear.pop()] = 0.0f;
	}
}
