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

#include "Matrix.h"

#include "Tetromino.h"

Matrix::Matrix(QObject* parent)
	: QObject(parent)
{
	init();
}

Matrix::~Matrix()
{
}

void Matrix::init()
{
	initRules();
	initField();
	initStats();
	initTetrominoes();
	initState();
	initTimer();
}

void Matrix::initRules()
{
	_rules = new Ruleset();
}

void Matrix::initField()
{
	_rows = _rules->getRows();
	_cols = _rules->getCols();
	_field.fill(Ruleset::PIECE_NONE, _rows * _cols);
}

void Matrix::initStats()
{
	_lines = 0;
	_level = _rules->getLevel(_lines);
	_score = 0;

	_speed = _rules->getSpeed(_level);
	_speedMultiplier = 1.0;
}

void Matrix::initTetrominoes()
{
	_rules->populateSequence(_next);
	_hold = Ruleset::PIECE_NONE;
	_held = false;

	_tetromino = NULL;
	_ghost = NULL;
}

void Matrix::initState()
{
	_state = STATE_NONE;
	_s1 = STATE_NONE;
}

void Matrix::initTimer()
{
	_timer = 0.0f;
	_nextTimer = 0.0f;
	_castTimer = 0.0f;
	_moveTimer = 0.0f;
	_fallTimer = 0.0f;
	_lockTimer = 0.0f;
}

Matrix::State Matrix::getState() const
{
	return _state;
}

void Matrix::setState(State state, bool force)
{
	_s1 = state;

	if (force)
	{
		onStateLeave(_state);
		_state = _s1;
		onStateEnter(_state);
	}
}

Ruleset* Matrix::getRules() const
{
	return _rules;
}

int Matrix::getRows() const
{
	return _rows;
}

int Matrix::getCols() const
{
	return _cols;
}

QVector<Ruleset::Piece> Matrix::getField() const
{
	return _field;
}

int Matrix::getLines() const
{
	return _lines;
}

int Matrix::getLevel() const
{
	return _level;
}

int Matrix::getScore() const
{
	return _score;
}

QQueue<Ruleset::Piece> Matrix::getNext() const
{
	return _next;
}

Ruleset::Piece Matrix::getHold() const
{
	return _hold;
}

Tetromino* Matrix::getTetromino() const
{
	return _tetromino;
}

Tetromino* Matrix::getGhost() const
{
	return _ghost;
}

bool Matrix::getPush() const
{
	return (_speedMultiplier != 1.0f);
}

void Matrix::setPush(bool a)
{
	_speedMultiplier = (a) ? _rules->getPushSpeedMultiplier() : 1.0f;
}

bool Matrix::occupied(Pair space) const
{
	return ((space.row < 0 || space.row >= _rows)
		|| (space.col < 0 || space.col >= _cols)
		|| (_field[(space.row * _cols) + space.col] != Ruleset::PIECE_NONE));
}

void Matrix::move(int direction)
{
	if (!((_state == STATE_FALL) || (_state == STATE_LAND)))
		return;

	if (_moveTimer >= 1000.0f / _rules->getMoveSpeed())
	{
		_moveTimer = 0.0f;
		_tetromino->move(direction);
	}
}

void Matrix::turn(int direction)
{
	if (!((_state == STATE_FALL) || (_state == STATE_LAND)))
		return;

	_tetromino->turn(direction);
}

void Matrix::drop()
{
	if (!((_state == STATE_FALL) || (_state == STATE_LAND)))
		return;

	_tetromino->drop();
}

void Matrix::hold()
{
	if (!((_state == STATE_FALL) || (_state == STATE_LAND)))
		return;

//	if (!_held)
//	{
		emit evTetrominoHold(_tetromino);

		Ruleset::Piece piece = _hold;
		_hold = _tetromino->getPiece();
		_held = true;

		if (piece)
			_next.push_front(piece);

		setState(STATE_NEXT);
//	}
//	else
//	{
//		emit evTetrominoHoldFail(_tetromino);
//	}
}

