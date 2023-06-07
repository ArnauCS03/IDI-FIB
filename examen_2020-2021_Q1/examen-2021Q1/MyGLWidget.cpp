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

  ExamGLWidget::initializeGL();
  
  angleOrbitaCub1 = 0.0f;
  angleOrbitaCub2 = 2*M_PI/3; // 120 graus
  angleOrbitaCub3 = angleOrbitaCub2*2;

  escalaCub1 = 2.0f/0.5f; // escala que volem / la que tenim
  escalaCub2 = 2.5f/0.5f;
  escalaCub3 = 3.0f/0.5f;

  posPatricio = glm::vec3(5., 0., 0.);

  pintaPatricio = false;
  angleIniPatricioMirarEixY = -M_PI/2; // 90º
  anglePatricioMirarEixY = 0;

  colFocusCamLoc = glGetUniformLocation(program -> programId(), "colFocus");

  iniFocus();
  iniCamera2();
}


void MyGLWidget::iniFocus() {

  focus_blanc = true;
  colFocusCam = glm::vec3(1.0, 1.0, 1.0);

  glUniform3fv(colFocusCamLoc, 1, &colFocusCam[0]);
}


void MyGLWidget::iniCamera2() {

  camOrto.VRP = centreEsc; 
  camOrto.OBS = camOrto.VRP + 2*radiEsc * glm::vec3(0., 1., 0.);
  camOrto.UP = glm::vec3(1., 0., 0.);
  camOrto.left = -radiEsc;
  camOrto.right = radiEsc;
  camOrto.bottom = -radiEsc;
  camOrto.top = radiEsc;
  camOrto.zNear = 2*radiEsc - radiEsc;
  camOrto.zFar = 2*radiEsc + radiEsc;
}


