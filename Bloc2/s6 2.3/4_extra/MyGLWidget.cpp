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


void MyGLWidget::animacio() {
    makeCurrent();
    anglePatricio4 += float(M_PI)/40.f; // 4.5º
    update();
}


void MyGLWidget::initializeGL() {

    BL2GLWidget::initializeGL();

    // inicialitzacio parametres
    cam3persona.prespectiva = true;
    cam3persona.psi = 0;  
    cam3persona.theta = 0; 
    cam3persona.phi = 0;
    ratoli.inici_accio = false;
    ratoli.invertir_moviment = false;
    ratoli.sensitibity = 200;
    ratoli.modo_freestyle = false;

    patricio.angle = 0;
    anglePatricio4 = 0;

    connect(&timer, SIGNAL(timeout()), this, SLOT(animacio())); //nou
    timer_activat = false;


    ini_escena();

    ini_camera();


    glEnable(GL_DEPTH_TEST); // Z-buffer    
}


void MyGLWidget::ini_escena() {

    float minx, miny, minz, maxx, maxy, maxz;

    minx = -2.5; miny = 0.0; minz = -2.5;
    maxx = 2.5;  maxy = 1.0;  maxz = 2.5;

    escena.minim = glm::vec3(minx, miny, minz);     
    escena.maxim = glm::vec3(maxx, maxy, maxz);  
    escena.centre = glm::vec3((minx+maxx)/2.f, (miny+maxy)/2.f, (minz+maxz)/2.f);   // (min+max)/2.f

    escena.radi = glm::distance(escena.centre, escena.maxim);
}


void MyGLWidget::ini_camera() {

    cam3persona.distancia =  2*escena.radi;  // d = 2*R
    cam3persona.FOVinicial = 2*(asin(float(escena.radi/(cam3persona.distancia)))); // FOV = 2*alfa  //sin(alfa) = R/2*R  
    cam3persona.FOV   = cam3persona.FOVinicial;
    cam3persona.raw   = 1.0f;

    cam3persona.left   = -escena.radi;
    cam3persona.right  =  escena.radi;
    cam3persona.bottom = -escena.radi;
    cam3persona.top    =  escena.radi;

    cam3persona.zNear = cam3persona.distancia - escena.radi;
    cam3persona.zFar  = cam3persona.distancia + escena.radi;
    cam3persona.OBS   = escena.centre + cam3persona.distancia*glm::vec3(0., 0., 1.);  // VRP + d*v  (v vec normalitzat en la direcció)
    cam3persona.VRP   = escena.centre;
    cam3persona.UP    = glm::vec3(0, 1, 0);

    projectTransform();
    viewTransform();
}


void MyGLWidget::paintGL() {

    // Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projectTransform();
    viewTransform();

    // Carreguem la transformació de model
    modelTransformPatricio1 ();

    // Activem el VAO
    glBindVertexArray (patricio.VAO);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, patricio.model.faces().size()*3);

    glBindVertexArray (0);

    // El segon patricio és el mateix que el primer, amb diferent transformació pero pintat igual que el primer.
    modelTransformPatricio2 ();

    pintaPatricio();


    modelTransformPatricio3 ();

    pintaPatricio();

    // 4t Patricio (el que orbita)
    modelTransformPatricio4 ();

    pintaPatricio();


    // Terra
    modelTransformTerra();
    
    glBindVertexArray (VAO_Terra);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray (0);
}


void MyGLWidget::pintaPatricio() {
   
    glBindVertexArray (patricio.VAO);

    glDrawArrays(GL_TRIANGLES, 0, patricio.model.faces().size()*3);

    glBindVertexArray (0);
}


void MyGLWidget::resizeGL (int w, int h) {
    ample = w;
    alt = h;

    float ra = float(w)/float(h); // rav

    cam3persona.raw = ra;   
    if (cam3persona.prespectiva) {
        if (ra < 1) {
            cam3persona.FOV = 2*atan(tan(cam3persona.FOVinicial/2)/ra);
        }
    }
    else { //resize ortogonal
        if (ra > 1) {
            cam3persona.left = -escena.radi*ra;
            cam3persona.right = escena.radi*ra;
        }
        else {
            cam3persona.bottom = -escena.radi/ra;
            cam3persona.top = escena.radi/ra;
        }
    }
}


