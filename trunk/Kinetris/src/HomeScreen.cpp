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

#include "HomeScreen.h"

#include "Game.h"

#include "LoaderThread.h"

const char HomeScreen::_IMAGE_TITLE[] = ":/res/title.png";

const qreal HomeScreen::_BACKGROUND_W = 1280.0f;
const qreal HomeScreen::_BACKGROUND_H = 720.0f;

const qreal HomeScreen::_SHOWEFFECT_DURATION = 1.0f; // sec
const qreal HomeScreen::_HIDEEFFECT_DURATION = 1.0f; // sec

const qreal HomeScreen::_GLOWEFFECT_DURATION = 1.0f; // sec

HomeScreen::HomeScreen(Game* parent)
	: QObject(parent)
{
	init();
}

HomeScreen::~HomeScreen()
{
}

void HomeScreen::init()
{
	initSprite();
	initEffect();
}

void HomeScreen::initSprite()
{
	_sprite = new QGraphicsWidget();

	QGraphicsItem* item;
	QGraphicsSimpleTextItem* text;
	QGraphicsProxyWidget* proxy;
	QLabel* label;
	QFont font;

	item = new QGraphicsPixmapItem();
	item->setParentItem(_sprite);
	item->setPos((_BACKGROUND_W - (400.0f / 0.75f)) * 0.5f, _BACKGROUND_H - 400.0f);
	_avatar = static_cast<QGraphicsPixmapItem*>(item);

	item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(_IMAGE_TITLE));
	item->setParentItem(_sprite);
	item->setPos(232.0f, 128.0f);

	font = QFont("Arial");
	font.setPixelSize(12);
	font.setWeight(QFont::Normal);
	font.setStretch(80);

	text = new QGraphicsSimpleTextItem("http://conanchen.com/Kinetris");
	text->setBrush(QColor::fromRgb(0xFF, 0xFF, 0xFF));
	text->setFont(font);
	text->setParentItem(_sprite);
	text->setPos(248.0f, 264.0f - 3.0f);

	label = new QLabel();
	label->resize(784, 24);
	label->setStyleSheet("background-color: transparent; color: #FFFFFF;");
	label->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	proxy = new QGraphicsProxyWidget();
	proxy->setParentItem(_sprite);
	proxy->setWidget(label);
	proxy->setPos(248.0f, 592.0f - 6.0f);
	_sprite_status = proxy;
	_status = label;
}

void HomeScreen::initEffect()
{
	QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect(_sprite);
	effect->setColor(QColor::fromRgb(0xFF, 0xFF, 0xFF));
	effect->setStrength(0.0f);
	_sprite->setGraphicsEffect(effect);

	_showEffectTimer = new QTimeLine(_SHOWEFFECT_DURATION, this);
	_hideEffectTimer = new QTimeLine(_HIDEEFFECT_DURATION, this);

	_glowTimer = 0.75f;
}

QGraphicsWidget* HomeScreen::getSprite() const
{
	return _sprite;
}

QString HomeScreen::getStatus() const
{
	return _status->text();
}

void HomeScreen::setStatus(const QString& status)
{
	if (_status->text().isEmpty())
	{
		_status->setText(status);
		_glowTimer = 0.75f;
	}

	_s1 = status;
}

void HomeScreen::setAvatar(QPixmap pixmap)
{
	_avatar->setPixmap(pixmap);
	_avatar->setScale(400.0f / pixmap.height());
}

void HomeScreen::show()
{
	_sprite->setVisible(true);
}

void HomeScreen::hide()
{
	_sprite->setVisible(false);
	setStatus(tr("Connecting to sensor..."));
}

void HomeScreen::update(qreal dt)
{
	if (_showEffectTimer->state() != QTimeLine::NotRunning)
	{
		//
	}

	if (_hideEffectTimer->state() != QTimeLine::NotRunning)
	{
		//
	}

	if (!((_status->text().isEmpty())
		&& (_s1.isEmpty())))
	{
		qreal t0 = _glowTimer / (_GLOWEFFECT_DURATION * 1000.0f);
		_glowTimer = fmod(_glowTimer + dt, _GLOWEFFECT_DURATION * 1000.0f);
		qreal t = _glowTimer / (_GLOWEFFECT_DURATION * 1000.0f);

		_sprite_status->setOpacity(0.375f * sin(t * 6.283185307179586476925286766559) + 0.625f);

		if ((_s1 != _status->text())
			&& ((t0 < 0.75f) && (t >= 0.75f)))
		{
			_status->setText(_s1);
		}
	}
}
