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

SensorThread::SensorThread(QObject* parent)
	: QThread(parent)
{
	init();
}

SensorThread::~SensorThread()
{
	setState(STATE_QUIT);

	// Wait for run method to return
	wait();
}

void SensorThread::init()
{
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

//	_depthHistogram.fill(0, 10000); // 10m

	_c0 = 0;

	initState();
}

void SensorThread::initState()
{
	_state = STATE_NONE;
	_s1 = STATE_NONE;
}

void SensorThread::initContext()
{
	_context->FindExistingNode(XN_NODE_TYPE_DEPTH, _depthGenerator);

	if (_context->FindExistingNode(XN_NODE_TYPE_IMAGE, _imageGenerator) == XN_STATUS_OK)
	{
		xn::ImageMetaData imageMetaData;
		_imageGenerator.GetMetaData(imageMetaData);
		_usersMap = QImage(imageMetaData.XRes(), imageMetaData.YRes(), QImage::Format_ARGB32_Premultiplied);
	}
	else
	{
		_imageGenerator = NULL;

		xn::DepthMetaData depthMetaData;
		_depthGenerator.GetMetaData(depthMetaData);
		_usersMap = QImage(depthMetaData.XRes(), depthMetaData.YRes(), QImage::Format_ARGB32_Premultiplied);
	}

	_context->FindExistingNode(XN_NODE_TYPE_USER, _userGenerator);

	XnCallbackHandle handle;
	_userGenerator.RegisterUserCallbacks(&SensorThread::onNewUser, &SensorThread::onLostUser, this, handle);

	_context->StartGeneratingAll();
}

