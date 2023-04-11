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

MyGLWidget::~MyGLWidget()
{
}


void MyGLWidget::animacio() {

    makeCurrent();

    if (sentitEsqVideocamera) {
        if (angleVideocamera < (float(M_PI)/4.f)) {
            angleVideocamera += float(M_PI)/12.f;   // 15º
        }
        else sentitEsqVideocamera = false;
    }
    else {
        if (angleVideocamera > -(M_PI/4.f)) {
            angleVideocamera -= float(M_PI)/12.f;  
        }
        else sentitEsqVideocamera = true;
    }

    update();
}


void MyGLWidget::initializeGL() {
    
    LL2GLWidget::initializeGL();

    angleVideocamera = 0;
    camSeguretatActiva = false;
    RickInvisible = false;
    timer_activat = false;
    sentitEsqVideocamera = true;
    
    iniCamera2Seguretat();

    connect(&timer, SIGNAL(timeout()), this, SLOT(animacio()));
}


void MyGLWidget::iniEscena() {

    float minx, miny, minz, maxx, maxy, maxz;

    minx = -4.f; miny = 0.f; minz = -4.f;
    maxx = 4.f;  maxy = 3.f; maxz = 4.f;

    escena.minim = glm::vec3(minx, miny, minz);     
    escena.maxim = glm::vec3(maxx, maxy, maxz);  
    escena.centre = glm::vec3((minx+maxx)/2.f, (miny+maxy)/2.f, (minz+maxz)/2.f);   // (min+max)/2.f
      
    escena.radi = glm::distance(escena.centre, escena.maxim);
}


void MyGLWidget::iniCamera() {

    cam3persona.distancia = 2*escena.radi; // d = 2*R  

    cam3persona.FOVinicial = 2*(asin(float(escena.radi/(2*escena.radi)))); // FOV = 2*alfa  //sin(alfa) = R/2*R  //distancia = 2*R 
    cam3persona.FOV = cam3persona.FOVinicial;
    cam3persona.raw = 1.0f;
    cam3persona.zNear = 2*escena.radi - escena.radi;   
    cam3persona.zFar = 2*escena.radi + escena.radi; 
    cam3persona.OBS = glm::vec3(0, 2, 5); 
    cam3persona.VRP = escena.centre;  
    cam3persona.UP =  glm::vec3(0, 1, 0);    

    cam3persona.psi = 0;
    cam3persona.theta = float(M_PI)/4.f;
    cam3persona.phi = 0;

    projectTransform(); 
    viewTransform();
}

void MyGLWidget::iniCamera2Seguretat() {

    camSeguretat.distancia = 2*escena.radi;    // d = 2*R 

    camSeguretat.FOVinicial = glm::radians(60.f);  // 60º
    camSeguretat.FOV = camSeguretat.FOVinicial;
    camSeguretat.raw = 1.0f;
    camSeguretat.zNear = 0.3f;
    camSeguretat.zFar = camSeguretat.distancia + escena.radi;  

    camSeguretat.OBS = glm::vec3(-4, 3, -4);

    camSeguretat.VRP = glm::vec3(-4, 3, -4) + glm::vec3(0, -sin(float(M_PI)/2.f), 0); // al inici que estigui orientat cap abaix, al look at ja calcular l'angle actual
  
    camSeguretat.UP =  glm::vec3(0, 1, 0);   

    projectTransformCam2Seguretat();
    viewTransformCamera2Seguretat(); 
}


