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

#ifndef KINETRIS_VISUALMATRIX_H
#define KINETRIS_VISUALMATRIX_H

#include <QtGui/QtGui>

#include "Matrix.h"

class Game;
class Tetromino;

class VisualMatrix : public Matrix
{
	Q_OBJECT

public:

	enum State
	{
		STATE_NONE = 0,
		STATE_PLAY,
		STATE_LOCK,
		STATE_LINE_EXPLODE,
		STATE_LINE_CRUMBLE,
		STATE_OVER
	};

	VisualMatrix(Game* parent);
	virtual ~VisualMatrix();

	State getState() const;

	QGraphicsWidget* getSprite() const;

	void setAvatar(QPixmap pixmap);

	virtual void move(int direction);
	virtual void turn(int direction);
	virtual void drop();
	virtual void hold();

	virtual void update(qreal dt);

protected:

	static const char IMAGE_FRAME_BG[];
	static const char IMAGE_FRAME_MG[];
	static const char IMAGE_FRAME_FG[];
	static const char IMAGE_LINES_PROGRESS[];
	static const char IMAGE_BLOCK[][32];
	static const char IMAGE_BLOCK_OTHER[];
	static const char IMAGE_BLOCK_GHOST[];
	static const char IMAGE_OVER[];

	static const char IMAGE_FAIL[];

	static const qreal BLOCK_LARGE;
	static const qreal BLOCK_SMALL;

	static const qreal NEXTEFFECT_DURATION; // sec
	static const qreal LOCKEFFECT_DURATION; // sec
	static const qreal HOLDEFFECT_DURATION; // sec
	static const qreal OVEREFFECT_DURATION; // sec

	static const qreal LINES_DELTA; // lines/sec
	static const qreal SCORE_DELTA; // score/sec

	static const qreal EXPLODEEFFECT_DURATION; // sec
	static const qreal EXPLODEEFFECT_DURATION_STAGGER; // sec
	static const qreal CRUMBLEEFFECT_DURATION; // sec

	State _state;
	State _s1;

	QGraphicsWidget* _sprite;
	QGraphicsWidget* _sprite_field;
	QVector<QGraphicsItem*> _sprite_space;
	QGraphicsWidget* _sprite_tetromino;
	QGraphicsWidget* _sprite_ghost;
	QGraphicsWidget* _sprite_hold;
	QGraphicsItem* _sprite_holdFail;
	QQueue<QGraphicsWidget*> _sprite_next;
	QGraphicsRectItem* _sprite_lines;
	QLabel* _sprite_level;
	QLabel* _sprite_score;
	QGraphicsWidget* _sprite_over;
	QGraphicsRectItem* _sprite_overRect;

	QGraphicsWidget* _avatar;

	QVector<QTimeLine*> _nextEffectTimer;
	QTimeLine* _landEffectTimer;
	QTimeLine* _lockEffectTimer;
	QTimeLine* _holdEffectTimer;
	QTimeLine* _overEffectTimer;

	qreal _linesSpinner;
	qreal _scoreSpinner;

	QVector<bool> _collapseWhich;
	int _collapseIndex;
	QVector<QTimeLine*> _collapseTimer;
	QVector<QGraphicsItemGroup*> _collapseGroup;
	
	void init();
	void initSprite();
	void initEffect();
	void initMatrix();
	void initStats();
	void initState();

	void setState(State state, bool force = false);

	virtual void setLines(int lines);
	virtual void setLines();
	virtual void setLevel(int level);
	virtual void setLevel();
	virtual void setScore(int score);
	virtual void setScore();

	QPointF getBlockPositionInShape(int row, int col) const;
	QPointF getBlockPositionInShape(Pair space) const;
	QPointF getBlockPositionInField(int row, int col) const;
	QPointF getBlockPositionInField(Pair space) const;
	QPointF getShapePositionInField(int row, int col) const;
	QPointF getShapePositionInField(Pair space) const;

	void collapse(QVector<bool> which);
	void collapse(int start);

	void updateNextEffect(qreal dt);
	void updateLandEffect(qreal dt);
	void updateLockEffect(qreal dt);
	void updateHoldEffect(qreal dt);
	void updateOverEffect(qreal dt);
	void updateLines(qreal dt);
	void updateScore(qreal dt);
	void updateExplodeEffect(qreal dt);
	void updateCrumbleEffect(qreal dt);

	void initExplodeEffect();
	void initCrumbleEffect();
	void killExplodeEffect();
	void killCrumbleEffect();

	void onStateEnter(State state);
	void onStateLeave(State state);

protected slots:

	void onTetromino(Tetromino* tetromino);
	void onTetrominoNext(Tetromino* tetromino);
	void onTetrominoCast(Tetromino* tetromino);
	void onTetrominoMove(Tetromino* tetromino, int count);
	void onTetrominoMoveFail(Tetromino* tetromino);
	void onTetrominoTurn(Tetromino* tetromino, int count);
	void onTetrominoTurnFail(Tetromino* tetromino);
	void onTetrominoFall(Tetromino* tetromino, int count);
	void onTetrominoDrop(Tetromino* tetromino, int count);
	void onTetrominoLand(Tetromino* tetromino);
	void onTetrominoLock(Tetromino* tetromino);
	void onTetrominoHold(Tetromino* tetromino);
	void onTetrominoHoldFail(Tetromino* tetromino);

	void onGhostMove(Tetromino* ghost);
	void onGhostTurn(Tetromino* ghost);
	void onGhost(Tetromino* ghost);

	void onLines(QVector<bool> which);
	void onLevel(int count);
	void onScore(int count);
	void onTopOut();
};

#endif // KINETRIS_VISUALMATRIX_H
