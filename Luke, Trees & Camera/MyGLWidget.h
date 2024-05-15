#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <QSpinBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDial>
#include "model.h"


class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core  {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent);
    ~MyGLWidget();

  protected:
    virtual void initializeGL ( );
    virtual void paintGL ( );
    virtual void resizeGL(int, int);

    virtual void keyPressEvent (QKeyEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mousePressEvent (QMouseEvent *e);

    virtual void treeTransform();
    virtual void setTrees(int numTrees);
    void valorsdefecte();

    virtual void LukeTransform();

    virtual void terraTransform();
  
    virtual void iniEscena ();
    virtual void iniCamera ();
    virtual void viewTransform ();
    virtual void projectTransform ();

  
    glm::vec3 dirLuke, posLuke;
    float rotLuke;

     void calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &CentreBase);
   
    void creaBuffersModels ();
    
    void creaBuffersTerra ();
    
    void carregaShaders ();


  private:
  
    int printOglError(const char file[], int line, const char func[]);

  
    float angleX, angleY;
    

    bool ortho;

    GLuint vertexLoc, colorLoc;


    GLuint transLoc, viewLoc, projLoc;

   
    typedef enum { TREE = 0, LUKE = 1, NUM_MODELS = 2} ModelType;

    
    GLuint VAO_models[NUM_MODELS];
    GLuint VAO_Terra;


     QOpenGLShaderProgram *program;

   
    GLint ample, alt;

   
    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float factorAngleX, factorAngleY;

   
    float radiEscena;
    float fov, ra=1, znear, zfar, d;
    glm::vec3 centreEscena, obs, vrp, up;

   
    Model models[NUM_MODELS];
    glm::vec3 centreBaseModels[NUM_MODELS];
    float escalaModels[NUM_MODELS];

    std::vector<glm::vec3> posicioTree;
    std::vector<float> escalaTree;
    std::vector<float> rotacioTree;
    int numTrees;

    float psi, theta, dist;

    float rotTree;

    QSpinBox *numArbresSpinBox;
    QCheckBox *orthoCheckBox;
    QPushButton *resetButton;
    QDial *treeDial ;

};
