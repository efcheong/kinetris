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

#include "VisualMatrix.h"

#include "Game.h"
#include "Tetromino.h"

#include "LoaderThread.h"

const char* VisualMatrix::IMAGE_FRAME_BG = ":/res/frame-bg.png";
const char* VisualMatrix::IMAGE_FRAME_MG = ":/res/frame-mg.png";
const char* VisualMatrix::IMAGE_FRAME_FG = ":/res/frame-fg.png";
const char* VisualMatrix::IMAGE_LINES_PROGRESS = ":/res/lines-progress.png";
const char* VisualMatrix::IMAGE_FAIL = ":/res/fail.png";

const char* VisualMatrix::IMAGE_BLOCK[] = {
	":/res/block-i.png",
	":/res/block-o.png",
	":/res/block-t.png",
	":/res/block-s.png",
	":/res/block-z.png",
	":/res/block-j.png",
	":/res/block-l.png"
};
const char* VisualMatrix::IMAGE_BLOCK_OTHER = ":/res/block-other.png";
const char* VisualMatrix::IMAGE_BLOCK_GHOST = ":/res/block-ghost.png";

const char* VisualMatrix::IMAGE_COUNT[] = {
	":/res/count-1.png",
	":/res/count-2.png",
	":/res/count-3.png",
};
const char* VisualMatrix::IMAGE_OVER = ":/res/over.png";

const qreal VisualMatrix::BLOCK_LARGE = 24.0f;
const qreal VisualMatrix::BLOCK_SMALL = 16.0f;

const qreal VisualMatrix::COUNTDOWN_DURATION = 3.0f; // sec

const qreal VisualMatrix::NEXTEFFECT_DURATION = 1.0f; // sec
const qreal VisualMatrix::LOCKEFFECT_DURATION = 0.5f; // sec
const qreal VisualMatrix::HOLDEFFECT_DURATION = 1.0f; // sec
const qreal VisualMatrix::OVEREFFECT_DURATION = 1.5f; // sec
const qreal VisualMatrix::HELPEFFECT_DURATION = 3.0f; // sec

const qreal VisualMatrix::LINES_DELTA = 1.0f; // lines/sec
const qreal VisualMatrix::SCORE_DELTA = 200.0f; // score/sec

const qreal VisualMatrix::EXPLODEEFFECT_DURATION = 0.5f; // sec
const qreal VisualMatrix::EXPLODEEFFECT_DURATION_STAGGER = 0.25f; // sec
const qreal VisualMatrix::CRUMBLEEFFECT_DURATION = 0.0625f; // sec

VisualMatrix::VisualMatrix(Game* parent)
	: Matrix(parent)
{
	init();
}

VisualMatrix::~VisualMatrix()
{
}

void VisualMatrix::init()
{
	initSprite();
	initEffect();
	initMatrix();
	initStats();
	initState();
}