void Matrix::update(qreal dt)
{
	if (_state != _s1)
	{
		onStateLeave(_state);
		_state = _s1;
		onStateEnter(_state);
	}

	if (!_state)
	{
		setState(STATE_NEXT);
	}
	else if (_state == STATE_NEXT)
	{
		_timer += dt;
		_moveTimer += dt;

		_castTimer += dt;
		if (_castTimer >= _rules->getDelayBeforeCast() * 1000.0f)
		{
			cast();
		}
	}
	else if (_state == STATE_FALL)
	{
		_timer += dt;
		_moveTimer += dt;

		qreal interval = 1000.0f / (_speed * _speedMultiplier);

		_fallTimer += dt;
		if (_fallTimer >= interval)
		{
			int d = _fallTimer / interval;
			_fallTimer = _fallTimer - (d * interval);
			_tetromino->fall(d);
		}
	}
	else if (_state == STATE_LAND)
	{
		_timer += dt;
		_moveTimer += dt;
		
		_lockTimer += dt;
		if (_lockTimer >= _rules->getDelayBeforeLock() * 1000.0f)
		{
			_tetromino->lock();
		}
	}
	else if (_state == STATE_LOCK)
	{
		_timer += dt;
		_moveTimer += dt;

		_nextTimer += dt;
		if (_nextTimer >= _rules->getDelayBeforeNext() * 1000.0f)
		{
			setState(STATE_NEXT);
		}
	}
	else if (_state == STATE_LINE)
	{
		_timer += dt;
		_moveTimer += dt;

		_nextTimer += dt;
		if (_nextTimer >= _rules->getDelayBeforeNext() * 1000.0f)
		{
			setState(STATE_NEXT);
		}
	}
	else if (_state == STATE_OVER)
	{
	}
}

void Matrix::setLines(int lines)
{
	_lines = lines;
	checkLevel();
}

void Matrix::setLevel(int level)
{
	_level = level;
	_speed = _rules->getSpeed(_level);
}

void Matrix::setScore(int score)
{
	_score = score;
}

void Matrix::next()
{
	Ruleset::Piece piece = _next.dequeue();
	_rules->populateSequence(_next);

	Pair position = _rules->getStartPosition(piece);

	if (_tetromino)
		_tetromino->deleteLater();

	_tetromino = new Tetromino(piece, this);
	_tetromino->setPosition(position);
	QObject::connect(_tetromino, SIGNAL(evMove(int)), this, SLOT(onMove(int)));
	QObject::connect(_tetromino, SIGNAL(evMoveFail()), this, SLOT(onMoveFail()));
	QObject::connect(_tetromino, SIGNAL(evTurn(int)), this, SLOT(onTurn(int)));
	QObject::connect(_tetromino, SIGNAL(evTurnFail()), this, SLOT(onTurnFail()));
	QObject::connect(_tetromino, SIGNAL(evFall(int)), this, SLOT(onFall(int)));
	QObject::connect(_tetromino, SIGNAL(evDrop(int)), this, SLOT(onDrop(int)));
	QObject::connect(_tetromino, SIGNAL(evLand()), this, SLOT(onLand()));
	QObject::connect(_tetromino, SIGNAL(evLock()), this, SLOT(onLock()));

	if (_ghost)
		_ghost->deleteLater();

	_ghost = new Tetromino(piece, this);
	_ghost->setPosition(position);
	_ghost->drop();
	_ghost->setLocked(false);

	emit evTetromino(_tetromino);
	emit evGhost(_ghost);

	if (overlapped())
	{
		emit evTopOut();
		setState(STATE_OVER, true);
	}
}

void Matrix::cast()
{
	emit evTetrominoCast(_tetromino);
	setState(STATE_FALL);
}

void Matrix::lock()
{
	QVector<bool> which(_rows);

	Ruleset::Piece piece = _tetromino->getPiece();
	QVector<Pair> shape = _tetromino->getShape();
	Pair position = _tetromino->getPosition();

	Pair p;
	foreach (const Pair& block, shape)
	{
		p.row = position.row + block.row;
		p.col = position.col + block.col;
		_field[(p.row * _cols) + p.col] = piece;
		which[p.row] = true;
	}

	if (overflowed())
	{
		emit evTopOut();
		setState(STATE_OVER, true);
	}
	else if (checkLines(which))
	{
		setState(STATE_LINE, true);
	}
}

void Matrix::collapse(QVector<bool> which)
{
	for (int row = _rows - 1; row >= 0; --row)
	{
		if (which[row])
		{
			collapse(row);
		}
	}
}

void Matrix::collapse(int start)
{
	if (start < _rows - 1)
	{
		memmove(&_field[start * _cols], &_field[(start + 1) * _cols],
			sizeof(Ruleset::Piece) * (_rows - (start + 1)) * _cols);

		memset(&_field[(_rows - 1) * _cols], Ruleset::PIECE_NONE,
			sizeof(Ruleset::Piece) * _cols);
	}
	else
	{
		memset(&_field[(_rows - 1) * _cols], Ruleset::PIECE_NONE,
			sizeof(Ruleset::Piece) * _cols);
	}
}

