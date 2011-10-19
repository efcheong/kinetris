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

#ifndef KINETRIS_H
#define KINETRIS_H

#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

class Game;

class Kinetris : public QMainWindow
{
	Q_OBJECT

public:

	Kinetris();
	virtual ~Kinetris();

	QGraphicsView* getView() const;

protected:

	QGraphicsView* _view;
};

#endif // KINETRIS_H
