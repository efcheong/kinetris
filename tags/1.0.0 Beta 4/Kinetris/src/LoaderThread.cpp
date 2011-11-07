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

#include "LoaderThread.h"

LoaderThread* LoaderThread::_instance = NULL;

LoaderThread::LoaderThread(QObject* parent)
	: QThread(parent)
{
	init();
}

LoaderThread::~LoaderThread()
{
	setState(STATE_QUIT);

	// Wait for run method to return
	wait();
}

LoaderThread* LoaderThread::instance(QObject* parent)
{
	if (!_instance)
		_instance = new LoaderThread(parent);

	return _instance;
}

LoaderThread* LoaderThread::instance()
{
	Q_ASSERT(_instance);

	return _instance;
}

void LoaderThread::init()
{
	initState();
}

void LoaderThread::initState()
{
	_state = STATE_NONE;
	_s1 = STATE_NONE;
}

LoaderThread::State LoaderThread::getState() const
{
	QMutexLocker l(&_stateMutex);

	return _state;
}

void LoaderThread::setState(State state)
{
	QMutexLocker l(&_stateMutex);

	_s1 = state;
}

QPixmap LoaderThread::getCachedPixmap(const QString& uri) const
{
	QMutexLocker l(&_cacheMutex);

	QPixmap pixmap;
	if (!QPixmapCache::find(uri, pixmap))
	{
		pixmap.load(uri);
		QPixmapCache::insert(uri, pixmap);
	}

	return pixmap;
}

void LoaderThread::update()
{
	State state = STATE_NONE;

	{
		QMutexLocker l(&_stateMutex);

		if (_s1 != _state)
		{
			onStateLeave(_state);
			_state = _s1;
			onStateEnter(_state);
		}

		state = _state;
	}

	if (!state)
	{
		//
	}
	else if (state == STATE_LOAD)
	{
		//
	}
	else if (state == STATE_QUIT)
	{
	}
}

void LoaderThread::run()
{
	initState();

	while (getState() != STATE_QUIT)
	{
		msleep(0);

		update();
	}
}

void LoaderThread::onStateEnter(State state)
{
}

void LoaderThread::onStateLeave(State state)
{
}
