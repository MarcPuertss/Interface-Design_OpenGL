#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

MyGLWidget::MyGLWidget(QWidget *parent=0): QOpenGLWidget(parent), program(NULL)
{
  srand (time(NULL));
}

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

void MyGLWidget::initializeGL ()
{
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.5, 0.7, 1.0, 1.0); 
  carregaShaders();
  creaBuffersModels();
  creaBuffersTerra();
  iniEscena();
  iniCamera();
  valorsdefecte();

  rotTree = 0.0f;

  setTrees(5);
  numArbresSpinBox = new QSpinBox(this);
  numArbresSpinBox->setRange(5, 30);
  numArbresSpinBox->setValue(5);
  connect(numArbresSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MyGLWidget::setTrees);

  orthoCheckBox = new QCheckBox("Ortogonal", this);
  connect(orthoCheckBox, &QCheckBox::stateChanged, [=](int state) {
     makeCurrent();
      while(true){
          ortho = false;
          if (state == Qt::Checked) {
              ortho = true;
              viewTransform();
              break;
          }
          else {
              ortho = false;
              viewTransform();
              break;
          }
     }
      update();
  });
 
  resetButton = new QPushButton("Reset", this);
  connect(resetButton, &QPushButton::clicked, [=]() {
      makeCurrent();
      while(true){
        posLuke = glm::vec3(0.0f, 0.0f, 0.0f);
        rotLuke = 0.0f;
        ortho = false;
        iniCamera();
        viewTransform();
        setTrees(5);
        break;
      }
      update();
  });
 
  treeDial = new QDial(this);
  treeDial->setRange(0, 360);
  treeDial->setValue(0);
  connect(treeDial, &QDial::valueChanged, [=](int value) {
      rotTree = glm::radians(static_cast<float>(value));
      update();
  });
 
  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(orthoCheckBox);
  layout->addWidget(resetButton);
  layout->addWidget(treeDial);

  layout->setAlignment(Qt::AlignTop);
  layout->setSpacing(10);
  setLayout(layout);
 
}

void MyGLWidget::valorsdefecte(){

    makeCurrent();
     while(true){
        ortho = true;
        viewTransform();
        break;
    }
    update();

    makeCurrent();
    while(true){
      posLuke = glm::vec3(0.0f, 0.0f, 0.0f);
      rotLuke = 0.0f;
      ortho = false;
      iniCamera();
      viewTransform();
      break;
    }
    update();

}
void MyGLWidget::iniEscena ()
{
    centreEscena = glm::vec3(0,0,0);
    radiEscena = 25.0f;
    d=2*radiEscena;

    ortho=false;
}

void MyGLWidget::iniCamera() {

    angleY = 0;
    angleX = 0;
    ortho = false;

    fov = M_PI/3.0f;
    znear = 0.1f;
    zfar  = 200; 

    psi = 0.0f;
    theta = -M_PI/4.0f;

    ortho=false;
    dist = radiEscena/4;
    viewTransform();
    projectTransform();    

}

void MyGLWidget::viewTransform ()
{

    if(ortho){
          obs = glm::vec3(0.0f, 10.0f, 0.0f);
          vrp = glm::vec3(0,0,0);
          up = glm::vec3(0,0,-1);
          glm::mat4 View;
          View = glm::lookAt(obs, vrp, up);
          glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
      }
      else {
          glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0., 0., -dist));
          View = glm::lookAt(glm::vec3(0,15,2),glm::vec3(0,1,0),glm::vec3(0,1,0));
          View = glm::rotate(View, theta, glm::vec3(1,0,0));
          View = glm::rotate(View, -psi, glm::vec3(0,1,0));
          View = glm::translate(View, -vrp);
          glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
      }
}

void MyGLWidget::projectTransform ()
{
    glm::mat4 Proj(1.0f);

	Proj = glm::perspective (fov, ra, znear, zfar);

    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::paintGL ()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray (VAO_models[TREE]);
  treeTransform();

  glBindVertexArray (VAO_models[LUKE]);
  LukeTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[LUKE].faces().size()*3);

  glBindVertexArray (VAO_Terra);
  terraTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}

void MyGLWidget::resizeGL (int w, int h)
{
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;

  glViewport(0, 0, w, h);
  int marge = 10;
  int amplada = 100;
  int yPosition = marge;
  int xPosition = w - marge - amplada;
  orthoCheckBox->setGeometry(xPosition, yPosition, amplada, 20);
  yPosition += 30;
  resetButton->setGeometry(xPosition, yPosition, amplada, 20);
  yPosition += 30;
  treeDial->setGeometry(xPosition, yPosition, amplada, 50);

#endif

  ra = float(ample)/float(alt);
  factorAngleY = M_PI / ample;
  factorAngleX = M_PI / alt;
  projectTransform();

}
void MyGLWidget::setTrees(int numTrees) {
    posicioTree.resize(numTrees);
    escalaTree.resize(numTrees);
    rotacioTree.resize(numTrees);
    int minX = -25.0f;
    int maxX = 25.0f;
    int minZ = -25.0f;
    int maxZ = 25.0f;
    float alt = 0.0f;
    for (int i = 0; i < numTrees; ++i) {
        float posX =  rand() % (maxX - minX + 1) + minX;
        float posZ = rand() % (maxZ - minZ + 1) + minZ;
        float posY = alt + escalaTree[i] / 2.0f;
        posicioTree[i] = glm::vec3(posX, posY, posZ);
        escalaTree[i] = 0.08f + rand()/ (RAND_MAX/(0.13f - 0.08f));
        rotacioTree[i] = rand() % 360;
    }
    update();
}

