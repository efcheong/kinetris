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

#include "MenuScreen.h"

#include "Game.h"

#include "LoaderThread.h"

const qreal MenuScreen::_BACKGROUND_W = 1024.0f;
const qreal MenuScreen::_BACKGROUND_H = 640.0f;

MenuScreen::MenuScreen(Game* parent)
	: QObject(parent)
{
	init();
}

MenuScreen::~MenuScreen()
{
}

void MenuScreen::init()
{
	initSprite();
	initEffect();
}

void MenuScreen::initSprite()
{
	_sprite = new QGraphicsWidget();

	QGraphicsRectItem* rect;
	QGraphicsSimpleTextItem* text;
	QGraphicsProxyWidget* proxy;
	QLabel* label;

	rect = new QGraphicsRectItem(0.0f, 0.0f, _BACKGROUND_W, _BACKGROUND_H);
	rect->setBrush(QBrush(QColor::fromRgb(0x00, 0x00, 0x00, 0xC0)));
	rect->setParentItem(_sprite);

	QFont font = QApplication::font();
	font.setCapitalization(QFont::AllUppercase);

	text = new QGraphicsSimpleTextItem(tr("Paused"));
	text->setBrush(QColor::fromRgb(0xFF, 0xFF, 0xFF));
	text->setFont(font);
	text->setParentItem(_sprite);
	text->setPos((_BACKGROUND_W - text->boundingRect().width()) * 0.5f, 268.0f - 6.0f);

	label = new QLabel();
	label->resize(784, 24);
	label->setStyleSheet("background-color: transparent; color: #FFFFFF;");
	label->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	proxy = new QGraphicsProxyWidget();
	proxy->setParentItem(_sprite);
	proxy->setWidget(label);
	proxy->setPos(120.0f, 308.0f - 6.0f);
	_status = label;
}

void MenuScreen::initEffect()
{
}

QGraphicsWidget* MenuScreen::getSprite() const
{
	return _sprite;
}

QString MenuScreen::getStatus() const
{
	return _status->text();
}

void MenuScreen::setStatus(const QString& status)
{
	_status->setText(status);
}

void MenuScreen::show()
{
	_sprite->setVisible(true);
}

void MenuScreen::hide()
{
	_sprite->setVisible(false);
	setStatus(tr("Wave to continue."));
}

void MenuScreen::update(qreal dt)
{
}
