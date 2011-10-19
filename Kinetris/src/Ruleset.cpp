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

#include "Ruleset.h"

// -row = Down
// +row = Up
// -col = Left
// +col = Right

const Pair Ruleset::_ROTATION_SHAPE[_PIECE_TOTAL * _SHAPE_TOTAL * _BLOCK_TOTAL] = {
	// Piece I

	// Orientation 0 (clockwise)
	{2, 0}, {2, 1}, {2, 2}, {2, 3},

	// Orientation 1 (clockwise)
	{0, 2}, {1, 2}, {2, 2}, {3, 2},

	// Orientation 2 (clockwise)
	{1, 0}, {1, 1}, {1, 2}, {1, 3},

	// Orientation 3 (clockwise)
	{0, 1}, {1, 1}, {2, 1}, {3, 1},

	// Piece O

	// Orientation 0 (clockwise)
	{2, 1}, {2, 2}, {3, 1}, {3, 2},

	// Orientation 1 (clockwise)
	{2, 1}, {2, 2}, {3, 1}, {3, 2},

	// Orientation 2 (clockwise)
	{2, 1}, {2, 2}, {3, 1}, {3, 2},

	// Orientation 3 (clockwise)
	{2, 1}, {2, 2}, {3, 1}, {3, 2},

	// Piece T

	// Orientation 0 (clockwise)
	{2, 0}, {2, 1}, {2, 2}, {3, 1},

	// Orientation 1 (clockwise)
	{1, 1}, {2, 1}, {2, 2}, {3, 1},

	// Orientation 2 (clockwise)
	{1, 1}, {2, 0}, {2, 1}, {2, 2},

	// Orientation 3 (clockwise)
	{1, 1}, {2, 0}, {2, 1}, {3, 1},
		
	// Piece S

	// Orientation 0 (clockwise)
	{2, 0}, {2, 1}, {3, 1}, {3, 2},

	// Orientation 1 (clockwise)
	{1, 2}, {2, 1}, {2, 2}, {3, 1},

	// Orientation 2 (clockwise)
	{1, 0}, {1, 1}, {2, 1}, {2, 2},

	// Orientation 3 (clockwise)
	{1, 1}, {2, 0}, {2, 1}, {3, 0},
		
	// Piece Z

	// Orientation 0 (clockwise)
	{2, 1}, {2, 2}, {3, 0}, {3, 1},

	// Orientation 1 (clockwise)
	{1, 1}, {2, 1}, {2, 2}, {3, 2},

	// Orientation 2 (clockwise)
	{1, 1}, {1, 2}, {2, 0}, {2, 1},

	// Orientation 3 (clockwise)
	{1, 0}, {2, 0}, {2, 1}, {3, 1},
		
	// Piece J

	// Orientation 0 (clockwise)
	{2, 0}, {2, 1}, {2, 2}, {3, 0},

	// Orientation 1 (clockwise)
	{1, 1}, {2, 1}, {3, 1}, {3, 2},

	// Orientation 2 (clockwise)
	{1, 2}, {2, 0}, {2, 1}, {2, 2},

	// Orientation 3 (clockwise)
	{1, 0}, {1, 1}, {2, 1}, {3, 1},
		
	// Piece L

	// Orientation 0 (clockwise)
	{2, 0}, {2, 1}, {2, 2}, {3, 2},

	// Orientation 1 (clockwise)
	{1, 1}, {1, 2}, {2, 1}, {3, 1},

	// Orientation 2 (clockwise)
	{1, 0}, {2, 0}, {2, 1}, {2, 2},

	// Orientation 3 (clockwise)
	{1, 1}, {2, 1}, {3, 0}, {3, 1}
};

// -row = Down
// +row = Up
// -col = Left
// +col = Right

