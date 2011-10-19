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

#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QtGui/QtGui>

class Game;

class HomeScreen : public QObject
{
	Q_OBJECT

public:

	HomeScreen(Game* parent);
	virtual ~HomeScreen();

	QGraphicsWidget* getSprite() const;

	QString getStatus() const;
	void setStatus(const QString& status);

	void setAvatar(QPixmap pixmap);

	void show();
	void hide();

	void update(qreal dt);

protected:

	static const char _IMAGE_TITLE[];

	static const qreal _BACKGROUND_W; // px
	static const qreal _BACKGROUND_H; // px

	static const qreal _SHOWEFFECT_DURATION; // sec
	static const qreal _HIDEEFFECT_DURATION; // sec

	static const qreal _GLOWEFFECT_DURATION; // sec

	QGraphicsWidget* _sprite;
	QGraphicsProxyWidget* _sprite_status;

	QLabel* _status;
	QString _s1;

	QGraphicsPixmapItem* _avatar;

	QTimeLine* _showEffectTimer;
	QTimeLine* _hideEffectTimer;

	qreal _glowTimer; // ms

	void init();
	void initSprite();
	void initEffect();
};

#endif // HOMESCREEN_H
