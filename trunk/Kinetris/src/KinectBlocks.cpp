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

#include "KinectBlocks.h"

#include "Game.h"

KinectBlocks::KinectBlocks()
    : QMainWindow()
{
	setWindowTitle(tr("Kinetris 1.0.0 Beta 1"));
	setFixedSize(1024, 640);
	move(0, 0);

	_view = new QGraphicsView(NULL, this);
	_view->setFrameStyle(QFrame::NoFrame);
	_view->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
	_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
	_view->setSceneRect(0.0f, 0.0f, width(), height());
	_view->setCursor(Qt::BlankCursor);
	setCentralWidget(_view);

	_view->setScene(Game::instance(this));
}

KinectBlocks::~KinectBlocks()
{
}

QGraphicsView* KinectBlocks::getView() const
{
	return _view;
}