const Pair Ruleset::_ROTATION_NUDGE[_PIECE_TOTAL * _SHAPE_TOTAL * _TURNDIRECTION_TOTAL * _NUDGE_TOTAL] = {
	// Piece I

	// Orientation 0 (clockwise)
	{0, 0}, {0, 1}, {0, -2}, {-1, -2}, {2, 1}, // Clockwise
	{0, 0}, {0, -1}, {0, 2}, {-1, 2}, {2, -1}, // Counterclockwise

	// Orientation 1 (clockwise)
	{0, 0}, {0, -1}, {0, 2}, {-1, 2}, {2, -1}, // Clockwise
	{0, 0}, {0, -1}, {0, 2}, {-2, -1}, {1, 2}, // Counterclockwise

	// Orientation 2 (clockwise)
	{0, 0}, {0, -1}, {0, 2}, {-1, -1}, {1, 2}, // Clockwise
	{0, 0}, {0, 1}, {0, -2}, {-1, 1}, {1, -2}, // Counterclockwise

	// Orientation 3 (clockwise)
	{0, 0}, {0, 1}, {0, -2}, {-2, 1}, {1, -2}, // Clockwise
	{0, 0}, {0, 1}, {0, -2}, {-1, -2}, {2, 1}, // Counterclockwise

	// Piece O

	// Orientation 0 (clockwise)
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Clockwise
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Counterclockwise

	// Orientation 1 (clockwise)
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Clockwise
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Counterclockwise

	// Orientation 2 (clockwise)
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Clockwise
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Counterclockwise

	// Orientation 3 (clockwise)
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Clockwise
	{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, // Counterclockwise

	// Piece T

	// Orientation 0 (clockwise)
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Clockwise
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Counterclockwise

	// Orientation 1 (clockwise)
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Clockwise
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Counterclockwise

	// Orientation 2 (clockwise)
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Clockwise
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Counterclockwise

	// Orientation 3 (clockwise)
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Clockwise
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Counterclockwise

	// Piece S

	// Orientation 0 (clockwise)
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Clockwise
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Counterclockwise

	// Orientation 1 (clockwise)
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Clockwise
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Counterclockwise

	// Orientation 2 (clockwise)
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Clockwise
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Counterclockwise

	// Orientation 3 (clockwise)
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Clockwise
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Counterclockwise

	// Piece Z

	// Orientation 0 (clockwise)
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Clockwise
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Counterclockwise

	// Orientation 1 (clockwise)
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Clockwise
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Counterclockwise

	// Orientation 2 (clockwise)
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Clockwise
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Counterclockwise

	// Orientation 3 (clockwise)
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Clockwise
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Counterclockwise

	// Piece J

	// Orientation 0 (clockwise)
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Clockwise
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Counterclockwise

	// Orientation 1 (clockwise)
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Clockwise
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Counterclockwise

	// Orientation 2 (clockwise)
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Clockwise
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Counterclockwise

	// Orientation 3 (clockwise)
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Clockwise
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Counterclockwise

	// Piece L

	// Orientation 0 (clockwise)
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Clockwise
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Counterclockwise

	// Orientation 1 (clockwise)
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Clockwise
	{0, 0}, {0, 1}, {-1, 1}, {2, 0}, {2, 1}, // Counterclockwise

	// Orientation 2 (clockwise)
	{0, 0}, {0, 1}, {1, 1}, {-2, 0}, {-2, 1}, // Clockwise
	{0, 0}, {0, -1}, {1, -1}, {-2, 0}, {-2, -1}, // Counterclockwise

	// Orientation 3 (clockwise)
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1}, // Clockwise
	{0, 0}, {0, -1}, {-1, -1}, {2, 0}, {2, -1} // Counterclockwise
};

// -row = Down
// +row = Up
// -col = Left
// +col = Right

const Pair Ruleset::_POSITION_START[_PIECE_TOTAL] = {
	// Piece I
	{18, 3},

	// Piece O
	{18, 3},

	// Piece T
	{18, 3},

	// Piece S
	{18, 3},

	// Piece Z
	{18, 3},

	// Piece J
	{18, 3},

	// Piece L
	{18, 3}
};

Ruleset::Ruleset()
	: QObject()
{
	_rotationShape.reserve(_PIECE_TOTAL * _SHAPE_TOTAL * _BLOCK_TOTAL);
	for (int i = 0, il = _PIECE_TOTAL * _SHAPE_TOTAL * _BLOCK_TOTAL; i < il; ++i)
	{
		_rotationShape << _ROTATION_SHAPE[i];
	}

	_rotationNudge.reserve(_PIECE_TOTAL * _SHAPE_TOTAL * _TURNDIRECTION_TOTAL * _NUDGE_TOTAL);
	for (int i = 0, il = _PIECE_TOTAL * _SHAPE_TOTAL * _TURNDIRECTION_TOTAL * _NUDGE_TOTAL; i < il; ++i)
	{
		_rotationNudge << _ROTATION_NUDGE[i];
	}
}

