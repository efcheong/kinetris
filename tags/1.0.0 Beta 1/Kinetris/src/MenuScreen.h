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

#ifndef MENUSCREEN_H
#define MENUSCREEN_H

#include <QtGui/QtGui>

class Game;

class MenuScreen : public QObject
{
	Q_OBJECT

public:

	MenuScreen(Game* parent);
	virtual ~MenuScreen();

	QGraphicsWidget* getSprite() const;

	QString getStatus() const;
	void setStatus(const QString& status);

	void show();
	void hide();

	void update(qreal dt);

protected:

	static const qreal _BACKGROUND_W; // px
	static const qreal _BACKGROUND_H; // px

	QGraphicsWidget* _sprite;

	QLabel* _status;

	void init();
	void initSprite();
	void initEffect();
};

#endif // MENUSCREEN_H