void SensorThread::initSession()
{
	_sessionManager = new XnVSessionManager();
	_sessionManager->Initialize(_context, "Wave", "");
	_sessionManager->SetQuickRefocusTimeout(1500); // default=15000; ms
	_sessionManager->RegisterSession(this, &SensorThread::onSessionStart, &SensorThread::onSessionEnd);

	_pointDenoiser = new XnVPointDenoiser();
	_pointDenoiser->SetDistanceThreshold(10.0f); // default=10.0; mm
	_pointDenoiser->SetCloseRatio(0.0f); // default=0.0
	_pointDenoiser->SetFarRatio(1.0f); // default=1.0
	_pointDenoiser->RegisterPrimaryPointCreate(this, &SensorThread::onPrimaryPointCreate);
	_pointDenoiser->RegisterPrimaryPointDestroy(this, &SensorThread::onPrimaryPointDestroy);
	_pointDenoiser->RegisterPrimaryPointReplace(this, &SensorThread::onPrimaryPointReplace);
	_sessionManager->AddListener(_pointDenoiser);

	_steadyDetector = new XnVSteadyDetector();
	_steadyDetector->SetDetectionDuration(200); // default=200; ms
	_steadyDetector->SetMaximumStdDevForSteady(0.01f); // default=0.01; m/sec
	_steadyDetector->SetMinimumStdDevForNotSteady(0.02f); // default=0.02; m/sec
	_steadyDetector->RegisterSteady(this, &SensorThread::onSteady);
	_steadyDetector->RegisterNotSteady(this, &SensorThread::onNotSteady);
	_pointDenoiser->AddListener(_steadyDetector);

	_circleDetector = new XnVCircleDetector();
	_circleDetector->SetMinRadius(40.0f); // default=40.0; mm
	_circleDetector->SetMaxRadius(1200.0f); // default=1200.0; mm
	_circleDetector->SetExistingWeight(0.8f); // default=0.8
	_circleDetector->SetCloseToExistingRadius(50.0f); // default=50.0; mm
	_circleDetector->SetCloseEnough(1000.0f); // default=1000; mm^2
	_circleDetector->SetMinimumPoints(20); // default=20
	_circleDetector->SetMaxErrors(5); // default=5
	_circleDetector->RegisterCircle(this, &SensorThread::onCircle);
	_pointDenoiser->AddListener(_circleDetector);

	_slideXDetector = new XnVSelectableSlider1D(1, 0.0f, AXIS_X, false);
	_slideXDetector->SetSliderSize(150.0f); // default=250.0; mm
	_slideXDetector->SetBorderWidth(0.0f); // default=0.0
	_slideXDetector->SetHysteresisRatio(0.5f); // default=0.5
	_slideXDetector->SetValueChangeOnOffAxis(false); // default=false
	_slideXDetector->RegisterValueChange(this, &SensorThread::onSlideX);
	_pointDenoiser->AddListener(_slideXDetector);

	_slideYDetector = new XnVSelectableSlider1D(1, 0.0f, AXIS_Y, false);
	_slideYDetector->SetSliderSize(150.0f); // default=250.0; mm
	_slideYDetector->SetBorderWidth(0.0f); // default=0.0
	_slideYDetector->SetHysteresisRatio(0.5f); // default=0.5
	_slideYDetector->SetValueChangeOnOffAxis(false); // default=false
	_slideYDetector->RegisterValueChange(this, &SensorThread::onSlideY);
	_pointDenoiser->AddListener(_slideYDetector);

	_slideZDetector = new XnVSelectableSlider1D(1, 0.0f, AXIS_Z, false);
	_slideZDetector->SetSliderSize(150.0f); // default=250.0; mm
	_slideZDetector->SetBorderWidth(0.0f); // default=0.0
	_slideZDetector->SetHysteresisRatio(0.5f); // default=0.5
	_slideZDetector->SetValueChangeOnOffAxis(false); // default=false
	_slideZDetector->RegisterValueChange(this, &SensorThread::onSlideZ);
	_pointDenoiser->AddListener(_slideZDetector);

	_swipeDetector = new XnVSwipeDetector();
	_swipeDetector->SetUseSteady(true); // default=true
	_swipeDetector->SetSteadyDuration(200); // default=200; ms
	_swipeDetector->SetSteadyMaxStdDev(0.01f); // default=0.01; m/sec
	_swipeDetector->SetMotionTime(200); // default=350; ms
	_swipeDetector->SetMotionSpeedThreshold(0.4f); // default=0.25; m/sec
	_swipeDetector->SetXAngleThreshold(25.0f); // default=25.0; deg
	_swipeDetector->SetYAngleThreshold(20.0f); // default=20.0; deg
	_swipeDetector->RegisterSwipe(this, &SensorThread::onSwipe);
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
	_pushDetector->RegisterPush(this, &SensorThread::onPush);
	_pointDenoiser->AddListener(_pushDetector);

	_waveDetector = new XnVWaveDetector();
	_waveDetector->SetFlipCount(8); // default=4
	_waveDetector->SetMaxDeviation(50); // default=50; mm
	_waveDetector->SetMinLength(50); // default=50; mm
	_waveDetector->RegisterWave(this, &SensorThread::onWave);
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
//	onSessionEnd(this);
	delete _sessionManager;
	blockSignals(false);
}

void SensorThread::killContext()
{
	_context->StopGeneratingAll();
	_context->Release();
}

SensorThread::State SensorThread::getState() const
{
	QMutexLocker l(&_stateMutex);

	return _state;
}

void SensorThread::setState(State state)
{
	QMutexLocker l(&_stateMutex);

	_s1 = state;
}

void SensorThread::run()
{
	_context = new xn::Context();

	initState();

	while (getState() != STATE_QUIT)
	{
		msleep(0);

		update();
	}

	killSession();
	killContext();

	delete _context;
}

void SensorThread::update()
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
		setState(STATE_CONNECT);
	}
	else if (state == STATE_CONNECT)
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
	else if (state == STATE_CAPTURE)
	{
		XnStatus result = _context->WaitAndUpdateAll();
		if (result != XN_STATUS_OK)
		{
			emit evDisconnect();

			setState(STATE_QUIT);
			return;
		}
		
		_sessionManager->Update(_context);

		if (_imageGenerator)
			onImageMap();
		else
			onDepthMap();
	}
	else if (state == STATE_QUIT)
	{
	}
}