void MyGLWidget::paintGL()
{ 
  glViewport (0, 0, ample, alt);

  glm::vec3 neutre = glm::vec3(1.f, 1.f, 1.f);
  glUniform3fv(filtreLoc, 1, &neutre[0]);

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  viewTransform(); 
  projectTransform();

  // Rick
  glBindVertexArray (VAO_models[RICK]);
  RickTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[RICK].faces().size()*3);


    // Fer invisible
    GLuint VBO_Color;
    glGenBuffers(1, &VBO_Color);

    if (RickInvisible) {
      glm::vec3 c(0.f, 0.7f, 0.f); 
      glm::vec3 colRick[models[RICK].faces().size()*3*3];

      for (int i = 0; i < int(models[RICK].faces().size()*3*3); ++i) colRick[i] = c;

        glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colRick), colRick, GL_STATIC_DRAW);
        glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(colorLoc);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[RICK].faces().size()*3*3, models[RICK].VBO_matdiff(), GL_STATIC_DRAW);
        glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(colorLoc);  
    }

  // Morty
  glBindVertexArray (VAO_models[MORTY]);
  MortyTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[MORTY].faces().size()*3);
  
  // VideoCamera
  glBindVertexArray (VAO_models[VIDEOCAMERA]);
  VideoCameraTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[VIDEOCAMERA].faces().size()*3);  
  
  // Terra
  glBindVertexArray (VAO_Terra);
  identTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Paret
  ParetTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);


  // Camara Seguretat
  if (camSeguretatActiva) {
    glm::vec3 verd = glm::vec3(0.f, 1.f, 0.f);
    glUniform3fv(filtreLoc, 1, &verd[0]);

    glViewport (ample-ample/4, alt-alt/4, ample/4, alt/4);
    
    viewTransformCamera2Seguretat();
    projectTransformCam2Seguretat();

    // Rick
    glBindVertexArray (VAO_models[RICK]);
    RickTransform();
    glDrawArrays(GL_TRIANGLES, 0, models[RICK].faces().size()*3);

    // Morty
    glBindVertexArray (VAO_models[MORTY]);
    MortyTransform();
    glDrawArrays(GL_TRIANGLES, 0, models[MORTY].faces().size()*3);
    
    // VideoCamera
    glBindVertexArray (VAO_models[VIDEOCAMERA]);
    VideoCameraTransform();
    glDrawArrays(GL_TRIANGLES, 0, models[VIDEOCAMERA].faces().size()*3);  
    
    // Terra
    glBindVertexArray (VAO_Terra);
    identTransform();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Paret
    ParetTransform();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray (0);
  }
}


