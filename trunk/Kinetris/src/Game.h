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

#ifndef GAME_H
#define GAME_H

#include <QtGui/QtGui>

class KinectBlocks;
class SensorThread;
class LoaderThread;
class Background;
class HomeScreen;
class PlayScreen;
class MenuScreen;
class Player;
class InputManager;
class VisualMatrix;

class Game : public QGraphicsScene
{
	Q_OBJECT

public:

	enum State
	{
		STATE_NONE = 0,
		STATE_INIT,
		STATE_HOME,
		STATE_PLAY,
		STATE_MENU
	};

	virtual ~Game();

	static Game* instance(KinectBlocks* parent);
	static Game* instance();

	State getState() const;

	void update(qreal dt);

protected:
	
	static const qreal _UPDATE_INTERVAL; // ms

	qint64 _t0;
	int _timer;

	State _state;
	State _s1;

	SensorThread* _sensorThread;
	LoaderThread* _loaderThread;

	QGraphicsScene* _scene;

	Background* _background;
	HomeScreen* _homeScreen;
	PlayScreen* _playScreen;
	MenuScreen* _menuScreen;

	Player* _player;
	InputManager* _inputManager;
	VisualMatrix* _matrix;

	Game(KinectBlocks* parent);

	void init();
	void initSensor();
	void initLoader();
	void initStyle();
	void initState();
	void initTimer();

	void initSprite();
	void initEffect();
	void initPlayer();
	void initMatrix();

	void setState(State state);

	void onStateEnter(State state);
	void onStateLeave(State state);

	void timerEvent(QTimerEvent* event);

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

protected slots:

	void onConnectBegin();
	void onConnectError();
	void onConnect();
	void onDisconnect();

	void onUserEnter();
	void onUserLeave();

	void onSessionBegin();
	void onSessionEnd();

	void onFocusGain();
	void onFocusLose();
	void onFocusSwap();

	void onSteady();
	void onCircle(int direction);
	void onSlideX(qreal direction);
	void onSlideY(qreal direction);
	void onSlideZ(qreal direction);
	void onSwipeX(int direction, qreal speed, qreal angle);
	void onSwipeY(int direction, qreal speed, qreal angle);
	void onPush(qreal speed, qreal angle);
	void onWave();

	void onUsersMap(QImage image);

	void onLevel(int count);

private:

	static Game* _instance;
};

#endif // GAME_H
