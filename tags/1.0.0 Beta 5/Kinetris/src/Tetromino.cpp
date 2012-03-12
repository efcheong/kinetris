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

#include "Tetromino.h"

#include "Matrix.h"

Tetromino::Tetromino(Ruleset::Piece piece, Matrix* parent)
	: QObject(parent)
{
	_piece = piece;

	_position.row = 0;
	_position.col = 0;
	_rotation = 0;
	_shape = parent->getRules()->getRotationShape(piece, 0);

	_locked = false;
}

Tetromino::~Tetromino()
{
}

Ruleset::Piece Tetromino::getPiece() const
{
	return _piece;
}

Pair Tetromino::getPosition() const
{
	return _position;
}

void Tetromino::setPosition(Pair position)
{
	_position = position;
}

int Tetromino::getRotation() const
{
	return _rotation;
}

void Tetromino::setRotation(int rotation)
{
	_rotation = rotation;
	_shape = static_cast<Matrix*>(parent())->getRules()->getRotationShape(_piece, _rotation);
}

QVector<Pair> Tetromino::getShape() const
{
	return _shape;
}

bool Tetromino::getLocked() const
{
	return _locked;
}

void Tetromino::setLocked(bool a)
{
	_locked = a;
}

int Tetromino::move(int direction)
{
	if (_locked)
		return 0;

	Matrix* m = static_cast<Matrix*>(parent());

	// Find maximum possible step distance
	int dir = (direction < 0) ? -1 : 1;
	int magnitude = qAbs(direction);
	int min = magnitude;
	Pair p;
	foreach (const Pair& block, _shape)
	{
		for (int i = 1; i <= min; ++i)
		{
			p.row = _position.row + block.row;
			p.col = _position.col + block.col + (dir * i);

			// Test bounds and collision
			if (m->occupied(p))
			{
				// New minimum
				if (i - 1 < min)
				{
					min = i - 1;
				}
			}
		}
	}

	int d;
	if (min <= 0)
	{
		d = 0;
		emit evMoveFail();
	}
	else
	{
		d = dir * min;
		_position.col += d;
		emit evMove(d);
	}

	return d;
}

int Tetromino::turn(int direction)
{
	if (_locked)
		return 0;

	Matrix* m = static_cast<Matrix*>(parent());

	// Find maximum possible rotations
	int dir = (direction < 0) ? -1 : 1;
	int magnitude = qAbs(direction);
	int min = magnitude;
	QVector<Pair> nudgeList;
	QVector<Pair> blockList;
	Pair p;
	for (int i = 1; i <= min; ++i)
	{
		nudgeList = m->getRules()->getRotationNudge(_piece, _rotation + (dir * (i - 1)), dir);
		blockList = m->getRules()->getRotationShape(_piece, _rotation + (dir * i));

		foreach (const Pair& nudge, nudgeList)
		{
			foreach (const Pair& block, blockList)
			{
				p.row = _position.row + block.row + nudge.row;
				p.col = _position.col + block.col + nudge.col;

				// Test bounds and collision
				if (m->occupied(p))
				{
					// This nudge won't work
					goto L1;
					// break
				}
			}

			// No invalid blocks
			// Nudge
			_position.row += nudge.row;
			_position.col += nudge.col;
			goto L0;
			// break

			L1: ;
		}

		// No valid nudges
		min = i - 1;
		break;

		L0: ;
	}

	int d;
	if (min <= 0)
	{
		d = 0;
		emit evTurnFail();
	}
	else
	{
		d = dir * min;
		_rotation += d;
		_shape = m->getRules()->getRotationShape(_piece, _rotation);
		emit evTurn(d);
	}

	return d;
}

int Tetromino::fall(int magnitude)
{
	Q_ASSERT(magnitude >= 0);

	if (_locked)
		return 0;

	Matrix* m = static_cast<Matrix*>(parent());

	// Find maximum possible fall distance
	int min = magnitude;
	Pair p;
	foreach (const Pair& block, _shape)
	{
		for (int i = 1; i <= min; ++i)
		{
			p.row = _position.row + block.row - i;
			p.col = _position.col + block.col;

			// Test bounds and collision
			if (m->occupied(p))
			{
				// New minimum
				if (i - 1 < min)
				{
					min = i - 1;
				}
			}
		}
	}

	int d;
	if (min <= 0)
	{
		d = 0;
		emit evLand();
	}
	else
	{
		d = min;
		_position.row -= d;
		emit evFall(d);
	}

	return d;
}

int Tetromino::drop()
{
	if (_locked)
		return 0;

	Matrix* m = static_cast<Matrix*>(parent());

	blockSignals(true);
	int d = fall(m->getRows());
	blockSignals(false);

	if (d)
	{
		emit evDrop(d);
		emit evLand();
//		lock();
	}

	return d;
}

void Tetromino::lock()
{
	if (_locked)
		return;

	_locked = true;
	emit evLock();
}