void VisualMatrix::initSprite()
{
	_sprite = new QGraphicsWidget();

	QGraphicsItem* item;
	QGraphicsWidget* widget;
	QGraphicsRectItem* rect;
	QGraphicsColorizeEffect* effect;
	QGraphicsSimpleTextItem* text;
	QGraphicsProxyWidget* proxy;
	QLabel* label;
	QFont font;

	// Frame
	{
		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_FRAME_BG));
		item->setParentItem(_sprite);
		item->setPos(0.0f, 0.0f);
	}

	// Frame
	{
		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_FRAME_MG));
		item->setParentItem(_sprite);
		item->setPos(0.0f, 0.0f);
	}

	// Avatar
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		widget->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
		widget->setParentItem(_sprite);
		widget->setPos(170.0f, 50.0f);
		
		item = new QGraphicsPixmapItem();
		item->setParentItem(widget);
		item->setPos(((BLOCK_LARGE * 10) - (320.0f / 0.75f)) * 0.5f, (BLOCK_LARGE * 20) - 320.0f);
		
		_avatar = widget;
	}

	// Help
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		widget->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
		widget->setParentItem(_sprite);
		widget->setPos(170.0f, 50.0f);
		
		rect = new QGraphicsRectItem(0.0f, 0.0f, BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		rect->setBrush(QBrush(QColor::fromRgb(0x00, 0x00, 0x00, 0xC0)));
		rect->setParentItem(widget);
		
		font = QApplication::font();
		font.setFamily("Arial");
		font.setPixelSize(16);
		font.setWeight(QFont::Bold);
		font.setStretch(80);
		
		text = new QGraphicsSimpleTextItem(tr("Slide left and right: Move."));
		text->setBrush(QColor::fromRgb(0xC0, 0xC0, 0xC0));
		text->setFont(font);
		text->setParentItem(widget);
		text->setPos(((BLOCK_LARGE * 10) - text->boundingRect().width()) * 0.5f, 217.0f - 6.0f);
		
		text = new QGraphicsSimpleTextItem(tr("Swipe up: Hold.  Swipe down: Drop."));
		text->setBrush(QColor::fromRgb(0xC0, 0xC0, 0xC0));
		text->setFont(font);
		text->setParentItem(widget);
		text->setPos(((BLOCK_LARGE * 10) - text->boundingRect().width()) * 0.5f, 237.0f - 6.0f);
		
		text = new QGraphicsSimpleTextItem(tr("Pull back, and make circles: Rotate."));
		text->setBrush(QColor::fromRgb(0xC0, 0xC0, 0xC0));
		text->setFont(font);
		text->setParentItem(widget);
		text->setPos(((BLOCK_LARGE * 10) - text->boundingRect().width()) * 0.5f, 257.0f - 6.0f);

//		widget->setVisible(false);
		
		_sprite_help = widget;
	}

	// Lines
	{
		rect = new QGraphicsRectItem(120.0f - 1.0f, 220.0f - 1.0f, 8.0f + 2.0f, 144.0f + 2.0f);
		rect->setBrush(QBrush(LoaderThread::instance()->getCachedPixmap(IMAGE_LINES_PROGRESS)));
		rect->setParentItem(_sprite);

		_sprite_lines = rect;
	}

	// Frame
	{
		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_FRAME_FG));
		item->setParentItem(_sprite);
		item->setPos(0.0f, 0.0f);
	}

	// Field
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		widget->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
		widget->setParentItem(_sprite);
		widget->setPos(170.0f, 50.0f);
		
		_sprite_field = widget;

		_sprite_space.fill(NULL, _rows * _cols);
	}

	// Ghost
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 4, BLOCK_LARGE * 4);
		widget->setParentItem(_sprite_field);
		widget->setPos(BLOCK_LARGE * getShapePositionInField(18, 3));
		
		for (int i = 0; i < 4; ++i)
		{
			item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_BLOCK_GHOST));
			item->setParentItem(widget);
		}
	
		_sprite_ghost = widget;
	}

	// Tetromino
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 4, BLOCK_LARGE * 4);
		widget->setParentItem(_sprite_field);
		widget->setPos(BLOCK_LARGE * getShapePositionInField(18, 3));
		widget->setZValue(1.0f);
		
		effect = new QGraphicsColorizeEffect(widget);
		effect->setColor(QColor::fromRgb(0xFF, 0xFF, 0xFF));
		effect->setStrength(0.0f);
		widget->setGraphicsEffect(effect);
		
		for (int i = 0; i < 4; ++i)
		{
			item = new QGraphicsPixmapItem();
			item->setParentItem(widget);
		}
		
		_sprite_tetromino = widget;
	}

	// Hold
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 4, BLOCK_LARGE * 2);
		widget->setScale(BLOCK_SMALL / BLOCK_LARGE);
		widget->setParentItem(_sprite);
		widget->setPos(56.0f, 108.0f + BLOCK_SMALL);
		
		effect = new QGraphicsColorizeEffect(widget);
		effect->setColor(QColor::fromRgb(0xFF, 0xFF, 0xFF));
		effect->setStrength(0.0f);
		widget->setGraphicsEffect(effect);
		
		for (int i = 0; i < 4; ++i)
		{
			item = new QGraphicsPixmapItem();
			item->setParentItem(widget);
		}
		
		_sprite_hold = widget;
	}

	// Hold fail
	{
		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_FAIL));
		item->setScale(BLOCK_SMALL / BLOCK_LARGE);
		item->setParentItem(_sprite);
		item->setPos(56.0f + (BLOCK_SMALL * 3), 108.0f + (BLOCK_SMALL * 3));
		item->setVisible(false);
	
		_sprite_holdFail = item;
	}

	// Next
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 4, BLOCK_LARGE * 2);
		widget->setScale(BLOCK_SMALL / BLOCK_LARGE);
		widget->setParentItem(_sprite);
		widget->setPos(460.0f, 108.0f + BLOCK_SMALL);
		
		effect = new QGraphicsColorizeEffect(widget);
		effect->setColor(QColor::fromRgb(0xFF, 0xFF, 0xFF));
		effect->setStrength(0.0f);
		widget->setGraphicsEffect(effect);
		
		for (int i = 0; i < 4; ++i)
		{
			item = new QGraphicsPixmapItem();
			item->setParentItem(widget);
		}
	
		_sprite_next << widget;
	}

	// Next
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 4, BLOCK_LARGE * 2);
		widget->setScale(BLOCK_SMALL / BLOCK_LARGE);
		widget->setParentItem(_sprite);
		widget->setPos(460.0f, 228.0f + BLOCK_SMALL);
		
		effect = new QGraphicsColorizeEffect(widget);
		effect->setColor(QColor::fromRgb(0xFF, 0xFF, 0xFF));
		effect->setStrength(0.0f);
		widget->setGraphicsEffect(effect);
		
		for (int i = 0; i < 4; ++i)
		{
			item = new QGraphicsPixmapItem();
			item->setParentItem(widget);
		}
	
		_sprite_next << widget;
	}

	// Next
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 4, BLOCK_LARGE * 2);
		widget->setScale(BLOCK_SMALL / BLOCK_LARGE);
		widget->setParentItem(_sprite);
		widget->setPos(460.0f, 292.0f + BLOCK_SMALL);
		
		effect = new QGraphicsColorizeEffect(widget);
		effect->setColor(QColor::fromRgb(0xFF, 0xFF, 0xFF));
		effect->setStrength(0.0f);
		widget->setGraphicsEffect(effect);
		
		for (int i = 0; i < 4; ++i)
		{
			item = new QGraphicsPixmapItem();
			item->setParentItem(widget);
		}
		
		_sprite_next << widget;
	}

	// Font
	{
		font = QApplication::font();
		font.setCapitalization(QFont::AllUppercase);
	}

	// Hold
	{
		text = new QGraphicsSimpleTextItem(tr("Hold"));
		text->setBrush(QColor::fromRgb(0x00, 0x00, 0x00));
		text->setFont(font);
		text->setParentItem(_sprite);
		text->setPos(56.0f, 80.0f - 6.0f);
	}

	// Next
	{
		text = new QGraphicsSimpleTextItem(tr("Next"));
		text->setBrush(QColor::fromRgb(0x00, 0x00, 0x00));
		text->setFont(font);
		text->setParentItem(_sprite);
		text->setPos(526.0f - text->boundingRect().width(), 80.0f - 6.0f);
	}

	// Level
	{
		text = new QGraphicsSimpleTextItem(tr("Level"));
		text->setBrush(QColor::fromRgb(0x00, 0x00, 0x00));
		text->setFont(font);
		text->setParentItem(_sprite);
		text->setPos(56.0f, 316.0f - 6.0f);
	}

	// Level
	{
		label = new QLabel();
		label->resize(48, 24);
		label->setStyleSheet("background-color: transparent; color: #FFFFFF;");
		label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
		
		proxy = new QGraphicsProxyWidget();
		proxy->setParentItem(_sprite);
		proxy->setWidget(label);
		proxy->setPos(56.0f, 344.0f - 6.0f);
	
		_sprite_level = label;
	}

	// Score
	{
		text = new QGraphicsSimpleTextItem(tr("Score"));
		text->setBrush(QColor::fromRgb(0x00, 0x00, 0x00));
		text->setFont(font);
		text->setParentItem(_sprite);
		text->setPos(176.0f, 564.0f - 6.0f);
	}

	// Score
	{
		label = new QLabel();
		label->resize(148, 24);
		label->setStyleSheet("background-color: transparent; color: #FFFFFF;");
		label->setAlignment(Qt::AlignTop | Qt::AlignRight);
		
		proxy = new QGraphicsProxyWidget();
		proxy->setParentItem(_sprite);
		proxy->setWidget(label);
		proxy->setPos(256.0f, 564.0f - 6.0f);
		
		_sprite_score = label;
	}

	// Countdown
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		widget->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
		widget->setParentItem(_sprite);
		widget->setPos(170.0f, 50.0f);

		rect = new QGraphicsRectItem(0.0f, 0.0f, BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		rect->setBrush(QBrush(QColor::fromRgb(0x00, 0x00, 0x00, 0xC0)));
		rect->setParentItem(widget);

		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_COUNT[0]));
		item->setParentItem(widget);
		item->setPos(77.0f, 197.0f);
		item->setVisible(false);
		_sprite_count << item;

		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_COUNT[1]));
		item->setParentItem(widget);
		item->setPos(77.0f, 197.0f);
		item->setVisible(false);
		_sprite_count << item;

		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_COUNT[2]));
		item->setParentItem(widget);
		item->setPos(77.0f, 197.0f);
		item->setVisible(false);
		_sprite_count << item;

		widget->setVisible(false);
		
		_sprite_countdown = widget;
	}

	// Game over
	{
		widget = new QGraphicsWidget();
		widget->resize(BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		widget->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
		widget->setParentItem(_sprite);
		widget->setPos(170.0f, 50.0f);
		widget->setZValue(8.0f);
		
		rect = new QGraphicsRectItem(0.0f, 0.0f, BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		rect->setBrush(QBrush(QColor::fromRgb(0x00, 0x00, 0x00, 0xC0)));
		rect->setParentItem(widget);
		
		item = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_OVER));
		item->setParentItem(widget);
		item->setPos(4.0f, 172.0f);
		
		rect = new QGraphicsRectItem(0.0f, 0.0f, BLOCK_LARGE * 10, BLOCK_LARGE * 20);
		rect->setBrush(QBrush(QColor::fromRgb(0xFF, 0xFF, 0xFF)));
		rect->setParentItem(widget);
		_sprite_overFlash = rect;
		
		widget->setVisible(false);
		
		_sprite_over = widget;
	}
}