void SensorThread::onStateEnter(State state)
{
}

void SensorThread::onStateLeave(State state)
{
}

void XN_CALLBACK_TYPE SensorThread::onNewUser(xn::UserGenerator& generator, XnUserID user, void* self)
{
	emit static_cast<SensorThread*>(self)->evUserEnter();
}

void XN_CALLBACK_TYPE SensorThread::onLostUser(xn::UserGenerator& generator, XnUserID user, void* self)
{
	emit static_cast<SensorThread*>(self)->evUserLeave();
}

void XN_CALLBACK_TYPE SensorThread::onSessionStart(const XnPoint3D& position, void* self)
{
	emit static_cast<SensorThread*>(self)->evSessionBegin();
}

void XN_CALLBACK_TYPE SensorThread::onSessionEnd(void* self)
{
	emit static_cast<SensorThread*>(self)->evSessionEnd();
}

void XN_CALLBACK_TYPE SensorThread::onPrimaryPointCreate(const XnVHandPointContext* hand, const XnPoint3D& position, void* self)
{
	emit static_cast<SensorThread*>(self)->evFocusGain();
}

void XN_CALLBACK_TYPE SensorThread::onPrimaryPointDestroy(XnUInt32 id, void* self)
{
	emit static_cast<SensorThread*>(self)->evFocusLose();
}

void XN_CALLBACK_TYPE SensorThread::onPrimaryPointReplace(XnUInt32 id, const XnVHandPointContext* hand, void* self)
{
	emit static_cast<SensorThread*>(self)->evFocusSwap();
}

void XN_CALLBACK_TYPE SensorThread::onSteady(XnUInt32 id, XnFloat standardDeviation, void* self)
{
	static_cast<SensorThread*>(self)->_circleDetector->Reset();

	emit static_cast<SensorThread*>(self)->evSteadyBegin();
}

void XN_CALLBACK_TYPE SensorThread::onNotSteady(XnUInt32 id, XnFloat standardDeviation, void* self)
{
	emit static_cast<SensorThread*>(self)->evSteadyEnd();
}

void XN_CALLBACK_TYPE SensorThread::onCircle(XnFloat count, XnBool confidence, const XnVCircle* circle, void* self)
{
	int& _c0 = static_cast<SensorThread*>(self)->_c0;

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
			emit static_cast<SensorThread*>(self)->evCircle(d);
		}
	}
}

void XN_CALLBACK_TYPE SensorThread::onSlideX(XnFloat value, void* self)
{
	emit static_cast<SensorThread*>(self)->evSlideX((value - 0.5f) * 2.0f);
}

void XN_CALLBACK_TYPE SensorThread::onSlideY(XnFloat value, void* self)
{
	emit static_cast<SensorThread*>(self)->evSlideY((value - 0.5f) * 2.0f);
}

void XN_CALLBACK_TYPE SensorThread::onSlideZ(XnFloat value, void* self)
{
	emit static_cast<SensorThread*>(self)->evSlideZ((value - 0.5f) * 2.0f);
}

void XN_CALLBACK_TYPE SensorThread::onSwipe(XnVDirection direction, XnFloat speed, XnFloat angle, void* self)
{
	if (direction == DIRECTION_DOWN)
	{
		emit static_cast<SensorThread*>(self)->evSwipeY(-1, speed, angle);
	}
	else if (direction == DIRECTION_UP)
	{
		emit static_cast<SensorThread*>(self)->evSwipeY(1, speed, angle);
	}
	else if (direction == DIRECTION_LEFT)
	{
		emit static_cast<SensorThread*>(self)->evSwipeX(-1, speed, angle);
	}
	else if (direction == DIRECTION_RIGHT)
	{
		emit static_cast<SensorThread*>(self)->evSwipeX(1, speed, angle);
	}
}

