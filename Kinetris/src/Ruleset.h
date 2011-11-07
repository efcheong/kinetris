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

#ifndef KINETRIS_RULESET_H
#define KINETRIS_RULESET_H

#include <QtCore/QtCore>

#include "Pair.h"

class Ruleset : public QObject
{
	Q_OBJECT

public:

	enum Piece
	{
		PIECE_NONE = 0,
		PIECE_I,
		PIECE_O,
		PIECE_T,
		PIECE_S,
		PIECE_Z,
		PIECE_J,
		PIECE_L
	};

	Ruleset();
	virtual ~Ruleset();

	int getRotation(Piece piece, int rotation) const;

	QVector<Pair> getRotationShape(Piece piece, int rotation) const;
	QVector<Pair> getRotationNudge(Piece piece, int rotation, int direction) const;
	Pair getStartPosition(Piece piece) const; // bottom-left

	Pair getAtticPosition() const; // bottom-left

	int getRows() const;
	int getCols() const;

	int getLines(int level) const;
	int getLevel(int lines) const;
	qreal getSpeed(int level) const; // rows/sec

	int getScoreForLines(int count, int level) const;
	int getScoreForPush(int count, int level) const;
	int getScoreForDrop(int count, int level) const;

	qreal getMoveSpeed() const; // cols/sec
	qreal getPushSpeedMultiplier() const;

	qreal getDelayBeforeNext() const; // sec
	qreal getDelayBeforeCast() const; // sec
	qreal getDelayBeforeLock() const; // sec

	void populateSequence(QList<Piece>& sequence) const;

protected:

	static const int PIECE_TOTAL = 7;
	static const int SHAPE_TOTAL = 4;
	static const int BLOCK_TOTAL = 4;
	static const int TURNDIRECTION_TOTAL = 2;
	static const int NUDGE_TOTAL = 5;

	static const Pair ROTATION_SHAPE[PIECE_TOTAL * SHAPE_TOTAL * BLOCK_TOTAL];
	static const Pair ROTATION_NUDGE[PIECE_TOTAL * SHAPE_TOTAL * TURNDIRECTION_TOTAL * NUDGE_TOTAL];
	static const Pair POSITION_START[PIECE_TOTAL];

	QVector<Pair> _rotationShape;
	QVector<Pair> _rotationNudge;

	QList<Piece> generateSequence() const;
};

#endif // KINETRIS_RULESET_H
