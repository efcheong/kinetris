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

#include "Background.h"

#include "Game.h"

#include "LoaderThread.h"

const char Background::_IMAGE_NOISE[] = ":/res/noise.png";
const char Background::_IMAGE_FENCE[] = ":/res/fence.png";
const char Background::_IMAGE_COVER_BG[] = ":/res/cover-bg.png";
const char Background::_IMAGE_COVER_FG[] = ":/res/cover-fg.png";

const qreal Background::_BACKGROUND_W = 1024.0f;
const qreal Background::_BACKGROUND_H = 640.0f;
const qreal Background::_TEXTURE_W = 512.0f;
const qreal Background::_TEXTURE_H = 512.0f;
const qreal Background::_ROW_H = 16.0f;

Background::Background(Game* parent)
	: QObject(parent)
{
	init();
}

Background::~Background()
{
}

void Background::init()
{
	initSprite();
	initEffect();
}

void Background::initSprite()
{
	_sprite = new QGraphicsWidget();

	QGraphicsRectItem* rect;
	QGraphicsWidget* widget;

	// Background
	widget = new QGraphicsWidget();
	widget->resize(_BACKGROUND_W, _BACKGROUND_H);
	widget->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
	widget->setParentItem(_sprite);
	widget->setPos(0.0f, 0.0f);

	// Background Layer
	widget = new QGraphicsWidget();
	widget->setParentItem(_sprite);
	widget->setPos(0.0f, -_ROW_H);
	_sprite_bg << widget;

	// Background Solid
	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W, _BACKGROUND_H * 2.0f);
	rect->setBrush(QBrush(QColor::fromRgb(0x00, 0x80, 0xC0)));
	rect->setParentItem(widget);

	// Background Noise
	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W + 16.0f, _BACKGROUND_H * 2.0f);
	rect->setBrush(QBrush(LoaderThread::instance()->getCachedPixmap(_IMAGE_NOISE).scaled(_TEXTURE_W, _TEXTURE_H)));
	rect->setParentItem(widget);
	rect->setPos(-8.0f, -8.0f);

	// Background Layer
	widget = new QGraphicsWidget();
	widget->setParentItem(_sprite);
	widget->setPos(0.0f, 0.0f);
	_sprite_bg << widget;

	// Background Solid
	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W, _BACKGROUND_H * 2.0f);
	rect->setBrush(QBrush(QColor::fromRgb(0x00, 0x80, 0xC0)));
	rect->setParentItem(widget);

	// Background Noise
	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W + 16.0f, _BACKGROUND_H * 2.0f);
	rect->setBrush(QBrush(LoaderThread::instance()->getCachedPixmap(_IMAGE_NOISE).scaled(_TEXTURE_W, _TEXTURE_H)));
	rect->setParentItem(widget);
	rect->setPos(-8.0f, -8.0f);

	// Fence
	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W, _BACKGROUND_H);
	rect->setBrush(QBrush(LoaderThread::instance()->getCachedPixmap(_IMAGE_FENCE)));
	rect->setParentItem(_sprite);

	// Background Gradient
	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W, _BACKGROUND_H);
	rect->setBrush(QBrush(LoaderThread::instance()->getCachedPixmap(_IMAGE_COVER_BG)));
	rect->setParentItem(_sprite);
	
	// Foreground Gradient
	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W, _BACKGROUND_H);
	rect->setBrush(QBrush(LoaderThread::instance()->getCachedPixmap(_IMAGE_COVER_FG)));
	rect->setParentItem(_sprite);
}

void Background::initEffect()
{
	_speed = 1.0f;
	_moveTimer = 0.0f;
}

QGraphicsWidget* Background::getSprite() const
{
	return _sprite;
}

qreal Background::getSpeed() const
{
	return _speed;
}

void Background::setSpeed(qreal speed)
{
	_speed = speed;
}

void Background::update(qreal dt)
{
	qreal interval = 1000.0f / _speed;

	_moveTimer += dt;
	if (_moveTimer >= interval)
	{
		int d = _moveTimer / interval;
		_moveTimer = _moveTimer - (d * interval);
		_sprite_bg[0]->setY(fmod(_sprite_bg[0]->y() - (_ROW_H * d), _TEXTURE_H));
		_sprite_bg[1]->setY(fmod(_sprite_bg[1]->y() - (_ROW_H * d), _TEXTURE_H));
	}

	_sprite_bg[1]->setOpacity(1.0f - (_moveTimer / interval));
//	_sprite_bg[1]->setOpacity(1.0f - qAbs((_moveTimer / interval) - 0.5f) * 2.0f);
}