Ruleset::~Ruleset()
{
}

int Ruleset::getRotation(Piece piece, int rotation) const
{
	return rotation % _SHAPE_TOTAL;
}

QVector<Pair> Ruleset::getRotationShape(Piece piece, int rotation) const
{
	return _rotationShape.mid(((piece - 1) * _SHAPE_TOTAL * _BLOCK_TOTAL)
		+ (((rotation < 0) ? (_SHAPE_TOTAL - 1) + ((rotation + 1) % _SHAPE_TOTAL) : rotation % _SHAPE_TOTAL) * _BLOCK_TOTAL),
		_BLOCK_TOTAL);
}

QVector<Pair> Ruleset::getRotationNudge(Piece piece, int rotation, int direction) const
{
	return _rotationNudge.mid(((piece - 1) * _SHAPE_TOTAL * _TURNDIRECTION_TOTAL * _NUDGE_TOTAL)
		+ (((rotation < 0) ? (_SHAPE_TOTAL - 1) + ((rotation + 1) % _SHAPE_TOTAL) : rotation % _SHAPE_TOTAL) * _TURNDIRECTION_TOTAL * _NUDGE_TOTAL)
		+ (((direction < 0) ? 1 : 0) * _NUDGE_TOTAL),
		_NUDGE_TOTAL);
}

Pair Ruleset::getStartPosition(Piece piece) const
{
	return _POSITION_START[(piece - 1)];
}

Pair Ruleset::getAtticPosition() const
{
	Pair p = {20, 0};
	return p;
}

int Ruleset::getRows() const
{
	return 22;
}

int Ruleset::getCols() const
{
	return 10;
}

int Ruleset::getLines(int level) const
{
	return (level - 1) * 4;
}

int Ruleset::getLevel(int lines) const
{
	return (lines / 4) + 1;
}

qreal Ruleset::getSpeed(int level) const
{
	return 1.0f + ((level - 1) * 0.5f);
}

int Ruleset::getScoreForLines(int count, int level) const
{
	// 1:  100 =    0 + 100
	// 2:  300 =  100 + 200
	// 3:  500 =  300 + 200
	// 4:  800 =  500 + 300
	//
	// 5: 1100 =  800 + 300
	// 6: 1400 = 1100 + 300
	// 7: 1800 = 1400 + 400
	// ...
	//
	// Using pattern 1, 2, 2, 3, 3, 3, 4, 4, 4, 4, ...

	int score;
	if ((count > 0) && (level > 0))
	{
		int d = ( 1 + sqrt( static_cast<qreal>(1 - (4 * -((count - 1) * 2))) ) ) / 2;

		score = getScoreForLines(count - 1, level)
			+ (d * 100 * level);
	}
	else
	{
		score = 0;
	}

	return score;
}

int Ruleset::getScoreForPush(int count, int level) const
{
	return 1 * count * level;
}

int Ruleset::getScoreForDrop(int count, int level) const
{
	return 2 * count * level;
}

qreal Ruleset::getMoveSpeed() const
{
	return 8.0f;
}

qreal Ruleset::getPushSpeedMultiplier() const
{
	return 10.0f;
}

qreal Ruleset::getDelayBeforeNext() const
{
	return 0.0f;
}

qreal Ruleset::getDelayBeforeCast() const
{
	return 1.0f;
}

qreal Ruleset::getDelayBeforeLock() const
{
	return 1.5f;
}

void Ruleset::populateSequence(QList<Ruleset::Piece>& sequence) const
{
	while (sequence.size() < 3)
	{
		sequence << generateSequence();
	}
}

QList<Ruleset::Piece> Ruleset::generateSequence() const
{
	QList<Piece> a;
	a << PIECE_I
		<< PIECE_O
		<< PIECE_T
		<< PIECE_S
		<< PIECE_Z
		<< PIECE_J
		<< PIECE_L;

	QList<Piece> b;
	while (a.size())
	{
		b << a.takeAt(qRound((qrand() / static_cast<qreal>(RAND_MAX))
			* (a.size() - 1)));
	}

	return b;
}