void VisualMatrix::initEffect()
{
	_nextEffectTimer << new QTimeLine(1, this);
	_nextEffectTimer << new QTimeLine(1, this);
	_nextEffectTimer << new QTimeLine(1, this);
	_landEffectTimer = new QTimeLine(1, this);
	_lockEffectTimer = new QTimeLine(1, this);
	_holdEffectTimer = new QTimeLine(1, this);
	_overEffectTimer = new QTimeLine(1, this);
	_helpEffectTimer = new QTimeLine(1, this);
	_countdownTimer = new QTimeLine(1, this);

	_linesSpinner = 0.0f;
	_scoreSpinner = 0.0f;

	_collapseWhich.reserve(_rows);
	_collapseIndex = 0;
	_collapseTimer.reserve(4);
	_collapseTimer << new QTimeLine(1, this);
	_collapseTimer << new QTimeLine(1, this);
	_collapseTimer << new QTimeLine(1, this);
	_collapseTimer << new QTimeLine(1, this);
	_collapseGroup.reserve(4);
	_collapseGroup << new QGraphicsItemGroup(_sprite_field);
	_collapseGroup << new QGraphicsItemGroup(_sprite_field);
	_collapseGroup << new QGraphicsItemGroup(_sprite_field);
	_collapseGroup << new QGraphicsItemGroup(_sprite_field);
}

