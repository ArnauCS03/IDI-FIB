TEMPLATE    = app
QT         += opengl 

INCLUDEPATH +=  /user/include/glm

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h

SOURCES += main.cpp \
        MyForm.cpp MyGLWidget.cpp
