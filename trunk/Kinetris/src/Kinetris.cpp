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

#include "Kinetris.h"

#include "Game.h"

Kinetris::Kinetris()
    : QMainWindow()
{
	setWindowTitle(tr("Kinetris 1.0.0 Beta 5"));
	setWindowIcon(QIcon(":/res/appicon.png"));
	showFullScreen();

	_view = new QGraphicsView(NULL, this);
	_view->setFrameStyle(QFrame::NoFrame);
	_view->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
	_view->setBackgroundBrush(QBrush(QColor::fromRgb(0x00, 0x00, 0x00, 0xFF)));
	_view->setSceneRect(0.0f, 0.0f, 1280.0f, 720.0f);
	_view->setCursor(Qt::BlankCursor);
	setCentralWidget(_view);

	// Scale graphics based on screen height, to maintain aspect ratio
	qreal k = QApplication::desktop()->screenGeometry(this).height() / 720.0f;
	_view->scale(k, k);

	_view->setScene(new Game(this));
}

Kinetris::~Kinetris()
{
}

QGraphicsView* Kinetris::getView() const
{
	return _view;
}