void VisualMatrix::initMatrix()
{
	QObject::connect(this, SIGNAL(evTetromino(Tetromino*)), this, SLOT(onTetromino(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetromino(Tetromino*)), this, SLOT(onTetrominoNext(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetrominoCast(Tetromino*)), this, SLOT(onTetrominoCast(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetrominoMove(Tetromino*, int)), this, SLOT(onTetrominoMove(Tetromino*, int)));
	QObject::connect(this, SIGNAL(evTetrominoMoveFail(Tetromino*)), this, SLOT(onTetrominoMoveFail(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetrominoTurn(Tetromino*, int)), this, SLOT(onTetrominoTurn(Tetromino*, int)));
	QObject::connect(this, SIGNAL(evTetrominoTurnFail(Tetromino*)), this, SLOT(onTetrominoTurnFail(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetrominoFall(Tetromino*, int)), this, SLOT(onTetrominoFall(Tetromino*, int)));
	QObject::connect(this, SIGNAL(evTetrominoDrop(Tetromino*, int)), this, SLOT(onTetrominoDrop(Tetromino*, int)));
	QObject::connect(this, SIGNAL(evTetrominoLand(Tetromino*)), this, SLOT(onTetrominoLand(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetrominoLock(Tetromino*)), this, SLOT(onTetrominoLock(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetrominoHold(Tetromino*)), this, SLOT(onTetrominoHold(Tetromino*)));
	QObject::connect(this, SIGNAL(evTetrominoHoldFail(Tetromino*)), this, SLOT(onTetrominoHoldFail(Tetromino*)));

	QObject::connect(this, SIGNAL(evGhost(Tetromino*)), this, SLOT(onGhost(Tetromino*)));
	QObject::connect(this, SIGNAL(evGhostMove(Tetromino*)), this, SLOT(onGhostMove(Tetromino*)));
	QObject::connect(this, SIGNAL(evGhostTurn(Tetromino*)), this, SLOT(onGhostTurn(Tetromino*)));

	QObject::connect(this, SIGNAL(evLines(QVector<bool>)), this, SLOT(onLines(QVector<bool>)));
	QObject::connect(this, SIGNAL(evLevel(int)), this, SLOT(onLevel(int)));
	QObject::connect(this, SIGNAL(evScore(int)), this, SLOT(onScore(int)));
	QObject::connect(this, SIGNAL(evTopOut()), this, SLOT(onTopOut()));

	QObject::connect(this, SIGNAL(evTetrominoLock(Tetromino*)), this, SLOT(onFirstLock(Tetromino*)));
}

void VisualMatrix::initStats()
{
	setLines();
	setLevel();
	setScore();
}

void VisualMatrix::initState()
{
	_state = STATE_NONE;
	_s1 = STATE_NONE;
}

VisualMatrix::State VisualMatrix::getState() const
{
	return _state;
}

QGraphicsWidget* VisualMatrix::getSprite() const
{
	return _sprite;
}

void VisualMatrix::setAvatar(QPixmap pixmap)
{
	static_cast<QGraphicsPixmapItem*>(_avatar->childItems().first())->setPixmap(pixmap);
	static_cast<QGraphicsPixmapItem*>(_avatar->childItems().first())->setScale(320.0f / pixmap.height());
}

void VisualMatrix::move(int direction)
{
	if (!(_state == STATE_PLAY))
		return;

	Matrix::move(direction);
}

void VisualMatrix::turn(int direction)
{
	if (!(_state == STATE_PLAY))
		return;

	Matrix::turn(direction);
}

void VisualMatrix::drop()
{
	if (!(_state == STATE_PLAY))
		return;

	Matrix::drop();
}

void VisualMatrix::hold()
{
	if (!(_state == STATE_PLAY))
		return;

	Matrix::hold();
}

void VisualMatrix::update(qreal dt)
{
	if (_state != _s1)
	{
		onStateLeave(_state);
		_state = _s1;
		onStateEnter(_state);
	}

	if (!_state)
	{
//		setState(STATE_COUNTDOWN);
		setState(STATE_PLAY);
	}
	else if (_state == STATE_COUNTDOWN)
	{
		updateCountdown(dt);
	}
	else if (_state == STATE_PLAY)
	{
		updateNextEffect(dt);
		updateLandEffect(dt);
		updateHoldEffect(dt);
		updateHelpEffect(dt);
		updateLines(dt);
		updateScore(dt);

		Matrix::update(dt);
	}
	else if (_state == STATE_LOCK)
	{
		updateNextEffect(dt);
		updateLandEffect(dt);
		updateHoldEffect(dt);
		updateHelpEffect(dt);
		updateLines(dt);
		updateScore(dt);
		updateLockEffect(dt);
	}
	else if (_state == STATE_LINE_EXPLODE)
	{
		updateNextEffect(dt);
		updateLandEffect(dt);
		updateHoldEffect(dt);
		updateHelpEffect(dt);
		updateLines(dt);
		updateScore(dt);
		updateExplodeEffect(dt);
	}
	else if (_state == STATE_LINE_CRUMBLE)
	{
		updateNextEffect(dt);
		updateLandEffect(dt);
		updateHoldEffect(dt);
		updateHelpEffect(dt);
		updateLines(dt);
		updateScore(dt);
		updateCrumbleEffect(dt);
	}
	else if (_state == STATE_OVER)
	{
		updateOverEffect(dt);
	}
}

void VisualMatrix::setState(State state, bool force)
{
	_s1 = state;

	if (force)
	{
		onStateLeave(_state);
		_state = _s1;
		onStateEnter(_state);
	}
}

void VisualMatrix::setLines(int lines)
{
	Matrix::setLines(lines);
}

void VisualMatrix::setLines()
{
	int i0 = _rules->getLines(_rules->getLevel(_linesSpinner));
	int i1 = _rules->getLines(_rules->getLevel(_linesSpinner) + 1);
	qreal progress = qBound<qreal>(0.0f, static_cast<qreal>(_linesSpinner - i0) / static_cast<qreal>(i1 - i0), 1.0f);

	QRectF rect = _sprite_lines->rect();
	rect.setTop((220.0f - 1.0f) + ((1.0f - progress) * (144.0f + 2.0f)));

	_sprite_lines->setRect(rect);

	setLevel();
}

void VisualMatrix::setLevel(int level)
{
	Matrix::setLevel(level);

	_sprite_level->setText(QString::number(level));
}

void VisualMatrix::setLevel()
{
	_sprite_level->setText(QString::number(_rules->getLevel(_linesSpinner)));
}

void VisualMatrix::setScore(int score)
{
	Matrix::setScore(score);
}

void VisualMatrix::setScore()
{
	QLocale locale(QLocale::English);

	_sprite_score->setText(locale.toString(static_cast<int>(_scoreSpinner)).replace(",", " "));
}

QPointF VisualMatrix::getBlockPositionInShape(int row, int col) const
{
	return QPointF(col, 4 - row - 1);
}

QPointF VisualMatrix::getBlockPositionInShape(Pair space) const
{
	return getBlockPositionInShape(space.row, space.col);
}

QPointF VisualMatrix::getBlockPositionInField(int row, int col) const
{
	return QPointF(col, _rules->getAtticPosition().row - row - 1);
}

QPointF VisualMatrix::getBlockPositionInField(Pair space) const
{
	return getBlockPositionInField(space.row, space.col);
}

QPointF VisualMatrix::getShapePositionInField(int row, int col) const
{
	return QPointF(col, _rules->getAtticPosition().row - row - 4);
}

QPointF VisualMatrix::getShapePositionInField(Pair space) const
{
	return getShapePositionInField(space.row, space.col);
}

void VisualMatrix::collapse(QVector<bool> which)
{
	for (int row = _rows - 1; row >= 0; --row)
	{
		if (which[row])
		{
			collapse(row);
		}
	}
}

void VisualMatrix::collapse(int start)
{
	if (start < _rows - 1)
	{
		memmove(&_sprite_space[start * _cols], &_sprite_space[(start + 1) * _cols],
			sizeof(QGraphicsItem*) * (_rows - (start + 1)) * _cols);

		memset(&_sprite_space[(_rows - 1) * _cols], NULL,
			sizeof(QGraphicsItem*) * _cols);
	}
	else
	{
		memset(&_sprite_space[(_rows - 1) * _cols], NULL,
			sizeof(QGraphicsItem*) * _cols);
	}
}

void VisualMatrix::updateCountdown(qreal dt)
{
	if (_countdownTimer->state() == QTimeLine::NotRunning)
		return;

	_countdownTimer->setCurrentTime(_countdownTimer->currentTime() + dt);
	
	int count = qCeil((_countdownTimer->duration() - _countdownTimer->currentTime()) / 1000.0f);
	for (int i = 0, il = _sprite_count.size(); i < il; ++i)
	{
		_sprite_count[i]->setVisible((i + 1 == count));
	}

	if (_countdownTimer->currentTime() >= _countdownTimer->duration())
	{
		_countdownTimer->stop();

		_sprite_countdown->setVisible(false);

		setState(STATE_PLAY);
	}
}

void VisualMatrix::updateNextEffect(qreal dt)
{
	for (int i = 0, il = _nextEffectTimer.count(); i < il; ++i)
	{
		if (_nextEffectTimer[i]->state() == QTimeLine::NotRunning)
			continue;

		_nextEffectTimer[i]->setCurrentTime(_nextEffectTimer[i]->currentTime() + dt);
			
		static_cast<QGraphicsColorizeEffect*>(_sprite_next[i]->graphicsEffect())->setStrength(1.0f - _nextEffectTimer[i]->currentValue());

		if (_nextEffectTimer[i]->currentTime() >= _nextEffectTimer[i]->duration())
			_nextEffectTimer[i]->stop();
	}
}

void VisualMatrix::updateLandEffect(qreal dt)
{
	// Prevent "unreferenced formal parameter" warning
	dt;

	if (_landEffectTimer->state() == QTimeLine::NotRunning)
		return;

	if (Matrix::_state != Matrix::STATE_LAND)
		return;

	_landEffectTimer->setCurrentTime(_lockTimer);

	if (_landEffectTimer->currentTime() >= _landEffectTimer->duration())
	{
		_landEffectTimer->stop();
		_landEffectTimer->setCurrentTime(0);
	}
			
	static_cast<QGraphicsColorizeEffect*>(_sprite_tetromino->graphicsEffect())->setStrength(_landEffectTimer->currentValue());
}

void VisualMatrix::updateLockEffect(qreal dt)
{
	if (_lockEffectTimer->state() == QTimeLine::NotRunning)
		return;

	_lockEffectTimer->setCurrentTime(_lockEffectTimer->currentTime() + dt);
		
	static_cast<QGraphicsColorizeEffect*>(_sprite_tetromino->graphicsEffect())->setStrength(1.0f - _lockEffectTimer->currentValue());

	if (_lockEffectTimer->currentTime() >= _lockEffectTimer->duration())
	{
		_lockEffectTimer->stop();

		_sprite_tetromino->setVisible(false);
		_sprite_ghost->setVisible(false);

		setState(STATE_PLAY);
	}
}

void VisualMatrix::updateHoldEffect(qreal dt)
{
	if (_holdEffectTimer->state() == QTimeLine::NotRunning)
		return;

	_holdEffectTimer->setCurrentTime(_holdEffectTimer->currentTime() + dt);
		
	static_cast<QGraphicsColorizeEffect*>(_sprite_hold->graphicsEffect())->setStrength(1.0f - _holdEffectTimer->currentValue());

	if (_holdEffectTimer->currentTime() >= _holdEffectTimer->duration())
		_holdEffectTimer->stop();
}

void VisualMatrix::updateOverEffect(qreal dt)
{
	if (_overEffectTimer->state() == QTimeLine::NotRunning)
		return;

	_overEffectTimer->setCurrentTime(_overEffectTimer->currentTime() + dt);
		
	_sprite_overFlash->setOpacity(1.0f - _overEffectTimer->currentValue());

	if (_overEffectTimer->currentTime() >= _overEffectTimer->duration())
		_overEffectTimer->stop();
}

void VisualMatrix::updateHelpEffect(qreal dt)
{
	if (_helpEffectTimer->state() == QTimeLine::NotRunning)
		return;

	_helpEffectTimer->setCurrentTime(_helpEffectTimer->currentTime() + dt);

	_sprite_help->setOpacity(1.0f - _helpEffectTimer->currentValue());

	if (_helpEffectTimer->currentTime() >= _helpEffectTimer->duration())
	{
		_helpEffectTimer->stop();

		_sprite_help->setVisible(false);
	}
}

void VisualMatrix::updateLines(qreal dt)
{
	if (_lines == _linesSpinner)
		return;

	qreal min = qMin(qAbs(_lines - _linesSpinner), (LINES_DELTA * 0.001f) * dt);
	int dir = (_lines < _linesSpinner) ? -1 : 1;
		
	_linesSpinner += dir * min;
	setLines();
}

void VisualMatrix::updateScore(qreal dt)
{
	if (_score == _scoreSpinner)
		return;

	qreal min = qMin(qAbs(_score - _scoreSpinner), (SCORE_DELTA * 0.001f) * dt);
	int dir = (_score < _scoreSpinner) ? -1 : 1;
		
	_scoreSpinner += dir * min;
	setScore();
}

void VisualMatrix::updateExplodeEffect(qreal dt)
{
	int count = 0;
	
	const qreal threshold = EXPLODEEFFECT_DURATION_STAGGER * 1000.0f;
	qreal t0;
	
	QTimeLine* timer;
	QGraphicsItemGroup* group;
	for (int i = 0, il = _collapseGroup.count(); i < il; ++i)
	{
		timer = _collapseTimer[i];
		group = _collapseGroup[i];

		if ((!group->childItems().count())
			|| (timer->currentTime() >= timer->duration()))
		{
			++count;
			continue;
		}

		if (timer->state() == QTimeLine::NotRunning)
			continue;

		t0 = timer->currentTime();
		timer->setCurrentTime(timer->currentTime() + dt);

		group->setScale(1.0f + (1.0f * timer->currentValue()));
		group->setOpacity(1.0f - (1.0f * timer->currentValue()));

		if (timer == _collapseTimer.last())
			continue;

		if ((t0 < threshold) && (timer->currentTime() >= threshold))
		{
			_collapseTimer[i + 1]->start();
			_collapseTimer[i + 1]->setPaused(true);
			_collapseTimer[i + 1]->setCurrentTime(timer->currentTime() - threshold);
		}
	}

	if (count >= _collapseGroup.count())
	{
		setState(STATE_LINE_CRUMBLE);
	}
}

void VisualMatrix::updateCrumbleEffect(qreal dt)
{
	int count = 0;

	QTimeLine* timer;
	QGraphicsItemGroup* group;
	for (int i = 0, il = _collapseGroup.count(); i < il; ++i)
	{
		timer = _collapseTimer[i];
		group = _collapseGroup[i];

		if ((!group->childItems().count())
			|| (timer->currentTime() >= timer->duration()))
		{
			++count;
			continue;
		}

		timer->setCurrentTime(timer->currentTime() + dt);

		group->setPos(0.0f, ((i + 1) * BLOCK_LARGE) * timer->currentValue());
	}

	if (count >= _collapseGroup.count())
	{
		collapse(_collapseWhich);
		setState(STATE_PLAY);
	}
}

void VisualMatrix::initExplodeEffect()
{
	QTimeLine* timer;
	QGraphicsItemGroup* group;
	int i = 0;
	for (int row = 0; row < _rows; ++row)
	{
		if (_collapseWhich[row])
		{
			timer = _collapseTimer[i];
			group = _collapseGroup[i];

			timer->setDuration(EXPLODEEFFECT_DURATION * 1000.0f);
			timer->setCurveShape(QTimeLine::EaseOutCurve);
			if (timer == _collapseTimer.first())
			{
				timer->start();
				timer->setPaused(true);
			}
			else
			{
				timer->setCurrentTime(0);
			}

			group->setParentItem(_sprite);
			group->setOpacity(1.0f);
			group->setScale(1.0f);
			group->setPos(_sprite_field->pos() + (BLOCK_LARGE * getBlockPositionInField(row, 0)));
			group->setTransformOriginPoint(BLOCK_LARGE * (_cols * 0.5f), BLOCK_LARGE * 0.5f);
			group->setZValue(1.0f);
			for (int col = 0; col < _cols; ++col)
			{
				group->addToGroup(_sprite_space[(row * _cols) + col]);
			}

			++i;
		}
	}
}

void VisualMatrix::initCrumbleEffect()
{
	QTimeLine* timer;
	QGraphicsItemGroup* group;
	int i = 0;
	for (int row = 0; row < _rows; ++row)
	{
		if (_collapseWhich[row])
		{
			timer = _collapseTimer[i];
			group = _collapseGroup[i];

			timer->setDuration((i + 1) * (CRUMBLEEFFECT_DURATION * 1000.0f));
			timer->setCurveShape(QTimeLine::LinearCurve);
			timer->start();
			timer->setPaused(true);

			group->setParentItem(_sprite);
			group->setOpacity(1.0f);
			group->setPos(0.0f, 0.0f);
			group->setZValue(1.0f);

			++i;
			continue;
		}

		if (i > 0)
		{
			for (int col = 0; col < _cols; ++col)
			{
				if (!_sprite_space[(row * _cols) + col])
					continue;

				group->addToGroup(_sprite_space[(row * _cols) + col]);
			}
		}
	}
}

void VisualMatrix::killExplodeEffect()
{
	QTimeLine* timer;
	QGraphicsItemGroup* group;
	for (int i = 0, il = _collapseGroup.count(); i < il; ++i)
	{
		timer = _collapseTimer[i];
		group = _collapseGroup[i];

		timer->stop();

		foreach (QGraphicsItem* g, group->childItems())
		{
			group->removeFromGroup(g);
			delete g;
		}
	}
}

void VisualMatrix::killCrumbleEffect()
{
	QTimeLine* timer;
	QGraphicsItemGroup* group;
	for (int i = 0, il = _collapseGroup.count(); i < il; ++i)
	{
		timer = _collapseTimer[i];
		group = _collapseGroup[i];

		timer->stop();

		foreach (QGraphicsItem* g, group->childItems())
		{
			group->removeFromGroup(g);
		}
	}
}

void VisualMatrix::onStateEnter(State state)
{
	if (!state)
	{
	}
	else if (state == STATE_COUNTDOWN)
	{
		_sprite_countdown->setVisible(true);

		_countdownTimer->setDuration(COUNTDOWN_DURATION * 1000.0f);
		_countdownTimer->start();
		_countdownTimer->setPaused(true);
	}
	else if (state == STATE_PLAY)
	{
	}
	else if (state == STATE_LOCK)
	{
	}
	else if (state == STATE_LINE_EXPLODE)
	{
		initExplodeEffect();
	}
	else if (state == STATE_LINE_CRUMBLE)
	{
		initCrumbleEffect();
	}
	else if (state == STATE_OVER)
	{
		_sprite_over->setVisible(true);

		_overEffectTimer->setCurveShape(QTimeLine::EaseInCurve);
		_overEffectTimer->setDuration(OVEREFFECT_DURATION * 1000.0f);
		_overEffectTimer->start();
		_overEffectTimer->setPaused(true);
	}
}

void VisualMatrix::onStateLeave(State state)
{
	if (!state)
	{
	}
	else if (state == STATE_COUNTDOWN)
	{
//		_sprite_help->setVisible(true);
	}
	else if (state == STATE_PLAY)
	{
	}
	else if (state == STATE_LOCK)
	{
	}
	else if (state == STATE_LINE_EXPLODE)
	{
		killExplodeEffect();
	}
	else if (state == STATE_LINE_CRUMBLE)
	{
		killCrumbleEffect();
	}
	else if (state == STATE_OVER)
	{
	}
}

void VisualMatrix::onTetromino(Tetromino* tetromino)
{
	Ruleset::Piece piece = tetromino->getPiece();
	QVector<Pair> shape = tetromino->getShape();
	Pair position = tetromino->getPosition();

	QList<QGraphicsItem*> g = _sprite_tetromino->childItems();
	for (int i = 0, il = shape.count(); i < il; ++i)
	{
		static_cast<QGraphicsPixmapItem*>(g[i])
			->setPixmap(LoaderThread::instance()->getCachedPixmap(IMAGE_BLOCK[piece - 1]));
		g[i]->setPos(BLOCK_LARGE * getBlockPositionInShape(shape[i]));
	}

	_sprite_tetromino->setPos(BLOCK_LARGE * getShapePositionInField(position));
	_sprite_tetromino->setVisible(true);
}

void VisualMatrix::onTetrominoNext(Tetromino* tetromino)
{
	// Prevent "unreferenced formal parameter" warning
	tetromino;

	QQueue<Ruleset::Piece> piece = _next;
	QVector<Pair> shape;
	for (int i = 0, il = _sprite_next.count(); i < il; ++i)
	{
		shape = _rules->getRotationShape(piece[i], 0);

		QList<QGraphicsItem*> g = _sprite_next[i]->childItems();
		for (int j = 0, jl = shape.count(); j < jl; ++j)
		{
			static_cast<QGraphicsPixmapItem*>(g[j])
				->setPixmap(LoaderThread::instance()->getCachedPixmap(IMAGE_BLOCK[piece[i] - 1]));
			g[j]->setPos(BLOCK_LARGE * getBlockPositionInShape(shape[j]));
		}

		_nextEffectTimer[i]->setCurveShape(QTimeLine::EaseInCurve);
		_nextEffectTimer[i]->setDuration(NEXTEFFECT_DURATION * 1000.0f);
		_nextEffectTimer[i]->start();
		_nextEffectTimer[i]->setPaused(true);
	}
}

void VisualMatrix::onTetrominoCast(Tetromino* tetromino)
{
	// Prevent "unreferenced formal parameter" warning
	tetromino;
}

void VisualMatrix::onTetrominoMove(Tetromino* tetromino, int count)
{
	// Prevent "unreferenced formal parameter" warning
	count;

	_sprite_tetromino->setPos(BLOCK_LARGE * getShapePositionInField(tetromino->getPosition()));
}

void VisualMatrix::onTetrominoMoveFail(Tetromino* tetromino)
{
	// Prevent "unreferenced formal parameter" warning
	tetromino;
}

void VisualMatrix::onTetrominoTurn(Tetromino* tetromino, int count)
{
	// Prevent "unreferenced formal parameter" warning
	count;

	QVector<Pair> shape = tetromino->getShape();
	Pair position = tetromino->getPosition();

	QList<QGraphicsItem*> g = _sprite_tetromino->childItems();
	for (int i = 0, il = shape.count(); i < il; ++i)
	{
		g[i]->setPos(BLOCK_LARGE * getBlockPositionInShape(shape[i]));
	}

	_sprite_tetromino->setPos(BLOCK_LARGE * getShapePositionInField(position));
}

void VisualMatrix::onTetrominoTurnFail(Tetromino* tetromino)
{
	// Prevent "unreferenced formal parameter" warning
	tetromino;
}

void VisualMatrix::onTetrominoFall(Tetromino* tetromino, int count)
{
	// Prevent "unreferenced formal parameter" warning
	count;

	_sprite_tetromino->setPos(BLOCK_LARGE * getShapePositionInField(tetromino->getPosition()));
}

void VisualMatrix::onTetrominoDrop(Tetromino* tetromino, int count)
{
	// Prevent "unreferenced formal parameter" warning
	count;

	_sprite_tetromino->setPos(BLOCK_LARGE * getShapePositionInField(tetromino->getPosition()));
}

void VisualMatrix::onTetrominoLand(Tetromino* tetromino)
{
	// Prevent "unreferenced formal parameter" warning
	tetromino;

	_landEffectTimer->setCurveShape(QTimeLine::EaseOutCurve);
	_landEffectTimer->setDuration(_rules->getDelayBeforeLock() * 1000.0f);
	_landEffectTimer->start();
	_landEffectTimer->setPaused(true);
}

void VisualMatrix::onTetrominoLock(Tetromino* tetromino)
{
	Ruleset::Piece piece = tetromino->getPiece();
	QVector<Pair> shape = tetromino->getShape();
	Pair position = tetromino->getPosition();

	QGraphicsPixmapItem* g;
	Pair p;
	foreach (const Pair& block, shape)
	{
		p.row = position.row + block.row;
		p.col = position.col + block.col;

		g = new QGraphicsPixmapItem(LoaderThread::instance()->getCachedPixmap(IMAGE_BLOCK[piece - 1]));
		g->setParentItem(_sprite_field);
		g->setPos(BLOCK_LARGE * getBlockPositionInField(p));
		_sprite_space[(p.row * _cols) + p.col] = g;
	}

//	_sprite_tetromino->setVisible(false);
//	_sprite_ghost->setVisible(false);
	
//	_sprite_holdFail->setVisible(false);

	_lockEffectTimer->setCurveShape(QTimeLine::EaseInCurve);
	_lockEffectTimer->setDuration(LOCKEFFECT_DURATION * 1000.0f);
	_lockEffectTimer->start();
	_lockEffectTimer->setPaused(true);

	setState(STATE_LOCK);
}

void VisualMatrix::onTetrominoHold(Tetromino* tetromino)
{
	Ruleset::Piece piece = tetromino->getPiece();
	QVector<Pair> shape = _rules->getRotationShape(piece, 0);
//	Pair position = tetromino->getPosition();

	QList<QGraphicsItem*> g = _sprite_hold->childItems();
	for (int i = 0, il = shape.count(); i < il; ++i)
	{
		static_cast<QGraphicsPixmapItem*>(g[i])
			->setPixmap(LoaderThread::instance()->getCachedPixmap(IMAGE_BLOCK[piece - 1]));
		g[i]->setPos(BLOCK_LARGE * getBlockPositionInShape(shape[i]));
	}
	
//	_sprite_holdFail->setVisible(true);

	_holdEffectTimer->setCurveShape(QTimeLine::EaseInCurve);
	_holdEffectTimer->setDuration(HOLDEFFECT_DURATION * 1000.0f);
	_holdEffectTimer->start();
	_holdEffectTimer->setPaused(true);

	if (_landEffectTimer->state() != QTimeLine::NotRunning)
	{
		_landEffectTimer->stop();
		_landEffectTimer->setCurrentTime(0);
		static_cast<QGraphicsColorizeEffect*>(_sprite_tetromino->graphicsEffect())->setStrength(_landEffectTimer->currentValue());
	}
}

void VisualMatrix::onTetrominoHoldFail(Tetromino* tetromino)
{
	// Prevent "unreferenced formal parameter" warning
	tetromino;
}

void VisualMatrix::onGhost(Tetromino* ghost)
{
	QVector<Pair> shape = ghost->getShape();
	Pair position = ghost->getPosition();

	QList<QGraphicsItem*> g = _sprite_ghost->childItems();
	for (int i = 0, il = shape.count(); i < il; ++i)
	{
		g[i]->setPos(BLOCK_LARGE * getBlockPositionInShape(shape[i]));
	}

	_sprite_ghost->setPos(BLOCK_LARGE * getShapePositionInField(position));
	_sprite_ghost->setVisible(true);
}

void VisualMatrix::onGhostMove(Tetromino* ghost)
{
	_sprite_ghost->setPos(BLOCK_LARGE * getShapePositionInField(ghost->getPosition()));
}

void VisualMatrix::onGhostTurn(Tetromino* ghost)
{
	QVector<Pair> shape = ghost->getShape();
	Pair position = ghost->getPosition();

	QList<QGraphicsItem*> g = _sprite_ghost->childItems();
	for (int i = 0, il = shape.count(); i < il; ++i)
	{
		g[i]->setPos(BLOCK_LARGE * getBlockPositionInShape(shape[i]));
	}

	_sprite_ghost->setPos(BLOCK_LARGE * getShapePositionInField(position));
}

void VisualMatrix::onLines(QVector<bool> which)
{
	_collapseWhich = which;

	setState(STATE_LINE_EXPLODE);
}

void VisualMatrix::onLevel(int count)
{
	// Prevent "unreferenced formal parameter" warning
	count;
}

void VisualMatrix::onScore(int count)
{
	// Prevent "unreferenced formal parameter" warning
	count;
}

void VisualMatrix::onTopOut()
{
	setState(STATE_OVER);
}

void VisualMatrix::onFirstLock(Tetromino* tetromino)
{
	// Prevent "unreferenced formal parameter" warning
	tetromino;

	QObject::disconnect(this, SIGNAL(evTetrominoLock(Tetromino*)), this, SLOT(onFirstLock(Tetromino*)));

	_helpEffectTimer->setCurveShape(QTimeLine::EaseInCurve);
	_helpEffectTimer->setDuration(HELPEFFECT_DURATION * 1000.0f);
	_helpEffectTimer->start();
	_helpEffectTimer->setPaused(true);
}