void MyGLWidget::RickTransform() {

  glm::mat4 TG(1.0f);  
  TG = glm::translate(TG, glm::vec3(-2, 0, -2));
  TG = glm::rotate(TG, float(M_PI), glm::vec3(0, 1, 0));
  TG = glm::scale(TG, glm::vec3(escalaModels[RICK]));
  TG = glm::translate(TG, -centreBaseModels[RICK]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::MortyTransform() {

    glm::mat4 TG(1.0f);  
    TG = glm::translate(TG, posMorty);
    TG = glm::rotate(TG, float(angleMorty), glm::vec3(0, 1, 0));
    TG = glm::scale(TG, glm::vec3(escalaModels[MORTY]));
    TG = glm::translate(TG, -centreBaseModels[MORTY]);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::VideoCameraTransform() {

  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, glm::vec3(-4, 3, -4));
  TG = glm::rotate(TG, angleVideocamera, glm::vec3(0, 1, 0));
  TG = glm::rotate(TG, float(M_PI)/4.f, glm::vec3(0, 1, 0)); 
  TG = glm::rotate(TG, float(M_PI)/4.f, glm::vec3(1, 0, 0));
  TG = glm::scale(TG, glm::vec3(escalaModels[VIDEOCAMERA]));
  TG = glm::translate(TG, -centreBaseModels[VIDEOCAMERA]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::viewTransform() {

    glm::mat4 View(1.0f);
    View = glm::translate(View, glm::vec3(0., 0., -cam3persona.distancia));
    View = glm::rotate(View, -cam3persona.phi, glm::vec3(0., 0., 1.));
    View = glm::rotate(View, cam3persona.theta, glm::vec3(1., 0., 0.));
    View = glm::rotate(View, -cam3persona.psi, glm::vec3(0., 1., 0.));
    View = glm::translate(View, glm::vec3(-cam3persona.VRP.x, -cam3persona.VRP.y, -cam3persona.VRP.z));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::viewTransformCamera2Seguretat() {
    
    glm::mat4 View = glm::lookAt(camSeguretat.OBS, camSeguretat.VRP + glm::vec3(sin(angleVideocamera+float(M_PI)/4.f), 0, cos(angleVideocamera+float(M_PI)/4.f)), camSeguretat.UP);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}


void MyGLWidget::projectTransform() {

    glm::mat4 Proj;
    Proj = glm::perspective(cam3persona.FOV, cam3persona.raw, cam3persona.zNear, cam3persona.zFar);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::projectTransformCam2Seguretat() {

    glm::mat4 Proj;
    Proj = glm::perspective(camSeguretat.FOV, camSeguretat.raw, camSeguretat.zNear, camSeguretat.zFar);  
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}


void MyGLWidget::resizeGL (int w, int h)  {

    ample = w;
    alt = h;
    cam3persona.raw = float(ample)/float(alt);
    factorAngleY = M_PI / ample;
    factorAngleX = M_PI / alt;
    projectTransform();
}


void MyGLWidget::keyPressEvent (QKeyEvent *event) {

    makeCurrent();
    switch (event -> key()) {
        case Qt::Key_Up: { 

            posMorty += glm::vec3(sin(angleMorty), 0, cos(angleMorty))*glm::vec3(0.1);   
            break;
        }
        case Qt::Key_Down: {
            
            posMorty -= glm::vec3(sin(angleMorty), 0, cos(angleMorty))*glm::vec3(0.1);
            break;
        }
        case Qt::Key_Left: {

            angleMorty += float(M_PI)/4.f;
            break;
        }
        case Qt::Key_Right: {

            angleMorty -= float(M_PI)/4.f;
            break;
        }
        case Qt::Key_Q: {

            if (angleVideocamera < (M_PI/4.f)) {
                angleVideocamera += float(M_PI)/180.f;
            } 
            break;
        }
        case Qt::Key_E: {

            if (angleVideocamera > -(M_PI/4.f)) {
                angleVideocamera -= float(M_PI)/180.f; 
            } 
            break;
        }
        case Qt::Key_C: {

            if (not camSeguretatActiva) {
                camSeguretatActiva = true;
            }
            else {
                camSeguretatActiva = false;
            }
            break;
        }
        case Qt::Key_G: {

            RickInvisible = !RickInvisible;
            paintGL();
            break;
        }
        case Qt::Key_R: {   // boto reset

            reset();            
            break;
        }
         case Qt::Key_T: {  // Activar/Desactivar animacio

            if (not timer_activat) {
                timer_activat = true;
                timer.start(1000);  // 1 cop per segon
            }         
            else {
                timer_activat = false;
                timer.stop();
            }
            break;
        }
        
        default: event->ignore(); break;
    }
    update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *event) {
    makeCurrent();
    LL2GLWidget::mousePressEvent(event);
    update();
}

void MyGLWidget::mouseReleaseEvent (QMouseEvent *event) {
    makeCurrent();
    LL2GLWidget::mouseReleaseEvent(event);
    
}

void MyGLWidget::mouseMoveEvent (QMouseEvent *e) {
    makeCurrent();

    cam3persona.psi -= (e->x() - xClick) * factorAngleY; 
    cam3persona.theta += (e->y() - yClick) * factorAngleX;

    xClick = e->x();
    yClick = e->y();
      
    update();
}


void MyGLWidget::carregaShaders() {

    LL2GLWidget::carregaShaders();
    filtreLoc = glGetUniformLocation(program->programId(), "filtre");
}


void MyGLWidget::reset() {
    
    angleVideocamera = 0;  
    camSeguretatActiva = false;
    RickInvisible = false;
    angleMorty = 0;
    posMorty = glm::vec3(1,0,0);
    timer_activat = false;
    timer.stop();
    sentitEsqVideocamera = true;

    cam3persona.psi = 0;
    cam3persona.theta = float(M_PI)/4.f;
    cam3persona.phi = 0;

    viewTransform();

    paintGL();
}


