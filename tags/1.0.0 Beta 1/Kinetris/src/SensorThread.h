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

#ifndef SENSORTHREAD_H
#define SENSORTHREAD_H

#include <QtGui/QtGui>

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <XnVNite.h>

class SensorThread : public QThread
{
	Q_OBJECT

signals:

	void evConnectBegin();
	void evConnectError();
	void evConnect();
	void evDisconnect();

	void evUserEnter();
	void evUserLeave();

	void evSessionBegin();
	void evSessionEnd();

	void evFocusGain();
	void evFocusLose();
	void evFocusSwap();
	void evFocusMove();

	void evSteady();
	void evCircle(int direction);
	void evSlideX(qreal direction);
	void evSlideY(qreal direction);
	void evSlideZ(qreal direction);
	void evSwipeX(int direction, qreal speed, qreal angle);
	void evSwipeY(int direction, qreal speed, qreal angle);
	void evPush(qreal speed, qreal angle);
	void evWave();

	void evUsersMap(QImage image);

public:

	enum State
	{
		STATE_NONE = 0,
		STATE_CONNECT,
		STATE_CAPTURE,
		STATE_QUIT
	};

	virtual ~SensorThread();

	static SensorThread* instance(QObject* parent);
	static SensorThread* instance();

	State getState() const;

	void update(qreal dt);

protected:

	static const char _CONFIG[];

	static const qreal _UPDATE_INTERVAL; // ms

	static int _c0;

	mutable QMutex _mutex;

	State _state;
	State _s1;

	qint64 _t0;
	int _timer;

	xn::Context* _context;
	xn::DepthGenerator _depthGenerator;
	xn::ImageGenerator _imageGenerator;
	xn::UserGenerator _userGenerator;

	XnVSessionManager* _sessionManager;
	XnVPointDenoiser* _pointDenoiser;
	XnVSteadyDetector* _steadyDetector;
	XnVCircleDetector* _circleDetector;
	XnVSelectableSlider1D* _slideXDetector;
	XnVSelectableSlider1D* _slideYDetector;
	XnVSelectableSlider1D* _slideZDetector;
	XnVSwipeDetector* _swipeDetector;
	XnVPushDetector* _pushDetector;
	XnVWaveDetector* _waveDetector;

	QVector<quint8> _usersMap;

	SensorThread(QObject* parent);

	void init();
	void initState();

	void initTimer();
	void initContext();
	void initSession();

	void killSession();
	void killContext();
	
	void setState(State state);

	void run();

	void onStateEnter(State state);
	void onStateLeave(State state);

	static void XN_CALLBACK_TYPE onNewUser(xn::UserGenerator& generator, XnUserID user, void* cookie);
	static void XN_CALLBACK_TYPE onLostUser(xn::UserGenerator& generator, XnUserID user, void* cookie);

	static void XN_CALLBACK_TYPE onSessionStart(const XnPoint3D& position, void* context);
	static void XN_CALLBACK_TYPE onSessionEnd(void* context);

	static void XN_CALLBACK_TYPE onPrimaryPointCreate(const XnVHandPointContext* hand, const XnPoint3D& position, void* context);
	static void XN_CALLBACK_TYPE onPrimaryPointDestroy(XnUInt32 id, void* context);
	static void XN_CALLBACK_TYPE onPrimaryPointReplace(XnUInt32 id, const XnVHandPointContext* hand, void* context);

	static void XN_CALLBACK_TYPE onSteady(XnFloat velocity, void* context);
	static void XN_CALLBACK_TYPE onCircle(XnFloat count, XnBool confidence, const XnVCircle* circle, void* context);
	static void XN_CALLBACK_TYPE onSlideX(XnFloat value, void* context);
	static void XN_CALLBACK_TYPE onSlideY(XnFloat value, void* context);
	static void XN_CALLBACK_TYPE onSlideZ(XnFloat value, void* context);
	static void XN_CALLBACK_TYPE onSwipe(XnVDirection direction, XnFloat speed, XnFloat angle, void* context);
	static void XN_CALLBACK_TYPE onPush(XnFloat speed, XnFloat angle, void* context);
	static void XN_CALLBACK_TYPE onWave(void* context);

	void onUsersMap();

private:

	static SensorThread* _instance;
};

#endif // SENSORTHREAD_H