void MyGLWidget::modelTransformPatricio1() {
    glm::mat4 transform (1.0f);
    //transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::translate(transform, escala*glm::vec3(2, 0, 2)); // multipliquem per escala perque falta el escalat general
    transform = glm::rotate(transform, patricio.angle, glm::vec3(0, 1, 0));
    transform = glm::scale(transform, glm::vec3(patricio.escala*escala));
    transform = glm::translate(transform, glm::vec3(0-patricio.centreBase.x, 0-patricio.centreBase.y, 0-patricio.centreBase.z));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformPatricio2() {
    glm::mat4 transform (1.0f);
    transform = glm::rotate(transform, patricio.angle+float(M_PI)/2.f, glm::vec3(0, 1, 0));
    transform = glm::scale(transform, glm::vec3(patricio.escala*escala));
    transform = glm::translate(transform, glm::vec3(0-patricio.centreBase.x, 0-patricio.centreBase.y, 0-patricio.centreBase.z));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformPatricio3() {
    glm::mat4 transform (1.0f);
    //transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::translate(transform, escala*glm::vec3(-2, 0, -2));
    transform = glm::rotate(transform, patricio.angle+float(M_PI), glm::vec3(0, 1, 0));
    transform = glm::scale(transform, glm::vec3(patricio.escala*escala));
    transform = glm::translate(transform, glm::vec3(0-patricio.centreBase.x, 0-patricio.centreBase.y, 0-patricio.centreBase.z));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformPatricio4() {
    glm::mat4 transform (1.0f);
    //transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::rotate(transform, patricio.angle+anglePatricio4, glm::vec3(0, 1, 0));
    transform = glm::translate(transform, escala*glm::vec3(1.5, 0, 0));  
    transform = glm::scale(transform, glm::vec3(patricio.escala*escala));
    transform = glm::translate(transform, glm::vec3(0-patricio.centreBase.x, 0-patricio.centreBase.y, 0-patricio.centreBase.z));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::modelTransformTerra() {
    glm::mat4 transform (1.0f);
    transform = glm::scale(transform, glm::vec3(escala));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform() {
    glm::mat4 Proj;
    if (cam3persona.prespectiva) Proj = glm::perspective(cam3persona.FOV, cam3persona.raw, cam3persona.zNear, cam3persona.zFar);
    else Proj = glm::ortho(cam3persona.left, cam3persona.right, cam3persona.bottom, cam3persona.top, cam3persona.zNear, cam3persona.zFar); 
  
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
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


void MyGLWidget::keyPressEvent(QKeyEvent *event) {

    makeCurrent();

    switch (event -> key()) {

        case Qt::Key_S :  // escalar a més gran
            escala += 0.05;
            break;

        case Qt::Key_D :  // escalar a més petit
            escala -= 0.05;
            break;

        case Qt::Key_R :  // rotar
            patricio.angle += M_PI/4;
            break;

        case Qt::Key_O :   
            if(cam3persona.prespectiva) cam3persona.prespectiva = false;
            else cam3persona.prespectiva = true;
            break;

        case Qt::Key_Z :   // zoom-in

            if (cam3persona.FOV > 0.1) cam3persona.FOV -= float(M_PI)/40.; // en radians //serien 4.5º  
            // Per ortogonal diferent, modifiquem el window
            if (cam3persona.left < 0) {  // tope perqe no amplii massa
                
                cam3persona.left += 0.3;
                cam3persona.right -= 0.3;
                cam3persona.bottom += 0.3;
                cam3persona.top -= 0.3;
            }
            break;

        case Qt::Key_X :   // zoom-out

            if (cam3persona.FOV < M_PI) cam3persona.FOV += float(M_PI)/40.; 

            // Per l'ortogonal
            if (cam3persona.left > -5.5) {
                cam3persona.left -= 0.3;
                cam3persona.right += 0.3;
                cam3persona.bottom -= 0.3;
                cam3persona.top += 0.3;
            }
            break;

        case Qt::Key_T :   // Activa/Desactiva timer animació
            if (not timer_activat) {
                timer_activat = true;
                timer.start(16); // cada 16 ms es crida la animació (60 cops per segon) si s'augmenta el valor va més lent girant
            }
            else {
                timer_activat = false;
                timer.stop();
            }
            break;

        default: event->ignore(); break;
    }
    update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *event) {

    makeCurrent();
    if (event -> buttons() == Qt::LeftButton && 
    !(event -> modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier))) { // click esquerra només
        ratoli.inici_accio = true;

        ratoli.pos_inicial.x = event->x();
        ratoli.pos_inicial.y = event->y();
    }
    //[EXTRA]: amb shift + click   moviment invertit 
    else if (event -> buttons() == Qt::LeftButton && (event -> modifiers() & Qt::ShiftModifier)) {
        ratoli.inici_accio = true;
        ratoli.invertir_moviment = true;

        ratoli.pos_inicial.x = event->x();
        ratoli.pos_inicial.y = event->y();
    }
    //[] amb ctrl + click   modo freestyle 
    else if (event -> buttons() == Qt::LeftButton && (event -> modifiers() & Qt::ControlModifier)) {
        ratoli.inici_accio = true;
        ratoli.modo_freestyle = true;
        ratoli.sensitibity = 300;
    }

    update();
}

void MyGLWidget::mouseMoveEvent (QMouseEvent *event) {

    makeCurrent();
    if (ratoli.inici_accio) {
        if (!ratoli.invertir_moviment) {
            cam3persona.psi += (event->x() - ratoli.pos_inicial.x)/ratoli.sensitibity; // divirir tot entre 200, és un suavitzador
            cam3persona.theta -= (event->y() - ratoli.pos_inicial.y)/ratoli.sensitibity;

            if (not ratoli.modo_freestyle) {
                ratoli.pos_inicial.x = event->x(); // la posicio actual passa a ser la anterior
                ratoli.pos_inicial.y = event->y();
            }  
        }
        else { //[Opional] invertir moviment
            cam3persona.psi -= (event->x() - ratoli.pos_inicial.x)/ratoli.sensitibity; 
            cam3persona.theta += (event->y() - ratoli.pos_inicial.y)/ratoli.sensitibity;

            if (not ratoli.modo_freestyle) {
                ratoli.pos_inicial.x = event->x(); 
                ratoli.pos_inicial.y = event->y();
            }
        }
    }
    
    update();
}

void MyGLWidget::mouseReleaseEvent (QMouseEvent *event) {

    makeCurrent();
    ratoli.inici_accio = false;
    ratoli.invertir_moviment = false; // []
    if (ratoli.modo_freestyle) {      // []
        ratoli.pos_inicial.x = event->x(); 
        ratoli.pos_inicial.y = event->y();
    }
    ratoli.modo_freestyle = false;    // []
    event->ignore();
    update();
}


void MyGLWidget::calcularCapsa_Patricio() {
  
    float minx, miny, minz, maxx, maxy, maxz;
    minx = maxx = patricio.model.vertices()[0];
    miny = maxy = patricio.model.vertices()[1];
    minz = maxz = patricio.model.vertices()[2];

    for (unsigned int i = 3; i < patricio.model.vertices().size(); i += 3) {
        minx = fmin(minx, patricio.model.vertices()[i+0]);
        maxx = fmax(maxx, patricio.model.vertices()[i+0]);

        miny = fmin(miny, patricio.model.vertices()[i+1]);
        maxy = fmax(maxy, patricio.model.vertices()[i+1]);

        minz = fmin(minz, patricio.model.vertices()[i+2]);
        maxz = fmax(maxz, patricio.model.vertices()[i+2]);
    }

    patricio.minim = glm::vec3(minx, miny, minz);
    patricio.maxim = glm::vec3(maxx, maxy, maxz);
    patricio.centre = glm::vec3((patricio.minim + patricio.maxim) / 2.f);
    patricio.centreBase = glm::vec3((minx + maxx)/2.f, miny, (minz + maxz)/2.f);
    patricio.amplada = maxx - minx;
    patricio.profunditat = maxz - minz;
    patricio.alcada = maxy - miny;
    patricio.escala = 1./(maxy-miny);
}

void MyGLWidget::creaBuffers() {

    patricio.model.load("../../../models/Patricio.obj");  //ruta relativa, a 3 carpetes superior
   
    calcularCapsa_Patricio(); 
    
    // Creació del VAO
    glGenVertexArrays(1, &patricio.VAO);
    glBindVertexArray(patricio.VAO);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.model.faces().size()*3*3, patricio.model.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);


    // colors vertex
    GLuint VBO_color;
    glGenBuffers(1, &VBO_color);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patricio.model.faces().size()*3*3, patricio.model.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(colorLoc);


    // Desactivem el VAO
    glBindVertexArray(0);

    // Terra
    glm::vec3 Vertices[4];
    Vertices[0] = glm::vec3(-2.5, 0., -2.5); //posterior esquerra
    Vertices[1] = glm::vec3(2.5, 0., -2.5);  //posterior dreta
    Vertices[2] = glm::vec3(-2.5, 0.0, 2.5);  //frontal esq
    Vertices[3] = glm::vec3(2.5, 0., 2.5);   //frontal dreta

    // VAO Terra
    glGenVertexArrays(1, &VAO_Terra);
    glBindVertexArray(VAO_Terra);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO_Terra;
    glGenBuffers(1, &VBO_Terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra);


    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);
    glm::vec3 Colors[4];
    Colors[0] = glm::vec3(1, 0, 0); //color vermell
    Colors[1] = glm::vec3(1, 0, 0);
    Colors[2] = glm::vec3(1, 0, 0);
    Colors[3] = glm::vec3(1, 0, 0);

    GLuint VBO_color_Terra;
    glGenBuffers(1, &VBO_color_Terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_Terra);

    
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

    // Activem l'atribut
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);  
    glEnableVertexAttribArray(colorLoc);
}


void MyGLWidget::carregaShaders() {
    BL2GLWidget::carregaShaders();
    projLoc = glGetUniformLocation(program->programId(), "proj");
    viewLoc = glGetUniformLocation(program->programId(), "view");
}
