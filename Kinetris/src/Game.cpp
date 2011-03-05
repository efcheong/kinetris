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

#include "Game.h"

#include "KinectBlocks.h"
#include "SensorThread.h"
#include "LoaderThread.h"
#include "Background.h"
#include "HomeScreen.h"
#include "PlayScreen.h"
#include "MenuScreen.h"
#include "Player.h"
#include "InputManager.h"
#include "VisualMatrix.h"
#include "Tetromino.h"
#include "Pair.h"

const qreal Game::_UPDATE_INTERVAL = 1000.0f / 30.0f; // ms

Game* Game::_instance = NULL;

Game::Game(KinectBlocks* parent)
	: QGraphicsScene(parent)
{
	init();
}

Game::~Game()
{
}

Game* Game::instance(KinectBlocks* parent)
{
	if (!_instance)
		_instance = new Game(parent);

	return _instance;
}

Game* Game::instance()
{
	Q_ASSERT(_instance);

	return _instance;
}

void Game::init()
{
	_player = NULL;
	_inputManager = NULL;
	_matrix = NULL;

	initSensor();
	initLoader();
	initStyle();
	initState();
	initTimer();
}

void Game::initSensor()
{
	_sensorThread = SensorThread::instance(this);
	
	QObject::connect(_sensorThread, SIGNAL(evConnectBegin()), this, SLOT(onConnectBegin()));
	QObject::connect(_sensorThread, SIGNAL(evConnectError()), this, SLOT(onConnectError()));
	QObject::connect(_sensorThread, SIGNAL(evConnect()), this, SLOT(onConnect()));
	QObject::connect(_sensorThread, SIGNAL(evDisconnect()), this, SLOT(onDisconnect()));
	QObject::connect(_sensorThread, SIGNAL(finished()), _sensorThread, SLOT(start()));

	QObject::connect(_sensorThread, SIGNAL(evUserEnter()), this, SLOT(onUserEnter()));
	QObject::connect(_sensorThread, SIGNAL(evUserLeave()), this, SLOT(onUserLeave()));

	QObject::connect(_sensorThread, SIGNAL(evSessionBegin()), this, SLOT(onSessionBegin()));
	QObject::connect(_sensorThread, SIGNAL(evSessionEnd()), this, SLOT(onSessionEnd()));

	QObject::connect(_sensorThread, SIGNAL(evFocusGain()), this, SLOT(onFocusGain()));
	QObject::connect(_sensorThread, SIGNAL(evFocusLose()), this, SLOT(onFocusLose()));
	QObject::connect(_sensorThread, SIGNAL(evFocusSwap()), this, SLOT(onFocusSwap()));

	QObject::connect(_sensorThread, SIGNAL(evSteady()), this, SLOT(onSteady()));
	QObject::connect(_sensorThread, SIGNAL(evCircle(int)), this, SLOT(onCircle(int)));
	QObject::connect(_sensorThread, SIGNAL(evSlideX(qreal)), this, SLOT(onSlideX(qreal)));
	QObject::connect(_sensorThread, SIGNAL(evSlideY(qreal)), this, SLOT(onSlideY(qreal)));
	QObject::connect(_sensorThread, SIGNAL(evSlideZ(qreal)), this, SLOT(onSlideZ(qreal)));
	QObject::connect(_sensorThread, SIGNAL(evSwipeX(int, qreal, qreal)), this, SLOT(onSwipeX(int, qreal, qreal)));
	QObject::connect(_sensorThread, SIGNAL(evSwipeY(int, qreal, qreal)), this, SLOT(onSwipeY(int, qreal, qreal)));
	QObject::connect(_sensorThread, SIGNAL(evPush(qreal, qreal)), this, SLOT(onPush(qreal, qreal)));
	QObject::connect(_sensorThread, SIGNAL(evWave()), this, SLOT(onWave()));

	qRegisterMetaType<QImage>("QImage");
	QObject::connect(_sensorThread, SIGNAL(evUsersMap(QImage)), this, SLOT(onUsersMap(QImage)));
}

void Game::initLoader()
{
	_loaderThread = LoaderThread::instance(this);
}

void Game::initStyle()
{
	QFont::insertSubstitution("Arial Black", "Arial");

	QFont font("Arial Black");
	font.setPixelSize(16);
	font.setWeight(QFont::Black);
	font.setStretch(80);

	QApplication::setFont(font);
}

void Game::initState()
{
	_state = STATE_NONE;
	_s1 = STATE_NONE;
}

void Game::initTimer()
{
	_t0 = QDateTime::currentMSecsSinceEpoch();
	_timer = startTimer(_UPDATE_INTERVAL);

	qsrand(_t0);
}

