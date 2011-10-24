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

#include "QuitScreen.h"

#include "Game.h"

#include "LoaderThread.h"

const qreal QuitScreen::_BACKGROUND_W = 1280.0f;
const qreal QuitScreen::_BACKGROUND_H = 720.0f;

QuitScreen::QuitScreen(Game* parent)
	: QObject(parent)
{
	init();
}

QuitScreen::~QuitScreen()
{
}

void QuitScreen::init()
{
	initSprite();
}

void QuitScreen::initSprite()
{
	_sprite = new QGraphicsWidget();

	QGraphicsRectItem* rect;
	QGraphicsSimpleTextItem* text;

	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W, _BACKGROUND_H);
	rect->setBrush(QBrush(QColor::fromRgb(0x00, 0x00, 0x00, 0xC0)));
	rect->setParentItem(_sprite);

	text = new QGraphicsSimpleTextItem(tr("Are you sure you want to quit?"));
	text->setBrush(QColor::fromRgb(0xFF, 0xFF, 0xFF));
	text->setParentItem(_sprite);
	text->setPos((_BACKGROUND_W - text->boundingRect().width()) * 0.5f, 308.0f - 6.0f);

	text = new QGraphicsSimpleTextItem(tr("Swipe left: Yes.  Swipe right: No."));
	text->setBrush(QColor::fromRgb(0xFF, 0xFF, 0xFF));
	text->setParentItem(_sprite);
	text->setPos((_BACKGROUND_W - text->boundingRect().width()) * 0.5f, 348.0f - 6.0f);
}

QGraphicsWidget* QuitScreen::getSprite() const
{
	return _sprite;
}

void QuitScreen::show()
{
	_sprite->setVisible(true);
}

void QuitScreen::hide()
{
	_sprite->setVisible(false);
}

void QuitScreen::update(qreal dt)
{
}
