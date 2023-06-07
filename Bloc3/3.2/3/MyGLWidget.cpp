// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::initializeGL() {

  BL3GLWidget::initializeGL();

  posFocusLoc = glGetUniformLocation (program->programId(), "posFocus");
  colorFocusLoc = glGetUniformLocation (program->programId(), "colorFocusFS");
  llumAmbLoc = glGetUniformLocation (program->programId(), "llumAmbient");

  boolFocusEscenaLoc = glGetUniformLocation (program->programId(), "focusEscena");


  iniFocus();
}


void MyGLWidget::iniFocus() {

  focus_escena = true;
  
  posFocusEscena = glm::vec4(1., 1., 1., 1.); // sense view, la multipliquem al VS
  colorFocus = glm::vec3(0.8, 0.8, 0.8);
  llumAmbient = glm::vec3(0.2, 0.2, 0.2);

  posFocusCamera = glm::vec4(0., 0., 0., 1.);

  glUniform1i (boolFocusEscenaLoc, focus_escena); // abans d'enviar la posicio

  glUniform4fv (posFocusLoc, 1, &posFocusEscena[0]);
  glUniform3fv (colorFocusLoc, 1, &colorFocus[0]);
  glUniform3fv (llumAmbLoc, 1, &llumAmbient[0]);
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / ample;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();

  switch (event->key()) {

    case Qt::Key_F:

      if (focus_escena) {  // passem focus de camara
        focus_escena = false;
        
        glUniform1i (boolFocusEscenaLoc, focus_escena);
        glUniform4fv (posFocusLoc, 1, &posFocusCamera[0]);
      }
      else {      // passem focus d'escena
        focus_escena = true;

        glUniform1i (boolFocusEscenaLoc, focus_escena);
        glUniform4fv (posFocusLoc, 1, &posFocusEscena[0]);
      }
      break;

    case Qt::Key_K: // moure focus Escena esquerra
      if (focus_escena) {
        posFocusEscena = posFocusEscena + glm::vec4(-0.2, 0., 0., 0.);
        glUniform4fv(posFocusLoc, 1, &posFocusEscena[0]);
      }

      break;

    case Qt::Key_L: // moure focus Escena dreta
      if (focus_escena) {
        posFocusEscena = posFocusEscena + glm::vec4(0.2, 0., 0., 0.);
        glUniform4fv(posFocusLoc, 1, &posFocusEscena[0]);
      }

      break;

    default: BL3GLWidget::keyPressEvent(event); break;
  }
  update();
}

void MyGLWidget::iniMaterialTerra() {

  amb = glm::vec3(0,0,0.2);
  diff = glm::vec3(0,0,0.8);
  spec = glm::vec3(1,1,1);
  shin = 100;
}