void Game::initSprite()
{
	_background = new Background(this);
	addItem(_background->getSprite());

	_homeScreen = new HomeScreen(this);
	addItem(_homeScreen->getSprite());
	_homeScreen->hide();

	_playScreen = new PlayScreen(this);
	addItem(_playScreen->getSprite());
	_playScreen->hide();

	_menuScreen = new MenuScreen(this);
	addItem(_menuScreen->getSprite());
	_menuScreen->getSprite()->setZValue(8.0f);
	_menuScreen->hide();
}

void Game::initEffect()
{
}

void Game::initPlayer()
{
	_player = new Player(this);

	_inputManager = new InputManager(this);
	_player->setInputManager(_inputManager);
}

void Game::initMatrix()
{
	if (_matrix)
		_matrix->deleteLater();

	_matrix = new VisualMatrix(this);
	_player->setMatrix(_matrix);
	_playScreen->setMatrix(_matrix);

	QObject::connect(_matrix, SIGNAL(evLevel(int)), this, SLOT(onLevel(int)));
}

Game::State Game::getState() const
{
	return _state;
}

void Game::update(qreal dt)
{
	if (_state != _s1)
	{
		onStateLeave(_state);
		_state = _s1;
		onStateEnter(_state);
	}

	if (!_state)
	{
		setState(STATE_INIT);
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_background->update(dt);
		_homeScreen->update(dt);

		_inputManager->update(dt);
	}
	else if (_state == STATE_PLAY)
	{
		_background->update(dt);
		_playScreen->update(dt);

		// 1. Game state is created/updated
		// 2. Visual is updated
		// 3. In-between frames
		// 4. Player reacts to visual

		// Freeze input reacting to previous frame
		_inputManager->update(dt);

		// Player processing
		// Apply input or AI
		_player->update(dt);

		// Update game state
		_matrix->update(dt);
	}
	else if (_state == STATE_MENU)
	{
		_menuScreen->update(dt);

		_inputManager->update(dt);
	}
}

void Game::setState(State state)
{
	_s1 = state;
}

void Game::onStateEnter(State state)
{
	if (!state)
	{
	}
	else if (state == STATE_INIT)
	{
		initSprite();
		initEffect();

		initPlayer();

		_sensorThread->start();

		setState(STATE_HOME);
	}
	else if (state == STATE_HOME)
	{
		_menuScreen->hide();
		_playScreen->hide();
		_homeScreen->show();

		initMatrix();

		_background->setSpeed(_matrix->getLevel());
	}
	else if (state == STATE_PLAY)
	{
		_menuScreen->hide();
		_homeScreen->hide();
		_playScreen->show();
	}
	else if (state == STATE_MENU)
	{
		_menuScreen->show();
	}
}

void Game::onStateLeave(State state)
{
	if (!state)
	{
	}
	else if (state == STATE_INIT)
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
}

void Game::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == _timer)
	{
		qint64 t = QDateTime::currentMSecsSinceEpoch();
		int dt = qMin<qreal>(_UPDATE_INTERVAL, t - _t0);
		update(dt);
		_t0 = t;
	}
	else
	{
		QGraphicsScene::timerEvent(event);
	}
}

void Game::keyPressEvent(QKeyEvent* event)
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		if (event->key() == Qt::Key_Escape)
		{
			_inputManager->setState(InputManager::INPUT_PLAY, 1.0f, true);

			setState(Game::STATE_PLAY);
		}
	}
	else if (_state == STATE_PLAY)
	{
		if (event->key() == Qt::Key_Escape)
		{
			_inputManager->setState(InputManager::INPUT_PLAY, 1.0f, true);

			if (dynamic_cast<Matrix*>(_matrix)->getState() == Matrix::STATE_OVER)
				setState(STATE_HOME);
			else
				setState(STATE_MENU);
		}
	}
	else if (_state == STATE_MENU)
	{
		if (event->key() == Qt::Key_Escape)
		{
			_inputManager->setState(InputManager::INPUT_PLAY, 1.0f, true);

			setState(Game::STATE_PLAY);
		}
	}

	QGraphicsScene::keyPressEvent(event);
}

void Game::keyReleaseEvent(QKeyEvent* event)
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		//
	}
	else if (_state == STATE_PLAY)
	{
		//
	}
	else if (_state == STATE_MENU)
	{
		//
	}

	QGraphicsScene::keyReleaseEvent(event);
}

void Game::onConnectBegin()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_homeScreen->setStatus(tr("Connecting to Kinect..."));
	}
	else if (_state == STATE_PLAY)
	{
	}
	else if (_state == STATE_MENU)
	{
	}
}

void Game::onConnectError()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_homeScreen->setStatus(tr("Unable to connect to Kinect."));
	}
	else if (_state == STATE_PLAY)
	{
	}
	else if (_state == STATE_MENU)
	{
	}
}

