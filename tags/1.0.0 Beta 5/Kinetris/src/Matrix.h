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

#ifndef KINETRIS_MATRIX_H
#define KINETRIS_MATRIX_H

#include <QtCore/QtCore>

#include "Pair.h"
#include "Ruleset.h"

class Tetromino;

class Matrix : public QObject
{
	Q_OBJECT

signals:

	void evTetromino(Tetromino* tetromino);
	void evTetrominoCast(Tetromino* tetromino);
	void evTetrominoMove(Tetromino* tetromino, int count);
	void evTetrominoMoveFail(Tetromino* tetromino);
	void evTetrominoTurn(Tetromino* tetromino, int count);
	void evTetrominoTurnFail(Tetromino* tetromino);
	void evTetrominoFall(Tetromino* tetromino, int count);
	void evTetrominoDrop(Tetromino* tetromino, int count);
	void evTetrominoLand(Tetromino* tetromino);
	void evTetrominoLock(Tetromino* tetromino);
	void evTetrominoHold(Tetromino* tetromino);
	void evTetrominoHoldFail(Tetromino* tetromino);

	void evGhost(Tetromino* ghost);
	void evGhostMove(Tetromino* ghost);
	void evGhostTurn(Tetromino* ghost);

	void evLines(QVector<bool> which);
	void evLevel(int count);
	void evScore(int count);
	void evTopOut();

public:

	enum State
	{
		STATE_NONE = 0,
		STATE_NEXT,
		STATE_CAST,
		STATE_HOLD,
		STATE_FALL,
		STATE_LAND,
		STATE_LOCK,
		STATE_LINE,
		STATE_OVER
	};

	Matrix(QObject* parent);
	virtual ~Matrix();

	State getState() const;
	void setState(State state, bool force = false);

	Ruleset* getRules() const;

	int getRows() const;
	int getCols() const;
	QVector<Ruleset::Piece> getField() const;

	int getLines() const;
	int getLevel() const;
	int getScore() const;

	QQueue<Ruleset::Piece> getNext() const;
	Ruleset::Piece getHold() const;

	Tetromino* getTetromino() const;
	Tetromino* getGhost() const;

	bool getPush() const;
	void setPush(bool a);

	bool occupied(Pair space) const;

	virtual void move(int direction);
	virtual void turn(int direction);
	virtual void drop();
	virtual void hold();

	virtual void update(qreal dt);

protected:

	State _state;
	State _s1;

	Ruleset* _rules;

	int _rows;
	int _cols;
	QVector<Ruleset::Piece> _field;

	int _score;
	int _lines;
	int _level;

	qreal _speed; // rows/sec
	qreal _speedMultiplier;

	QQueue<Ruleset::Piece> _next;
	Ruleset::Piece _hold;
	bool _held;

	Tetromino* _tetromino;
	Tetromino* _ghost;

	qreal _timer; // ms
	qreal _nextTimer; // ms
	qreal _castTimer; // ms
	qreal _moveTimer; // ms
	qreal _fallTimer; // ms
	qreal _lockTimer; // ms

	void init();
	void initRules();
	void initField();
	void initStats();
	void initTetrominoes();
	void initState();
	void initTimer();

	virtual void setLines(int lines);
	virtual void setLevel(int level);
	virtual void setScore(int score);

	void next();
	void cast();
	void lock();

	void collapse(QVector<bool> which);
	void collapse(int start);

	bool checkLines(QVector<bool> which);
	bool checkLevel();
	void awardScore(int count);

	bool overlapped();
	bool overflowed();

	void onStateEnter(State state);
	void onStateLeave(State state);

protected slots:

	void onMove(int count);
	void onMoveFail();
	void onTurn(int count);
	void onTurnFail();
	void onFall(int count);
	void onDrop(int count);
	void onLand();
	void onLock();
};

#endif // KINETRIS_MATRIX_H
