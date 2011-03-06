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

#include "SensorThread.h"

const char SensorThread::_CONFIG[] = "OpenNI.xml";

const qreal SensorThread::_UPDATE_INTERVAL = 1000.0f / 30.0f; // ms

int SensorThread::_c0 = 0;

SensorThread* SensorThread::_instance = NULL;

SensorThread::SensorThread(QObject* parent)
	: QThread(parent),
	_mutex(QMutex::Recursive)
{
	init();
}

SensorThread::~SensorThread()
{
	setState(STATE_QUIT);

	// Wait for run method to return
	wait();
}

SensorThread* SensorThread::instance(QObject* parent)
{
	if (!_instance)
		_instance = new SensorThread(parent);

	return _instance;
}

SensorThread* SensorThread::instance()
{
	Q_ASSERT(_instance);

	return _instance;
}

void SensorThread::init()
{
	_t0 = 0;
	_timer = 0;

	_context = NULL;

	_sessionManager = NULL;
	_pointDenoiser = NULL;
	_steadyDetector = NULL;
	_circleDetector = NULL;
	_slideXDetector = NULL;
	_slideYDetector = NULL;
	_slideZDetector = NULL;
	_swipeDetector = NULL;
	_pushDetector = NULL;
	_waveDetector = NULL;

	initState();
}

void SensorThread::initState()
{
	_state = STATE_NONE;
	_s1 = STATE_NONE;
}

void SensorThread::initTimer()
{
	_t0 = QDateTime::currentMSecsSinceEpoch();
}

void SensorThread::initContext()
{
	_context->FindExistingNode(XN_NODE_TYPE_DEPTH, _depthGenerator);
	_context->FindExistingNode(XN_NODE_TYPE_IMAGE, _imageGenerator);
	_context->FindExistingNode(XN_NODE_TYPE_USER, _userGenerator);

	XnCallbackHandle handle;
	_userGenerator.RegisterUserCallbacks(&SensorThread::onNewUser, &SensorThread::onLostUser, NULL, handle);

	_context->StartGeneratingAll();
}