bool Matrix::checkLines(QVector<bool> which)
{
	int count = 0;
	for (int row = 0; row < _rows; ++row)
	{
		if (which[row])
		{
			for (int col = 0; col < _cols; ++col)
			{
				if (_field[(row * _cols) + col] == Ruleset::PIECE_NONE)
				{
					// Don't have a full row
					which[row] = false;
					goto L0;
					// break;
				}
			}

			// No gaps
			++count;

			L0: ;
		}
	}

	if (count)
	{
		setLines(_lines + count);
		emit evLines(which);
		collapse(which);

		awardScore(_rules->getScoreForLines(count, _level));

		return true;
	}

	return false;
}

bool Matrix::checkLevel()
{
	int level = _rules->getLevel(_lines);
	int count = level - _level;

	if (count)
	{
		setLevel(_level + count);
		emit evLevel(count);

		return true;
	}

	return false;
}

void Matrix::awardScore(int count)
{
	setScore(_score + count);
	emit evScore(count);
}

bool Matrix::overlapped()
{
	QVector<Pair> shape = _tetromino->getShape();
	Pair position = _tetromino->getPosition();

	Pair p;
	foreach (const Pair& block, shape)
	{
		p.row = position.row + block.row;
		p.col = position.col + block.col;

		if (occupied(p))
		{
			// Block overlaps with another
			// We're dead
			return true;
		}
	}

	return false;
}

bool Matrix::overflowed()
{
	QVector<Pair> shape = _tetromino->getShape();
	Pair position = _tetromino->getPosition();

	Pair attic = _rules->getAtticPosition();

	Pair p;
	foreach (const Pair& block, shape)
	{
		p.row = position.row + block.row;
		p.col = position.col + block.col;

		if (p.row < attic.row)
		{
			// Not all blocks are in attic
			// We're okay
			return false;
		}
	}

	return true;
}

void Matrix::onStateEnter(State state)
{
	if (!state)
	{
	}
	else if (state == STATE_NEXT)
	{
		_castTimer = 0.0f;

		next();
	}
	else if (state == STATE_FALL)
	{
		_fallTimer = 0.0f;
	}
	else if (state == STATE_LAND)
	{
		_lockTimer = 0.0f;
	}
	else if (state == STATE_LOCK)
	{
		_nextTimer = 0.0f;

		_held = false;
		lock();
	}
	else if (state == STATE_LINE)
	{
		_nextTimer = 0.0f;
	}
	else if (state == STATE_OVER)
	{
	}
}

void Matrix::onStateLeave(State state)
{
}

void Matrix::onMove(int count)
{
	_ghost->setPosition(_tetromino->getPosition());
	_ghost->drop();
	_ghost->setLocked(false);

	emit evTetrominoMove(_tetromino, count);
	emit evGhostMove(_ghost);

	if (_state == STATE_LAND)
	{
		_lockTimer = 0.0f;

		Pair p0 = _tetromino->getPosition();
		Pair p1 = _ghost->getPosition();
		if (p1.row != p0.row)
		{
			setState(STATE_FALL);
		}
	}
}

void Matrix::onMoveFail()
{
	emit evTetrominoMoveFail(_tetromino);
}

void Matrix::onTurn(int count)
{
	_ghost->setRotation(_tetromino->getRotation());
	_ghost->setPosition(_tetromino->getPosition());
	_ghost->drop();
	_ghost->setLocked(false);

	emit evTetrominoTurn(_tetromino, count);
	emit evGhostTurn(_ghost);

	if (_state == STATE_LAND)
	{
		_lockTimer = 0.0f;

		Pair p0 = _tetromino->getPosition();
		Pair p1 = _ghost->getPosition();
		if (p1.row != p0.row)
		{
			setState(STATE_FALL);
		}
	}
}

void Matrix::onTurnFail()
{
	emit evTetrominoTurnFail(_tetromino);
}

void Matrix::onFall(int count)
{
	emit evTetrominoFall(_tetromino, count);

	if (getPush())
		awardScore(_rules->getScoreForPush(count, _level));
}

void Matrix::onDrop(int count)
{
	emit evTetrominoDrop(_tetromino, count);

	awardScore(_rules->getScoreForDrop(count, _level));
}

void Matrix::onLand()
{
	emit evTetrominoLand(_tetromino);
	setState(STATE_LAND);
}

void Matrix::onLock()
{
	emit evTetrominoLock(_tetromino);
	setState(STATE_LOCK);
}
