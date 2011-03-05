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

#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include <QtGui/QtGui>

class Game;
class VisualMatrix;

class PlayScreen : public QObject
{
	Q_OBJECT

public:

	PlayScreen(Game* parent);
	virtual ~PlayScreen();

	QGraphicsWidget* getSprite() const;

	void setMatrix(VisualMatrix* matrix);

	void show();
	void hide();

	void update(qreal dt);

protected:

	QGraphicsWidget* _sprite;

	QGraphicsWidget* _matrix;

	void init();
	void initSprite();
	void initEffect();
};

#endif // PLAYSCREEN_H