void SensorThread::initSession()
{
	_sessionManager = new XnVSessionManager();
	_sessionManager->Initialize(_context, "Wave", "");
	_sessionManager->SetQuickRefocusTimeout(15000); // default=15000; ms
	_sessionManager->RegisterSession(NULL, &SensorThread::onSessionStart, &SensorThread::onSessionEnd);

	_pointDenoiser = new XnVPointDenoiser();
	_pointDenoiser->SetDistanceThreshold(10.0f); // default=10.0; mm
	_pointDenoiser->SetCloseRatio(0.0f); // default=0.0
	_pointDenoiser->SetFarRatio(1.0f); // default=1.0
	_pointDenoiser->RegisterPrimaryPointCreate(NULL, &SensorThread::onPrimaryPointCreate);
	_pointDenoiser->RegisterPrimaryPointDestroy(NULL, &SensorThread::onPrimaryPointDestroy);
	_pointDenoiser->RegisterPrimaryPointReplace(NULL, &SensorThread::onPrimaryPointReplace);
	_sessionManager->AddListener(_pointDenoiser);

	_steadyDetector = new XnVSteadyDetector();
	_steadyDetector->SetDetectionDuration(200); // default=200; ms
	_steadyDetector->SetMaximumVelocity(0.01f); // default=0.01; m/sec
	_steadyDetector->RegisterSteady(NULL, &SensorThread::onSteady);
	_pointDenoiser->AddListener(_steadyDetector);

	_circleDetector = new XnVCircleDetector();
	_circleDetector->SetMinRadius(40.0f); // default=40.0; mm
	_circleDetector->SetMaxRadius(1200.0f); // default=1200.0; mm
	_circleDetector->SetExistingWeight(0.8f); // default=0.8
	_circleDetector->SetCloseToExistingRadius(50.0f); // default=50.0; mm
	_circleDetector->SetCloseEnough(1000.0f); // default=1000; mm^2
	_circleDetector->SetMinimumPoints(20); // default=20
	_circleDetector->SetMaxErrors(5); // default=5
	_circleDetector->RegisterCircle(NULL, &SensorThread::onCircle);
	_pointDenoiser->AddListener(_circleDetector);

	_slideXDetector = new XnVSelectableSlider1D(1, 0.0f, AXIS_X, false);
	_slideXDetector->SetSliderSize(150.0f); // default=250.0; mm
	_slideXDetector->SetBorderWidth(0.0f); // default=0.0
	_slideXDetector->SetHysteresisRatio(0.5f); // default=0.5
	_slideXDetector->SetValueChangeOnOffAxis(false); // default=false
	_slideXDetector->RegisterValueChange(NULL, &SensorThread::onSlideX);
	_pointDenoiser->AddListener(_slideXDetector);

	_slideYDetector = new XnVSelectableSlider1D(1, 0.0f, AXIS_Y, false);
	_slideYDetector->SetSliderSize(150.0f); // default=250.0; mm
	_slideYDetector->SetBorderWidth(0.0f); // default=0.0
	_slideYDetector->SetHysteresisRatio(0.5f); // default=0.5
	_slideYDetector->SetValueChangeOnOffAxis(false); // default=false
	_slideYDetector->RegisterValueChange(NULL, &SensorThread::onSlideY);
	_pointDenoiser->AddListener(_slideYDetector);

	_slideZDetector = new XnVSelectableSlider1D(1, 0.0f, AXIS_Z, false);
	_slideZDetector->SetSliderSize(150.0f); // default=250.0; mm
	_slideZDetector->SetBorderWidth(0.0f); // default=0.0
	_slideZDetector->SetHysteresisRatio(0.5f); // default=0.5
	_slideZDetector->SetValueChangeOnOffAxis(false); // default=false
	_slideZDetector->RegisterValueChange(NULL, &SensorThread::onSlideZ);
	_pointDenoiser->AddListener(_slideZDetector);

	_swipeDetector = new XnVSwipeDetector();
	_swipeDetector->SetUseSteady(true); // default=true
	_swipeDetector->SetSteadyDuration(200); // default=200; ms
	_swipeDetector->SetSteadyMaxVelocity(0.01f); // default=0.01; m/sec
	_swipeDetector->SetMotionTime(200); // default=350; ms
	_swipeDetector->SetMotionSpeedThreshold(0.4f); // default=0.25; m/sec
	_swipeDetector->SetXAngleThreshold(25.0f); // default=25.0; deg
	_swipeDetector->SetYAngleThreshold(20.0f); // default=20.0; deg
	_swipeDetector->RegisterSwipe(NULL, &SensorThread::onSwipe);
	_pointDenoiser->AddListener(_swipeDetector);

	_pushDetector = new XnVPushDetector();
	_pushDetector->SetPushImmediateOffset(0); // default=0; ms
	_pushDetector->SetPushImmediateDuration(240); // default=240; ms
	_pushDetector->SetPushImmediateMinimumVelocity(0.33f); // default=0.33; m/sec
	_pushDetector->SetPushMaximumAngleBetweenImmediateAndZ(30.0f); // default=30.0; deg
	_pushDetector->SetPushPreviousOffset(240); // default=240; ms
	_pushDetector->SetPushPreviousDuration(150); // default=150; ms
	_pushDetector->SetPushPreviousMinimumVelocity(0.17f); // default=0.17; m/sec
	_pushDetector->SetPushMinimumAngleBetweenImmediateAndPrevious(20.0f); // default=20.0; deg
	_pushDetector->SetStableDuration(240); // default=360; ms
	_pushDetector->SetStableMaximumVelocity(0.13f); // default=0.13; m/sec
	_pushDetector->RegisterPush(NULL, &SensorThread::onPush);
	_pointDenoiser->AddListener(_pushDetector);

	_waveDetector = new XnVWaveDetector();
	_waveDetector->SetFlipCount(8); // default=4
	_waveDetector->SetMaxDeviation(50); // default=50; mm
	_waveDetector->SetMinLength(50); // default=50; mm
	_waveDetector->RegisterWave(NULL, &SensorThread::onWave);
	_pointDenoiser->AddListener(_waveDetector);
}

