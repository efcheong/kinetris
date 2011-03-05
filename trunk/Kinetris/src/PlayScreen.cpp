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

#include "PlayScreen.h"

#include "Game.h"
#include "VisualMatrix.h"

#include "LoaderThread.h"

PlayScreen::PlayScreen(Game* parent)
	: QObject(parent)
{
	init();
}

PlayScreen::~PlayScreen()
{
}

void PlayScreen::init()
{
	initSprite();
	initEffect();
}

void PlayScreen::initSprite()
{
	_sprite = new QGraphicsWidget();

	_matrix = NULL;
}

void PlayScreen::initEffect()
{
	//
}

QGraphicsWidget* PlayScreen::getSprite() const
{
	return _sprite;
}

void PlayScreen::setMatrix(VisualMatrix* matrix)
{
	if (_matrix)
	{
		_matrix->setParentItem(NULL);
		_matrix->setPos(0.0f, 0.0f);
		_matrix->setVisible(false);
	}

	_matrix = matrix->getSprite();

	_matrix->setParentItem(_sprite);
	_matrix->setPos(222.0f, 4.0f);
	_matrix->setVisible(true);
}

void PlayScreen::show()
{
	_sprite->setVisible(true);
}

void PlayScreen::hide()
{
	_sprite->setVisible(false);
}

void PlayScreen::update(qreal dt)
{
	//
}
