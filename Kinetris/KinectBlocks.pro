TEMPLATE = app

TARGET = "KinectBlocks"

CONFIG(debug, debug|release) {
	DESTDIR = "bin/debug"
	OBJECTS_DIR = "obj/debug"
	MOC_DIR = "obj/debug"
	RCC_DIR = "obj/debug"
	UI_HEADERS_DIR = "obj/debug"
	UI_SOURCES_DIR = "obj/debug"
}
else {
	DESTDIR = "bin/release"
	OBJECTS_DIR = "obj/release"
	MOC_DIR = "obj/release"
	RCC_DIR = "obj/release"
	UI_HEADERS_DIR = "obj/release"
	UI_SOURCES_DIR = "obj/release"
}

QT += core \
	gui \
	opengl

# Windows
win32 {
	INCLUDEPATH += "C:/SDK/OpenNI/1.0.0/Include" \
		"C:/SDK/NITE/1.3.0/Include"

	LIBS += -L"C:/SDK/OpenNI/1.0.0/Lib" \
		-l"openNI" \
		-L"C:/SDK/NITE/1.3.0/Lib" \
		-l"XnVNITE"
}
# Mac
macx {
	INCLUDEPATH += "/usr/include/ni" \
		"/usr/include/nite"

	LIBS += -L"/usr/lib" \
		-l"openNI" \
		-L"/usr/lib" \
		-l"XnVNITE"
}
# Linux
unix:!macx {
	INCLUDEPATH += "/usr/include/ni" \
		"/usr/include/nite"

	LIBS += -L"/usr/lib" \
		-l"openNI" \
		-L"/usr/lib" \
		-l"XnVNITE"
}

HEADERS += "src/Pair.h" \
	"src/Ruleset.h" \
	"src/Tetromino.h" \
	"src/Matrix.h" \
	"src/VisualMatrix.h" \
	"src/Player.h" \
	"src/InputManager.h" \
	"src/MenuScreen.h" \
	"src/PlayScreen.h" \
	"src/HomeScreen.h" \
	"src/Background.h" \
	"src/LoaderThread.h" \
	"src/SensorThread.h" \
	"src/Game.h" \
	"src/KinectBlocks.h"

SOURCES += "src/Pair.cpp" \
	"src/Ruleset.cpp" \
	"src/Tetromino.cpp" \
	"src/Matrix.cpp" \
	"src/VisualMatrix.cpp" \
	"src/Player.cpp" \
	"src/InputManager.cpp" \
	"src/MenuScreen.cpp" \
	"src/PlayScreen.cpp" \
	"src/HomeScreen.cpp" \
	"src/Background.cpp" \
	"src/LoaderThread.cpp" \
	"src/SensorThread.cpp" \
	"src/Game.cpp" \
	"src/KinectBlocks.cpp" \
	"src/main.cpp"

RESOURCES += "KinectBlocks.qrc"

# Windows
win32 {
	QMAKE_PRE_LINK = $$quote(copy /y \"OpenNI.xml\" \"$$replace(DESTDIR, "/", "\\")\\\" &)
}
# Mac
macx {
	QMAKE_PRE_LINK = $$quote(cp \"OpenNI.xml\" \"$${DESTDIR}/\";)
}
# Linux
unix:!macx {
	QMAKE_PRE_LINK = $$quote(cp \"OpenNI.xml\" \"$${DESTDIR}/\";)
}