void SensorThread::killSession()
{
	if (!_sessionManager)
		return;

	_pointDenoiser->RemoveListener(_waveDetector);
	delete _waveDetector;
	_pointDenoiser->RemoveListener(_pushDetector);
	delete _pushDetector;
	_pointDenoiser->RemoveListener(_swipeDetector);
	delete _swipeDetector;
	_pointDenoiser->RemoveListener(_slideZDetector);
	delete _slideZDetector;
	_pointDenoiser->RemoveListener(_slideYDetector);
	delete _slideYDetector;
	_pointDenoiser->RemoveListener(_slideXDetector);
	delete _slideXDetector;
	_pointDenoiser->RemoveListener(_circleDetector);
	delete _circleDetector;
	_pointDenoiser->RemoveListener(_steadyDetector);
	delete _steadyDetector;

	_sessionManager->RemoveListener(_pointDenoiser);
	delete _pointDenoiser;

	blockSignals(true);
	_sessionManager->EndSession();
//	onSessionEnd(NULL);
	delete _sessionManager;
	blockSignals(false);
}

void SensorThread::killContext()
{
	_context->StopGeneratingAll();
	_context->Shutdown();
}

SensorThread::State SensorThread::getState() const
{
	QMutexLocker l(&_mutex);

	return _state;
}

void SensorThread::update(qreal dt)
{
	QMutexLocker l(&_mutex);

	if (_s1 != _state)
	{
		onStateLeave(_state);
		_state = _s1;
		onStateEnter(_state);
	}

	if (!_state)
	{
		setState(STATE_CONNECT);
	}
	else if (_state == STATE_CONNECT)
	{
		emit evConnectBegin();

		XnStatus result = _context->InitFromXmlFile(_CONFIG);
		if (result != XN_STATUS_OK)
		{
			emit evConnectError();
			msleep(3000);
			return;
		}
		else
		{
			emit evConnect();
		}

		initContext();
		initSession();

		setState(STATE_CAPTURE);
	}
	else if (_state == STATE_CAPTURE)
	{
		XnStatus result = _context->WaitAndUpdateAll();
		if (result != XN_STATUS_OK)
		{
			emit evDisconnect();

			setState(STATE_QUIT);
			return;
		}
		
		_sessionManager->Update(_context);

		onUsersMap();
	}
	else if (_state == STATE_QUIT)
	{
	}
}

void SensorThread::setState(State state)
{
	QMutexLocker l(&_mutex);

	_s1 = state;
}

void SensorThread::run()
{
	_context = new xn::Context();

	initState();
	initTimer();

	forever
	{
		QMutexLocker l(&_mutex);

		qint64 t = QDateTime::currentMSecsSinceEpoch();
		int dt = qMin<qreal>(_UPDATE_INTERVAL, t - _t0);
		update(dt);
		_t0 = t;

		if (_state == STATE_QUIT)
			break;
	}

	killSession();
	killContext();

	delete _context;
}

void SensorThread::onStateEnter(State state)
{
}

void SensorThread::onStateLeave(State state)
{
}

void XN_CALLBACK_TYPE SensorThread::onNewUser(xn::UserGenerator& generator, XnUserID user, void* cookie)
{
	emit _instance->evUserEnter();
}

void XN_CALLBACK_TYPE SensorThread::onLostUser(xn::UserGenerator& generator, XnUserID user, void* cookie)
{
	emit _instance->evUserLeave();
}

void XN_CALLBACK_TYPE SensorThread::onSessionStart(const XnPoint3D& position, void* context)
{
	emit _instance->evSessionBegin();
}

void XN_CALLBACK_TYPE SensorThread::onSessionEnd(void* context)
{
	emit _instance->evSessionEnd();
}

