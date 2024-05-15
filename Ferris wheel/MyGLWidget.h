#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resize - Es cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);  

    // keyPressEvent - Es cridat quan es prem una tecla
     virtual void keyPressEvent (QKeyEvent *event);

  private:
    //-------------------------------------------------
    // Funcions generals de pintat per objectes

    void pintaBase();
    void pintaCistella();
    void pintaSector();
    void pintaNuvol();
    //-------------------------------------------------
    //  Creació de buffers
    void creaBuffers ();        
    void creaBufferCistella();
    void creaBufferNuvol(glm::vec3 color);
    void creaBufferSector();
    void creaBufferBase();

    //-------------------------------------------------
    // Funcions de transformació pels objectes
    void transformacioSector(float angle);
    void transformacioCistella(float x, float y, float angle2);
    void transformacioBase();
    void transformacioNuvol();

    //-------------------------------------------------
    void carregaShaders ();
    void createVBOs( int size, glm::vec3 Colors[], glm::vec3 Vertices[] );
    //-------------------------------------------------
    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniforms
    GLuint transLoc;
    GLuint BaseLoc;
    GLuint NuvolLoc;
    GLuint nuvolPointsLoc;

    // Program
    QOpenGLShaderProgram *program;

    GLuint VAO_NUVOL, VAO_CISTELLA, VAO_SECTOR, VAO_BASE;
    GLint ample, alt;
    glm::vec3 COLOR_BLANC=glm::vec3(1.0,1.0,1.0);
    glm::vec3 COLOR_VERMELL=glm::vec3(1.0,0.5,0.3);
    glm::vec3 COLOR_BLAU=glm::vec3(48.0/255,58.0/255,158.0/255);
    glm::vec3 COLOR_BLAU_FLUIX=glm::vec3(109.0/255,117.0/255,201.0/255);
    glm::vec3 COLOR_GROC=glm::vec3(239.0/255,251.0/255,38.0/255);

    //status
    float angleSectorNoria = 90.f/4.f;
    float movNuvol;
    float x;
    float y;
    glm::vec3 nuvolPos[5];
};