void Game::onConnect()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_homeScreen->setStatus(tr("Detecting player... (Please step back from the Kinect.)"));
	}
	else if (_state == STATE_PLAY)
	{
	}
	else if (_state == STATE_MENU)
	{
		_menuScreen->setStatus(tr("Wave to continue."));
	}
}

void Game::onDisconnect()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_homeScreen->setStatus(tr("Lost connection to Kinect."));
	}
	else if (_state == STATE_PLAY)
	{
		_menuScreen->setStatus(tr("Lost connection to Kinect. Reconnecting..."));
		setState(STATE_MENU);
	}
	else if (_state == STATE_MENU)
	{
		_menuScreen->setStatus(tr("Lost connection to Kinect. Reconnecting..."));
	}
}

void Game::onUserEnter()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_homeScreen->setStatus(tr("Wave to start."));
	}
	else if (_state == STATE_PLAY)
	{
	}
	else if (_state == STATE_MENU)
	{
	}
}

void Game::onUserLeave()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
	}
	else if (_state == STATE_PLAY)
	{
	}
	else if (_state == STATE_MENU)
	{
	}
}

void Game::onSessionBegin()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		setState(Game::STATE_PLAY);
	}
	else if (_state == STATE_PLAY)
	{
	}
	else if (_state == STATE_MENU)
	{
		setState(Game::STATE_PLAY);
	}
}

void Game::onSessionEnd()
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_homeScreen->setStatus(tr("Wave to start."));
	}
	else if (_state == STATE_PLAY)
	{
		_menuScreen->setStatus(tr("Wave to continue."));
		setState(STATE_MENU);
	}
	else if (_state == STATE_MENU)
	{
	}
}

void Game::onFocusGain()
{
	qDebug() << "FocusGain";

	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
	}
	else if (_state == STATE_PLAY)
	{
		//
	}
	else if (_state == STATE_MENU)
	{
		//
	}
}

void Game::onFocusLose()
{
	qDebug() << "FocusLose";

	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
	}
	else if (_state == STATE_PLAY)
	{
		//
	}
	else if (_state == STATE_MENU)
	{
		//
	}
}

void Game::onFocusSwap()
{
}

void Game::onSteady()
{
	_inputManager->setState(InputManager::INPUT_NONE, 1.0f, true);
}

void Game::onCircle(int direction)
{
	if (!direction)
	{
		_inputManager->setState(InputManager::INPUT_L1, 0.0f, true);
		_inputManager->setState(InputManager::INPUT_R1, 0.0f, true);
	}
	else if (direction < 0)
	{
		_inputManager->setState(InputManager::INPUT_L1, 1.0f, true);
	}
	else if (direction > 0)
	{
		_inputManager->setState(InputManager::INPUT_R1, 1.0f, true);
	}
}

void Game::onSlideX(qreal direction)
{
	_inputManager->setState(InputManager::INPUT_X1, direction);
}

void Game::onSlideY(qreal direction)
{
	_inputManager->setState(InputManager::INPUT_Y1, direction);
}

void Game::onSlideZ(qreal direction)
{
	_inputManager->setState(InputManager::INPUT_Z1, -direction);
}

void Game::onSwipeX(int direction, qreal speed, qreal angle)
{
	_inputManager->setState(InputManager::INPUT_X2, direction, true);
}

void Game::onSwipeY(int direction, qreal speed, qreal angle)
{
	_inputManager->setState(InputManager::INPUT_Y2, direction, true);
}

void Game::onPush(qreal speed, qreal angle)
{
	_inputManager->setState(InputManager::INPUT_Z2, 1.0f, true);
}

void Game::onWave()
{
	_inputManager->setState(InputManager::INPUT_PLAY, 1.0f, true);

	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
	}
	else if (_state == STATE_PLAY)
	{
		if (dynamic_cast<Matrix*>(_matrix)->getState() == Matrix::STATE_OVER)
			setState(STATE_HOME);
//		else
//			setState(STATE_MENU);
	}
	else if (_state == STATE_MENU)
	{
		setState(STATE_PLAY);
	}
}

void Game::onUsersMap(QImage image)
{
	if (!_state)
	{
	}
	else if (_state == STATE_INIT)
	{
	}
	else if (_state == STATE_HOME)
	{
		_homeScreen->setAvatar(image);
	}
	else if (_state == STATE_PLAY)
	{
		_matrix->setAvatar(image);
	}
	else if (_state == STATE_MENU)
	{
	}
}

void Game::onLevel(int count)
{
	_background->setSpeed(_matrix->getRules()->getSpeed(_matrix->getLevel()));
}
