TEMPLATE    = app
QT         += opengl 

INCLUDEPATH +=  /usr/include/glm  \
                ./Model 

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h

SOURCES += main.cpp MyForm.cpp \
           MyGLWidget.cpp  \
           ./Model/model.cpp

RESOURCES += shaders.qrc