void MyGLWidget::treeTransform ()
{
    for(long unsigned int i = 0; i < posicioTree.size(); ++i){
      glm::mat4 TG(1.0f);
      TG = glm::rotate(TG, glm::radians(rotacioTree[i]), glm::vec3(0, 1, 0));
      TG = glm::scale(TG, glm::vec3(escalaTree[i]));
      TG = glm::translate(TG, -centreBaseModels[TREE]);
      TG = glm:: translate(TG, posicioTree[i]);
      TG = glm::rotate(TG, rotTree, glm::vec3(0, 1, 0));
      glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
      glDrawArrays(GL_TRIANGLES, 0, models[TREE].faces().size()*3);
    }
}

void MyGLWidget::LukeTransform()
{

  glm::mat4 TG(1.0f);
  TG = glm::translate(TG, posLuke);
  TG = glm::rotate(TG, rotLuke, glm::vec3(0, 1, 0));
  TG = glm::scale(TG, glm::vec3(0.025));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::terraTransform ()
{
  glm::mat4 TG(1.0f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{

    makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: {
      dirLuke = glm::vec3(sin(rotLuke), 0.0f, cos(rotLuke));
      glm::vec3 newPos = posLuke + dirLuke * 0.5f;
      float minX = -5.0f, maxX = 5.0f;
      float minZ = -5.0f, maxZ = 5.0f;
      if (newPos.x >= minX && newPos.x <= maxX && newPos.z >= minZ && newPos.z <= maxZ) {
        posLuke = newPos;
      }
      break;
    }
    case Qt::Key_Left: {
      rotLuke += M_PI / 4;
      break;
    }
    case Qt::Key_Right: {
      rotLuke -= M_PI / 4;
      break;
    }
    case Qt::Key_Plus: {
      rotTree += M_PI / 4;
      break;
    }
    case Qt::Key_Minus: {
      rotTree -= M_PI / 4;
      break;
    }
    case Qt::Key_C: {
      ortho = !ortho;
      viewTransform();
      break;
    }
    case Qt::Key_R: { // reset
      posLuke = glm::vec3(0.0f, 0.0f, 0.0f);
      rotLuke = 0.0f;
      ortho = false;
      iniCamera();
      viewTransform();
      setTrees(5);
      break;
    }
    default: event->ignore(); break;
  }
  update();
}



void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)

{
    makeCurrent();
  if (DoingInteractive == ROTATE && !ortho)
  {
    float angleX = e->x();
    float angleY = e->y();

    float x = angleX - xClick;
    float y = angleY - yClick;

    psi += (x/width())*10;
    theta += (y/height())*10;

    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}


void MyGLWidget::creaBuffersTerra ()
{
  glm::vec3 posTerra[4] = {
        glm::vec3(-5.0, 0.0, -5.0),
        glm::vec3(-5.0, 0.0,  5.0),
        glm::vec3( 5.0, 0.0, -5.0),
        glm::vec3( 5.0, 0.0,  5.0)
  };

  glm::vec3 c(0.65, 0.2, 0.05);
  glm::vec3 colTerra[4] = { c, c, c, c };


  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);


  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posTerra), posTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);


  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colTerra), colTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);

  glBindVertexArray (0);
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &centreBase)
{

  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }

  escala = alcadaDesitjada/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersModels ()
{

  models[TREE].load("./models/tree.obj");
  models[LUKE].load("./models/luke_jedi.obj");

  glGenVertexArrays(NUM_MODELS, &VAO_models[0]);

  float alcadaDesitjada[NUM_MODELS] = {1,1};//,1,1,1.5,1};

  for (int i = 0; i < NUM_MODELS; i++)
  {
	  calculaCapsaModel (models[i], escalaModels[i], alcadaDesitjada[i], centreBaseModels[i]);

	  glBindVertexArray(VAO_models[i]);

	  GLuint VBO[2];
	  glGenBuffers(2, VBO);

	  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_vertices(), GL_STATIC_DRAW);
	  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(vertexLoc);

	  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_matdiff(), GL_STATIC_DRAW);
	  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(colorLoc);
  }

  glBindVertexArray (0);
}

void MyGLWidget::carregaShaders()
{

  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);

  fs.compileSourceFile(":shaders/basicShader.frag");
  vs.compileSourceFile(":shaders/basicShader.vert");

  program = new QOpenGLShaderProgram(this);

  program->addShader(&fs);
  program->addShader(&vs);

  program->link();

  program->bind();


  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");

  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc  = glGetUniformLocation (program->programId(), "Proj");
  viewLoc  = glGetUniformLocation (program->programId(), "View");
}