void XN_CALLBACK_TYPE SensorThread::onPrimaryPointCreate(const XnVHandPointContext* hand, const XnPoint3D& position, void* context)
{
	emit _instance->evFocusGain();
}

void XN_CALLBACK_TYPE SensorThread::onPrimaryPointDestroy(XnUInt32 id, void* context)
{
	emit _instance->evFocusLose();
}

void XN_CALLBACK_TYPE SensorThread::onPrimaryPointReplace(XnUInt32 id, const XnVHandPointContext* hand, void* context)
{
	emit _instance->evFocusSwap();
}

void XN_CALLBACK_TYPE SensorThread::onSteady(XnFloat velocity, void* context)
{
	_instance->_circleDetector->Reset();

	emit _instance->evSteady();
}

void XN_CALLBACK_TYPE SensorThread::onCircle(XnFloat count, XnBool confidence, const XnVCircle* circle, void* context)
{
	if (!count)
	{
		_c0 = 0;
	}
	else
	{
		int d = count - _c0;
		if (d)
		{
			_c0 = count;
			emit _instance->evCircle(d);
		}
	}
}

void XN_CALLBACK_TYPE SensorThread::onSlideX(XnFloat value, void* context)
{
	emit _instance->evSlideX((value - 0.5f) * 2.0f);
}

void XN_CALLBACK_TYPE SensorThread::onSlideY(XnFloat value, void* context)
{
	emit _instance->evSlideY((value - 0.5f) * 2.0f);
}

void XN_CALLBACK_TYPE SensorThread::onSlideZ(XnFloat value, void* context)
{
	emit _instance->evSlideZ((value - 0.5f) * 2.0f);
}

void XN_CALLBACK_TYPE SensorThread::onSwipe(XnVDirection direction, XnFloat speed, XnFloat angle, void* context)
{
	if (direction == DIRECTION_DOWN)
	{
		emit _instance->evSwipeY(-1, speed, angle);
	}
	else if (direction == DIRECTION_UP)
	{
		emit _instance->evSwipeY(1, speed, angle);
	}
	else if (direction == DIRECTION_LEFT)
	{
		emit _instance->evSwipeX(-1, speed, angle);
	}
	else if (direction == DIRECTION_RIGHT)
	{
		emit _instance->evSwipeX(1, speed, angle);
	}
}

void XN_CALLBACK_TYPE SensorThread::onPush(XnFloat speed, XnFloat angle, void* context)
{
	emit _instance->evPush(speed, angle);
}

void XN_CALLBACK_TYPE SensorThread::onWave(void* context)
{
	emit _instance->evWave();
}

void SensorThread::onUsersMap()
{
	xn::ImageMetaData imageMetaData;
	_imageGenerator.GetMetaData(imageMetaData);
	QImage image = QImage(imageMetaData.FullXRes(), imageMetaData.FullYRes(), QImage::Format_ARGB32);
	int il = image.height() * image.width();

	XnUserID users[16];
	XnUInt16 ul = 16;
	_userGenerator.GetUsers(users, ul);

	// Create mask for all pixels enclosing all users in scene
	_usersMap.fill(0x00, il);
	xn::SceneMetaData sceneMetaData;
	const XnLabel* pu;
	for (int u = 0; u < ul; ++u)
	{
		_userGenerator.GetUserPixels(users[u], sceneMetaData);
		pu = sceneMetaData.Data();
		for (int i = 0; i < il; ++i)
		{
			if (pu[i])
				_usersMap[i] = 0xFF;
		}
	}

	const XnRGB24Pixel* p0 = imageMetaData.RGB24Data();
	QRgb* p1 = reinterpret_cast<QRgb*>(image.bits());
	for (int i = 0; i < il; ++i)
	{
		p1[i] = qRgba(p0[i].nRed, p0[i].nGreen, p0[i].nBlue, _usersMap[i]);
	}

	emit evUsersMap(image);
}