void MyGLWidget::paintGL ()  
{
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Pintem el terra
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  if (pintaPatricio) {
    // Pintem el Patricio
    glBindVertexArray (VAO_Patr);
    modelTransformPatricio ();
    glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
  }
  
  if (not pintaPatricio) {
    // Pintem el cub 1
    glBindVertexArray(VAO_Cub);
    modelTransformCub (escalaCub1, angleOrbitaCub1);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Pintem cub 2
    modelTransformCub(escalaCub2, angleOrbitaCub2);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Pintem cub 3
    modelTransformCub (escalaCub3, angleOrbitaCub3);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
  
  glBindVertexArray(0); 
}


void MyGLWidget::resizeGL (int w, int h) {

#ifdef __APPLE__
  // Aquest codi és necessari únicament per a MACs amb pantalla retina.
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif

  ra = float(ample)/float(alt);

  if (camPlanta) {
    if (ra > 1) {
      camOrto.left = -radiEsc*ra;
      camOrto.right = radiEsc*ra;
    }
    else {
      camOrto.bottom = -radiEsc/ra;
      camOrto.top = radiEsc/ra;
    }
  }
  projectTransform();
}


void MyGLWidget::modelTransformCub (float escala, float angle) 
{
  TG = glm::mat4(1.f);
  TG = glm::rotate(TG, angle, glm::vec3(0, 1, 0));
  TG = glm::translate(TG, glm::vec3(5, 0, 0));
  TG = glm::scale(TG, glm::vec3(escala, escala, escala));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformPatricio ()  
{
  TG = glm::mat4(1.f);
  TG = glm::rotate(TG, anglePatricioMirarEixY, glm::vec3(0, 1, 0));
  TG = glm::translate(TG, posPatricio);
  TG = glm::rotate(TG, angleIniPatricioMirarEixY, glm::vec3(0, 1, 0));
  TG = glm::scale(TG, glm::vec3 (2*escala, 2*escala, 2*escala));
  TG = glm::translate(TG, -centreBasePat);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::viewTransform()  
{
  if (!camPlanta)
    ExamGLWidget::viewTransform();
  else
  {
    glm::mat4 View2 = glm::lookAt(camOrto.OBS, camOrto.VRP, camOrto.UP);  
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View2[0][0]);
  }
}

void MyGLWidget::projectTransform()
{
  if (!camPlanta)
    ExamGLWidget::projectTransform();
  else
  {
    glm::mat4 Proj2 = glm::ortho(camOrto.left, camOrto.right, camOrto.bottom, camOrto.top, camOrto.zNear, camOrto.zFar);
    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj2[0][0]);
  }
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) {
  makeCurrent();
  switch (event->key()) {
  case Qt::Key_V: { // pintar Cubs o Patricio

      pintaPatricio = !pintaPatricio;
    break;
	}
  case Qt::Key_1: {  // moure patricio posicio 1

      //anglePatricioMirarEixY = 0; // ja no caldria, ho fa el slot

      emit Cub1();
    break;
	}
  case Qt::Key_2: {  // moure patricio posicio 2
      
      //anglePatricioMirarEixY = 2*M_PI/3;

      emit Cub2();
    break;
	}
  case Qt::Key_3: {  // moure patricio posicio 3

      //anglePatricioMirarEixY = 2*(2*M_PI/3);

      emit Cub3();
    break;
	}
  case Qt::Key_F: {  // canvi color focus

    focus_blanc = !focus_blanc;

    if (focus_blanc) colFocusCam = glm::vec3(1.0, 1.0, 1.0);
    else colFocusCam = glm::vec3(1.0, 1.0, 0.0);

    glUniform3fv(colFocusCamLoc, 1, &colFocusCam[0]);
    break;
	}
  case Qt::Key_C: { // canvi camera
      
      if (not camPlanta) {
        camPlanta = true;
        emit camera2();
      }
      else {
        camPlanta = false;
        emit camera1();
      }
      
      projectTransform();
      viewTransform();
      
    break;
	}
  case Qt::Key_Right: {  // Orbitar antihorari

      angleOrbitaCub1 += 2*M_PI/3;
      angleOrbitaCub2 += 2*M_PI/3;
      angleOrbitaCub3 += 2*M_PI/3;
      anglePatricioMirarEixY += 2*M_PI/3;
    break;
	}
  case Qt::Key_Left: {  // Orbitar horari

      angleOrbitaCub1 -= 2*M_PI/3;
      angleOrbitaCub2 -= 2*M_PI/3;
      angleOrbitaCub3 -= 2*M_PI/3;
      anglePatricioMirarEixY -= 2*M_PI/3;
    break;
	}
  case Qt::Key_R: {  // fer Reset
  
      reset();  
    break;
	}
  default: ExamGLWidget::keyPressEvent(event); break;
  }
  update();
}


void MyGLWidget::reset() {

  angleOrbitaCub1 = 0.0f;
  angleOrbitaCub2 = 2*M_PI/3; // 120 graus
  angleOrbitaCub3 = angleOrbitaCub2*2;

  posPatricio = glm::vec3(5., 0., 0.);

  pintaPatricio = false;
  angleIniPatricioMirarEixY = -M_PI/2; // 90º
  anglePatricioMirarEixY = 0;

  camPlanta = false;

  iniFocus();
  ExamGLWidget::iniCamera();

  emit camera1(); // tornar el boto a la camara original

  emit Cub1(); // tornar el patricio a la posicio inici (cub1)

  //projectTransform();  // ja no fa falta, ho fa el slot de tractaCamera1
  //viewTransform();
}


// === SLOTS ===

void MyGLWidget::tractaCamera1() {
  makeCurrent();
  camPlanta = false;
  projectTransform();
  viewTransform();
  update();
}

void MyGLWidget::tractaCamera2() {
  makeCurrent();
  camPlanta = true;
  projectTransform();
  viewTransform();
  update();
}

void MyGLWidget::tractaCub1() {
  makeCurrent();

  anglePatricioMirarEixY = 0;

  update();
}

void MyGLWidget::tractaCub2() {
  makeCurrent();

  anglePatricioMirarEixY = 2*M_PI/3;

  update();
}

void MyGLWidget::tractaCub3() {
  makeCurrent();

  anglePatricioMirarEixY = 2*(2*M_PI/3);

  update();
}

