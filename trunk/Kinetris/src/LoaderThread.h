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

#ifndef LOADERTHREAD_H
#define LOADERTHREAD_H

#include <QtGui/QtGui>

class LoaderThread : public QThread
{
signals:

	//

public:

	enum State
	{
		STATE_NONE = 0,
		STATE_LOAD,
		STATE_QUIT
	};

	virtual ~LoaderThread();

	static LoaderThread* instance(QObject* parent);
	static LoaderThread* instance();

	State getState() const;

	QPixmap getCachedPixmap(const QString& uri) const;

	void update(qreal dt);

protected:

	static const qreal UPDATE_INTERVAL;

	mutable QMutex _mutex;

	State _state;
	State _s1;

	qint64 _t0;
	int _timer;

	LoaderThread(QObject* parent);

	void init();
	void initState();

	void initTimer();
	
	void setState(State state);

	void run();

	void onStateEnter(State state);
	void onStateLeave(State state);

private:

	static LoaderThread* _instance;
};

#endif // LOADERTHREAD_H
