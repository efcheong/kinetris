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

#ifndef TETROMINO_H
#define TETROMINO_H

#include <QtCore/QtCore>

#include "Pair.h"
#include "Ruleset.h"

class Matrix;

class Tetromino : public QObject
{
	Q_OBJECT

signals:

	void evMove(int count);
	void evMoveFail();
	void evTurn(int count);
	void evTurnFail();
	void evFall(int count);
	void evDrop(int count);
	void evLand();
	void evLock();

public:

	Tetromino(Ruleset::Piece piece, Matrix* parent);
	virtual ~Tetromino();

	Ruleset::Piece getPiece() const;

	Pair getPosition() const; // bottom-left
	void setPosition(Pair position); // bottom-left
	
	int getRotation() const;
	void setRotation(int rotation);
	
	QVector<Pair> getShape() const;

	bool getLocked() const;
	void setLocked(bool a);

	int move(int direction);
	int turn(int direction);
	int fall(int magnitude);
	int drop();
	void lock();

protected:

	Ruleset::Piece _piece;

	Pair _position; // bottom-left
	int _rotation;
	QVector<Pair> _shape;

	bool _locked;
};

#endif // TETROMINO_H