void XN_CALLBACK_TYPE SensorThread::onPush(XnFloat speed, XnFloat angle, void* self)
{
	emit static_cast<SensorThread*>(self)->evPush(speed, angle);
}

void XN_CALLBACK_TYPE SensorThread::onWave(void* self)
{
	emit static_cast<SensorThread*>(self)->evWave();
}

void SensorThread::onDepthMap()
{
	xn::DepthMetaData depthMetaData;
	_depthGenerator.GetMetaData(depthMetaData);
	const XnDepthPixel* src = depthMetaData.Data();

	xn::SceneMetaData sceneMetaData;
	_userGenerator.GetUserPixels(0, sceneMetaData);
	const XnLabel* usr = sceneMetaData.Data();

//	// Too slow? Seems to prevent wave focus gesture from being recognized
//
//	_depthHistogram.fill(0);
//
//	const XnDepthPixel* depth = src;
//	int total = 0;
//	for (int i = 0, il = depthMetaData.XRes() * depthMetaData.YRes(); i < il; ++i)
//	{
//		if (*depth)
//		{
//			++_depthHistogram[*depth];
//			++total;
//		}
//
//		++depth;
//	}
//
//	if (total)
//	{
//		for (int i = 1, il = _depthHistogram.size(); i < il; ++i)
//		{
//			_depthHistogram[i] += _depthHistogram[i - 1];
//		}
//
//		for (int i = 0, il = _depthHistogram.size(); i < il; ++i)
//		{
//			_depthHistogram[i] = 0xC0 * (1.0f - (_depthHistogram[i] / static_cast<qreal>(total)));
//		}
//	}
//	
//	// Copy pixels occupied by users, set unoccupied pixels to transparent
//	QRgb* dst = reinterpret_cast<QRgb*>(_usersMap.bits());
//	for (int i = 0, il = _usersMap.width() * _usersMap.height(); i < il; ++i)
//	{
//		*dst = (*usr)
//			? qRgba(_depthHistogram[*src], _depthHistogram[*src], _depthHistogram[*src], 0xFF)
//			: 0x00000000;
//
//		++usr;
//		++src;
//		++dst;
//	}

	// Copy pixels occupied by users, set unoccupied pixels to transparent
	QRgb* dst = reinterpret_cast<QRgb*>(_usersMap.bits());
	for (int i = 0, il = _usersMap.width() * _usersMap.height(); i < il; ++i)
	{
		int n = 0xC0 * (1.0f - ((min(max(*src, 1000), 3000) - 1000) / static_cast<qreal>(3000 - 1000)));

		*dst = (*usr)
			? qRgba(n, n, n, 0xFF)
			: 0x00000000;

		++usr;
		++src;
		++dst;
	}

	emit evUsersMap(QPixmap::fromImage(_usersMap));
}

void SensorThread::onImageMap()
{
	xn::ImageMetaData imageMetaData;
	_imageGenerator.GetMetaData(imageMetaData);
	const XnRGB24Pixel* src = imageMetaData.RGB24Data();

	xn::SceneMetaData sceneMetaData;
	_userGenerator.GetUserPixels(0, sceneMetaData);
	const XnLabel* usr = sceneMetaData.Data();
	
	// Copy pixels occupied by users, set unoccupied pixels to transparent
	QRgb* dst = reinterpret_cast<QRgb*>(_usersMap.bits());
	for (int i = 0, il = _usersMap.width() * _usersMap.height(); i < il; ++i)
	{
		*dst = (*usr)
			? qRgba((*src).nRed, (*src).nGreen, (*src).nBlue, 0xFF)
			: 0x00000000;

		++usr;
		++src;
		++dst;
	}

	emit evUsersMap(QPixmap::fromImage(_usersMap));
}
