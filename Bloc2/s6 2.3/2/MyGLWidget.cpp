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

//-----

void MyGLWidget::initializeGL() {

    BL2GLWidget::initializeGL();

    // inicialitzacio parametres
    prespectiva = true;
    patricio.angle = 0;
    cam3persona.psi = 0;  
    cam3persona.theta = 0; 
    cam3persona.phi = 0;
    ratoli.inici_accio = false;
    ratoli.invertir_moviment = false;
    ratoli.sensitibity = 200;


    ini_escena();

    ini_camera();

    glEnable(GL_DEPTH_TEST); // Z-buffer    
}


void MyGLWidget::ini_escena() {

    float minx, miny, minz, maxx, maxy, maxz;

    minx = -2.5; miny = 0.0; minz = -2.5;
    maxx = 2.5;  maxy = 4.0;  maxz = 2.5;

    escena.minim = glm::vec3(minx, miny, minz);     
    escena.maxim = glm::vec3(maxx, maxy, maxz);  
    escena.centre = glm::vec3((minx+maxx)/2.f, (miny+maxy)/2.f, (minz+maxz)/2.f);   // (min+max)/2.f

    escena.radi = glm::distance(escena.centre, escena.maxim);
}


void MyGLWidget::ini_camera() {

    cam3persona.distancia = 2*escena.radi;  // d = 2*R
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
    modelTransformPatricio ();

    // Activem el VAO
    glBindVertexArray (VAO_Patricio);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, patricio.model.faces().size()*3);

    glBindVertexArray (0);


    // Carreguem la transformació de model
    modelTransformTerra ();

    // Activem el VAO
    glBindVertexArray (VAO_Terra);

    // pintem
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray (0);
}


void MyGLWidget::resizeGL (int w, int h) {
    ample = w;
    alt = h;

    float ra = float(w)/float(h); // rav

    cam3persona.raw = ra;   
    if (prespectiva) {
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


void MyGLWidget::modelTransformPatricio() {
    glm::mat4 transform (1.0f);
    transform = glm::rotate(transform, patricio.angle, glm::vec3(0, 1, 0));
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
    if (prespectiva) Proj = glm::perspective(cam3persona.FOV, cam3persona.raw, cam3persona.zNear, cam3persona.zFar);
    else Proj = glm::ortho(cam3persona.left, cam3persona.right, cam3persona.bottom, cam3persona.top, cam3persona.zNear, cam3persona.zFar); 
  
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {  // ja no cal lookAt, ara angles Euler
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
            if(prespectiva) prespectiva = false;
            else prespectiva = true;
            break;

        default: event->ignore(); break;
    }
    update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *event) {

    makeCurrent();
    if (event -> buttons() == Qt::LeftButton && 
    !(event -> modifiers() & (Qt::ShiftModifier | Qt::ShiftModifier | Qt::ControlModifier))) { // click esquerra només
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

    update();
}

void MyGLWidget::mouseMoveEvent (QMouseEvent *event) {

    makeCurrent();
    if (ratoli.inici_accio) {
        if (!ratoli.invertir_moviment) {
            cam3persona.psi -= (event->x() - ratoli.pos_inicial.x)/ratoli.sensitibity; // divirir tot entre 200, és un suavitzador
            cam3persona.theta -= (event->y() - ratoli.pos_inicial.y)/ratoli.sensitibity;

            ratoli.pos_inicial.x = event->x(); // la posicio actual passa a ser la anterior
            ratoli.pos_inicial.y = event->y();
        }
        else { //[Opional] invertir moviment
            cam3persona.psi += (event->x() - ratoli.pos_inicial.x)/ratoli.sensitibity; 
            cam3persona.theta += (event->y() - ratoli.pos_inicial.y)/ratoli.sensitibity;

            ratoli.pos_inicial.x = event->x(); 
            ratoli.pos_inicial.y = event->y();
        }
        
    }
    
    update();
}

void MyGLWidget::mouseReleaseEvent (QMouseEvent *event) {

    makeCurrent();
    ratoli.inici_accio = false;
    ratoli.invertir_moviment = false;
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
    patricio.escala = 4./(maxy-miny);
}


void MyGLWidget::creaBuffers() {

    patricio.model.load("../../../models/Patricio.obj");  //ruta relativa, a 3 carpetes superior

    calcularCapsa_Patricio(); 


    // Creació del VAO
    glGenVertexArrays(1, &VAO_Patricio);
    glBindVertexArray(VAO_Patricio);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);

    //nou-----
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


    // Skateboard
    glm::vec3 Vertices[4];
    Vertices[0] = glm::vec3(-1.5, 0., -0.55); //posterior esquerra
    Vertices[1] = glm::vec3(1.5, 0., -0.55);  //posterior dreta
    Vertices[2] = glm::vec3(-1.5, 0.0, 0.55);  //frontal esq
    Vertices[3] = glm::vec3(1.5, 0., 0.55);   //frontal dreta

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
    Colors[0] = glm::vec3(131./255., 137./255., 128./255.); //color gris
    Colors[1] = glm::vec3(131./255., 137./255., 128./255.);
    Colors[2] = glm::vec3(131./255., 137./255., 128./255.);
    Colors[3] = glm::vec3(131./255., 137./255., 128./255.);

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
