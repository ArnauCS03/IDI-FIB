#include "ExamGLWidget.h"

class MyGLWidget:public ExamGLWidget
{
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : ExamGLWidget(parent) {}
    ~MyGLWidget();


  public slots:

    void tractaCamera1();
    void tractaCamera2();
    void tractaCub1();
    void tractaCub2();
    void tractaCub3();

  signals:

    void camera1();
    void camera2();
    void Cub1();
    void Cub2();
    void Cub3();


  protected:

    virtual void initializeGL ();
    virtual void iniFocus ();
    void iniCamera2();
    virtual void paintGL ();
    virtual void resizeGL (int w, int h);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void modelTransformCub (float escala, float angle);
    virtual void modelTransformPatricio ();
    virtual void projectTransform ();
    virtual void viewTransform ();

    void reset();

  private:

    int printOglError(const char file[], int line, const char func[]);

    // Cubs
    float angleOrbitaCub1, angleOrbitaCub2, angleOrbitaCub3;
    float escalaCub1, escalaCub2, escalaCub3;

    // Patricio
    glm::vec3 posPatricio;
    float escalaPatricio;
    bool pintaPatricio;
    float angleIniPatricioMirarEixY, anglePatricioMirarEixY;

    // Focus camera
    glm::vec3 colFocusCam;
    bool focus_blanc;

    GLuint colFocusCamLoc;

    // Camera ortogonal
    struct Camera {
      glm::vec3 OBS;
      glm::vec3 VRP;
      glm::vec3 UP;
      float left;
      float right;  
      float bottom;
      float top;
      float zNear;
      float zFar;
    };

    Camera camOrto;


    bool activa_Reset;
};
