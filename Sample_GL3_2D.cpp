#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <time.h>
#include <stdlib.h>

#include <thread>
#include <ao/ao.h>
#include <mpg123.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
//#include <FTGL/ftgl.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include <GL/gl.h>
#include <GL/glu.h>
#define F first
#define S second
#define PB push_back
#define MP make_pair

#define BITS 8


using namespace std;


mpg123_handle *mh;
unsigned char *buffer;
size_t buffer_size;
size_t done;
int err;

int driver;
ao_device *dev;

typedef struct COLOR {
    float r;
    float g;
    float b;
} COLOR;
ao_sample_format format;
int channels, encoding;
long rate;
std::vector< pair<pair<float,float> , COLOR> > brickpos;
std::vector< pair< pair<float,float> , pair<float,COLOR> > > bulpos;
std::vector<pair<pair<float,float>, float> > mirror;
int flag_t=0,flag_r=0,flag_y=0,flag_u=0,flag_i=0;
int gameMap_trans[10][10]={
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
		{2, 2, 2, 2, 2, 2, 2, 6, 6, 6},
		{2, 2, 2, 4, 4, 4, 6, 6, 4, 6},
		{2, 2, 2, 2, 3, 3, 4, 4, 4, 6},
		{2, 2, 2, 2, 3, 3, 4, 4, 4, 6},
		{2, 2, 2, 4, 4, 4, 6, 6, 4, 6},
		{2, 2, 2, 2, 2, 2, 2, 6, 6, 6},
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
	};
map<string,int> sevenseg;
map<string, pair<float,float> > dimension;

int gameMapTrap_trans[10][10]={
		{1 ,1, 4, 1, 1, -1, -1, -1, -1, -1},
		{1 ,1, 1, 1, 1, -1, -1, -1, -1, -1},
		{1 ,1, 1, 1, 1, -1, -1, -1, -1, -1},
		{-1, -1, 1, 1, -1, -1, -1, -1, -1, -1},
		{-1, -1, 5, 5, -1, -1, -1, -1, -1, -1},
		{-1, -1, 5, 5, 1 ,1, 8, 1, 1, 1},
		{-1, -1, 1, 1, 1 ,1, 8, 1, 1, 1},
		{-1, -1, 1, 1, 1 ,1, 8, 1, 1, 1},
		{-1, -1, 1, 1, 1 ,1, 1, 1, 9, 1},
		{-1, -1, 1, 1, 1 ,1, 1, 1, 1, 1}
	};

double mouse_x,mouse_y;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID; // For use with normal shader
	GLuint TexMatrixID; // For use with texture shader
} Matrices;



float triangle_rot_dir = 1;
float rectangle_rot_dir = -1;
bool triangle_rot_status = true;
		float eye_x_fin;
		float eye_y_fin;
		float eye_z_fin;

bool rectangle_rot_status = true;
int inAir=0;
//Camera eye, target and up vector components
float eye_x,eye_y,eye_z;
float target_x=-50,target_y,target_z=-50;
float angle=0;
float camera_radius;
int camera_disable_rotation=0;
int said = 0;
int player_sprint=0;

typedef struct Point {
    float x;
    float y;
    float z;
} Point;
struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;
	GLuint TextureBuffer;
	GLuint TextureID;

	GLenum PrimitiveMode; // GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY and GL_TRIANGLES_ADJACENCY
	GLenum FillMode; // GL_FILL, GL_LINE
	int NumVertices;
};
typedef struct VAO VAO;
VAO *redbuckO,*redbuck2O,*greenbuckO,*greenbuck2O,*whitelineO,*blacklineO,*lazerO1,*lazerO2,*redbrick,*greenbrick,*blackbrick,*mirrorO1,*mirrorO2,*mirrorO3,*mirrorO4,*spc_redbrick,
*gold_brick; 
VAO *circle1,*bulletO,*bullet1,*bullet2;
VAO *topO, *bottomO, *middleO, *middle1O,*middle2O,*right1O,*right2O,*left1O,*left2O,*trapezium;
VAO *topO_big, *bottomO_big, *middleO_big, *middle1O_big,*middle2O_big,*right1O_big,*right2O_big,*left1O_big,*left2O_big;

typedef struct Triangle {
    int p1;
    int p2;
    int p3; //Indices of the points corresponding to the triangle
} Triangle;

struct Sprite {
    float weight;
    int dz;
    int dy;
    int dx;
    float rotation_y_offset;
    int isMovingAnim;
    float x_scale,y_scale,z_scale;
    float angle_y;
    float angle_z;
    float rotation_z_offset;
    float radius;
    int fixed;
    int inAir;
    float rotation_x_offset;
    float friction; //Value from 0 to 1
    int health;
    float angle_x; //Current Angle (Actual rotated angle of the object)
    int isRotating;
    int direction_x; //0 for clockwise and 1 for anticlockwise for animation
    int direction_y;
    int direction_z;
    float remAngle; //the remaining angle to finish animation
    float x_speed,y_speed,z_speed;
    int status;
    VAO* object;
    float x,y,z;
    string name;
};
typedef struct Sprite Sprite;

bool verttoright = false, verttoleft = false, verttoup = false, verttodown = false;
bool hortoleft = false, hortoright = false, hortoup = false, hortodown = false;
bool standing = false, roll = false, horhor = false;
int player_moving_right=0;
int player_moving_left=0;
int player_moving_backward=0;
int player_moving_forward=0;
int player_health=100;
int player_score=0;
int player_rotating=0;
float camera_fov=1.3;
int currentLevel=0;
Point aaaaa,bbbbb,ccccc;
int height,width;
int camera_follow=0;
int camera_follow_adjust=0;
int camera_top=0;
int camera_fps=0;
int vertroll=0;
float fps_head_offset=0,fps_head_offset_x=0;
int head_tilting=0;
int isNight=0;
int current_jump=0;
int powerup_timer=0;
int super_jump_mode=0;
Point aaaaaa,bbbbab,ccccca;
bool gameOver = false;
bool bridgeclosed = true;
map <string, Sprite> playerObjects;
map <string, Sprite> objects;

float timeElapsed = 0, start_time = 0;
int numMoves = 0;
int timeToFinishLevel=0;
int timeToStartLevel=0;
int elevatorFinishLevel=0;
int elevatorStartLevel=0;

void reshapeWindow (GLFWwindow* window, int width, int height);
// void* play_audio(string audioFile);

//The level specific map and trap map are loaded from files
//1 is not present, 2,3,4 are present
int gameMap[10][10]={
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0}
};
COLOR red = {1,0,0};
COLOR green = {0,1,0};
COLOR black = {0,0,0};
COLOR white = {1,1,1};
COLOR blue = {0,0,1};
COLOR lightpink = {255/255.0,122/255.0,173/255.0};
COLOR darkpink = {255/255.0,51/255.0,119/255.0};
COLOR gold = {255.0/255.0,223.0/255.0,0.0/255.0};
COLOR lightred = {255.0/255.0,179.0/255.0,179.0/255.0};
COLOR lightgreen = {153.0/255.0,255.0/255.0,153.0/255.0};
COLOR bluegreen = {0,1,1};
//1 is not present, 2,3,4 are present
int gameMapTrap[10][10]={
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0}
};

string convertInt(int number)
{
    if (number == 0)
        return "0";
    string temp="";
    string returnvalue="";
    while (number>0)
    {
        temp+=number%10+48;
        number/=10;
    }
    for (int i=0;i<temp.length();i+=1)
        returnvalue+=temp[temp.length()-i-1];
    return returnvalue;
}

// void goToNextLevel(GLFWwindow* window);

GLuint programID, waterProgramID, fontProgramID, textureProgramID;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	long long foo;
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line)){
			VertexShaderCode += "\n" + Line;
			foo++;
		}
		foo--;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
	cout << "Compiling shader : " <<  vertex_file_path << endl;

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage( max(InfoLogLength, int(1)) );
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	cout << VertexShaderErrorMessage.data() << endl;

	// Compile Fragment Shader
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);
	cout << "Compiling shader : " << fragment_file_path << endl;

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage( max(InfoLogLength, int(1)) );
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	cout << FragmentShaderErrorMessage.data() << endl;

	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	cout << "Linking program" << endl;

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	cout << ProgramErrorMessage.data() << endl;

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	cout << "Error: " << description << endl;
}

void quit(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
	return;
}

glm::vec3 getRGBfromHue (int hue)
{
	float intp;
	float fracp = modff(hue/60.0, &intp);
	float x = 1.0 - abs((float)((int)intp%2)+fracp-1.0);

	if(hue>=300)
		return glm::vec3(1,0,x);
	if (hue < 300 && hue>240)
		return glm::vec3(x,0,1);
	if (hue < 240 && hue>180)
		return glm::vec3(0,x,1);
	if (hue < 180 && hue>120)
		return glm::vec3(0,1,x);
	if (hue < 120 && hue>60)
		return glm::vec3(x,1,0);
	if (hue < 60)
		return glm::vec3(1,x,0);
}

/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	long long z=240;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	z = sqrt(240);
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
	z += z%2;
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
						  0,                  // attribute 0. Vertices
						  3,                  // size (x,y,z)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );
	z += sqrt(z);
	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	z *= ((z%2)%2);
	glVertexAttribPointer(
						  1,                  // attribute 1. Color
						  3,                  // size (r,g,b)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );

	return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i+=1) {
		long long wait = 0;
		color_buffer_data [3*i + 2] = blue;
		wait++;
		color_buffer_data [3*i + 1] = green;

		if(1) color_buffer_data [3*i] = red;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

struct VAO* create3DTexturedObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* texture_buffer_data, GLuint textureID, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->TextureID = textureID;
	vao->FillMode = fill_mode;
	vao->NumVertices = numVertices;
	vao->PrimitiveMode = primitive_mode;
	long long zzz=0;
	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->TextureBuffer));  // VBO - textures
	zzz++;
	glBindVertexArray (vao->VertexArrayID); // Bind the VAO
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
	zzz *= sqrt(240);
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
						  0,                  // attribute 0. Vertices
						  3,                  // size (x,y,z)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );
	for (int i = 0; i < 1400; i+=1)
	{
		zzz = sqrt(zzz);
		zzz += 10;
		/* code */
	}
	glBindBuffer (GL_ARRAY_BUFFER, vao->TextureBuffer); // Bind the VBO textures
	glBufferData (GL_ARRAY_BUFFER, 2*numVertices*sizeof(GLfloat), texture_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	zzz = zzz%2;
	glVertexAttribPointer(
						  2,                  // attribute 2. Textures
						  2,                  // size (s,t)
						  GL_FLOAT,           // type
						  GL_FALSE,           // normalized?
						  0,                  // stride
						  (void*)0            // array buffer offset
						  );
	return vao;
}
long long audio_init() {
    /* initializations */
    ao_initialize();
    driver = ao_default_driver_id();
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = 3000;
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));

    /* open the file and get the decoding format */
    mpg123_open(mh, "./subway.mp3");
    mpg123_getformat(mh, &rate, &channels, &encoding);

    /* set the output format and open the output device */
    format.matrix = 0;
    format.channels = channels;
    format.rate = rate;
    format.byte_format = AO_FMT_NATIVE;
    format.bits = mpg123_encsize(encoding) * BITS;
    dev = ao_open_live(driver, &format, NULL);
    return 1;
}

long long  audio_play() {
    /* decode and play */
    if (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
        ao_play(dev, (char*) buffer, done);
    else mpg123_seek(mh, 0, SEEK_SET);
    return 1;
}

long long audio_close() {
    /* clean up */
    free(buffer);
    ao_close(dev);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    ao_shutdown();
    return 1;
}
void createTrapezium()
{
    static const GLfloat vertex_buffer_data [] = {
    -1.2,-1,0, // vertex 1
    1.2,-1,0, // vertex 2
    0.6, 1,0, // vertex 3

    0.6, 1,0, // vertex 3
    -0.6, 1,0, // vertex 4
    -1.2,-1,0  // vertex 1
  };
  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    0,0,1, // color 2
    0,1,0, // color 3

    0,1,0, // color 3
    0.3,0.3,0.3, // color 4
    1,0,0  // color 1
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  trapezium = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);

}
void setStrokes(char val){
    sevenseg["top"]=0;
    sevenseg["bottom"]=0;
    sevenseg["middle"]=0;
    sevenseg["left1"]=0;
    sevenseg["left2"]=0;
    sevenseg["right1"]=0;
    sevenseg["right2"]=0;
    sevenseg["middle1"]=0;
    sevenseg["middle2"]=0;
    if(val=='0' || val=='2' || val=='3' || val=='5' || val=='6'|| val=='7' || val=='8' || val=='9' || val=='P' || val=='I' || val=='O' || val=='N' || val=='T' || val=='S' || val=='E'){
        sevenseg["top"]=1;
    }
    if(val=='2' || val=='3' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9' || val=='P' || val=='S' || val=='Y' || val=='E'){
        sevenseg["middle"]=1;
    }
    if(val=='0' || val=='2' || val=='3' || val=='5' || val=='6' || val=='8' || val=='9' || val=='O' || val=='S' || val=='I' || val=='Y' || val=='U' || val=='L' || val=='E' || val=='W'){
        sevenseg["bottom"]=1;
    }
    if(val=='0' || val=='4' || val=='5' || val=='6' || val=='8' || val=='9' || val=='P' || val=='O' || val=='N' || val=='S' || val=='Y' || val=='U' || val=='L' || val=='E' || val=='W'){
        sevenseg["left1"]=1;
    }
    if(val=='0' || val=='2' || val=='6' || val=='8' || val=='P' || val=='O' || val=='N' || val=='U' || val=='L' || val=='E' || val=='W'){
        sevenseg["left2"]=1;
    }
    if(val=='0' || val=='1' || val=='2' || val=='3' || val=='4' || val=='7' || val=='8' || val=='9' || val=='P' || val=='O' || val=='N' || val=='Y' || val=='U' || val=='W'){
        sevenseg["right1"]=1;
    }
    if(val=='0' || val=='1' || val=='3' || val=='4' || val=='5' || val=='6' || val=='7' || val=='8' || val=='9' || val=='O' || val=='N' || val=='S' || val=='Y' || val=='U' || val=='W'){
        sevenseg["right2"]=1;
    }
    if(val=='I' || val=='T'){
        sevenseg["middle1"]=1;
    }
    if(val=='I' || val=='T' || val=='W'){
        sevenseg["middle2"]=1;
    }
    // cout<<"yes sevenseg defined\n";
    // cout<<"score: "<<val<<endl;
    return;
}
/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}
VAO * createCircle (string name, COLOR color, float r, int NoOfParts, string component, int fill)
{
	// cout<<name<<" yes in circle\n";
    int parts = NoOfParts;
    float radius = r;
    GLfloat vertex_buffer_data[parts*9];
    GLfloat color_buffer_data[parts*9];
    int i,j;
    float angle=(2*M_PI/parts);
    float current_angle = 0;
    for(i=0;i<parts;i+=1)
    {
        for(j=0;j<3;j+=1){
            color_buffer_data[i*9+j*3]=color.r;
            color_buffer_data[i*9+j*3+1]=color.g;
            color_buffer_data[i*9+j*3+2]=color.b;
        }
        vertex_buffer_data[i*9]=0;
        vertex_buffer_data[i*9+1]=0;
        vertex_buffer_data[i*9+2]=0;
        vertex_buffer_data[i*9+3]=radius*cos(current_angle);
        vertex_buffer_data[i*9+4]=radius*sin(current_angle);
        vertex_buffer_data[i*9+5]=0;
        vertex_buffer_data[i*9+6]=radius*cos(current_angle+angle);
        vertex_buffer_data[i*9+7]=radius*sin(current_angle+angle);
        vertex_buffer_data[i*9+8]=0;
        current_angle+=angle;
    }
    VAO* circle;
    if(fill==1)
        circle = create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_FILL);
    else
        circle = create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_LINE);
    // cout<<"circle created\n";
    dimension[name].F = r;
    dimension[name].S = r;
    return circle;
}
void createReqObj(VAO * obj, float x,float y, float angle, glm::mat4 VP)
{
	glm::mat4 MVP;  // MVP = Projection * View * Model
	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translateRectangle = glm::translate (glm::vec3(x,y,0));        // glTranslatef
	glm::mat4 rotateRectangle = glm::rotate((float)(angle*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	Matrices.model *= (translateRectangle * rotateRectangle);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// VAO *rect = createRectangle ("redbuck", 100, redbuck,redbuck,redbuck,redbuck, 100, 100, 40, 40, "bucket");
  	draw3DObject(obj);
  	// cout<<"created\n";
	return;
}
void draw3DTexturedObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Bind Textures using texture units
	glBindTexture(GL_TEXTURE_2D, vao->TextureID);

	// Enable Vertex Attribute 2 - Texture
	glEnableVertexAttribArray(2);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->TextureBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle

	// Unbind Textures to be safe
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* Create an OpenGL Texture from an image */
// GLuint createTexture (const char* filename)
// {
// 	GLuint TextureID;
// 	// Generate Texture Buffer
// 	glGenTextures(1, &TextureID);
// 	// All upcoming GL_TEXTURE_2D operations now have effect on our texture buffer
// 	glBindTexture(GL_TEXTURE_2D, TextureID);
// 	// Set our texture parameters
// 	// Set texture wrapping to GL_REPEAT
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	// Set texture filtering (interpolation)
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

// 	// Load image and create OpenGL texture
// 	int twidth, theight;
// 	unsigned char* image = SOIL_load_image(filename, &twidth, &theight, 0, SOIL_LOAD_RGB);
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
// 	glGenerateMipmap(GL_TEXTURE_2D); // Generate MipMaps to use
// 	SOIL_free_image_data(image); // Free the data read from file after creating opengl texture
// 	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess it up

// 	return TextureID;
// }


/**************************
 * Customizable functions *
 **************************/


/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Function is called first on GLFW_PRESS.

	if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_H:
				camera_follow_adjust=1-camera_follow_adjust;
				break;
			case GLFW_KEY_UP:
			 hortoup = true;
			 numMoves++;
			 break;
			case GLFW_KEY_DOWN:
			 hortodown = true;
			 numMoves++;
			 break;
			case GLFW_KEY_T:
				flag_t = 1;
				break;
			case GLFW_KEY_R:
				flag_r = 1;
				break;
			case GLFW_KEY_Y:
				flag_y = 1;
				break;
			case GLFW_KEY_LEFT:
				hortoleft = true;
				numMoves++;
				break;
			case GLFW_KEY_U:
				flag_u = 1;
				break;
			case GLFW_KEY_I:
				flag_i = 1;
				break;
			case GLFW_KEY_Z:
				player_sprint=0;
				break;
			case GLFW_KEY_J:
				player_moving_forward=0;
				break;
			case GLFW_KEY_N:
				player_moving_backward=0;
				break;
			case GLFW_KEY_X:
				// do something ..
				break;
			case GLFW_KEY_P:
				triangle_rot_status = !triangle_rot_status;
				break;
			case GLFW_KEY_C:
				rectangle_rot_status = !rectangle_rot_status;
				break;
			case GLFW_KEY_M:
				player_rotating=0;
				player_moving_right=0;
				break;
			case GLFW_KEY_B:
				player_rotating=0;
				player_moving_left=0;
				break;
			case GLFW_KEY_RIGHT:
				hortoright = true;
				numMoves++;
				break;
			// case GLFW_KEY_D:
			// 	player_moving_right=0;
			// 	break;
			// case GLFW_KEY_A:
			// 	player_moving_left=0;
			// 	break;
			// case GLFW_KEY_S:
			// 	head_tilting=0;
			// 	break;
			default:
				break;
		}
	}
	else if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_SPACE:
				if(((super_jump_mode==1 && current_jump==0) || inAir==0)){
					//Dont let the person jump when inside the elevator
					if(inAir!=0)
						current_jump=1;
					else
						current_jump=0;
					if(super_jump_mode)
						objects["player"].y_speed=15;
					else
						objects["player"].y_speed=13;
					objects["player"].y+=5;
					inAir=1;
					// jump_height=objects["player"].y;
				}
				break;
			case GLFW_KEY_N:
				player_moving_backward=1;
				break;
			case GLFW_KEY_J:
				player_moving_forward=1;
				break;
			case GLFW_KEY_Z:
				player_sprint=1;
				break;
			case GLFW_KEY_B:
				if(camera_fps==1){
					player_moving_left=1;
				}
				else{
					player_rotating=-1; //The left key has a slight problem when used together with up or down and space.
				}
				break;
			case GLFW_KEY_M:
				if(camera_fps==1){
					player_moving_right=1;
				}
				else{
					player_rotating=1;
				}
				break;
			case GLFW_KEY_ESCAPE:
				quit(window);
				break;
			default:
				break;
		}
	}
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
			quit(window);
			break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */



/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	 is different from WindowSize */
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = camera_fov; //Use from 1 to 2

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	 glLoadIdentity ();
	 gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	 Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 7000.0f);

	// Ortho projection for 2D views
	//Matrices.projection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, -1000.0f, 5000.0f);
}

void mousescroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset==1) {
        camera_fov/=1.1;
    }
    else if(yoffset==-1){
        camera_fov*=1.1; //make it bigger than current size
    }
    if(camera_fov<=0.5){
    	camera_fov=0.5;
    }
    if(camera_fov>=2){
    	camera_fov=2;
    }
    reshapeWindow(window,700,1400);
}

VAO *triangle, *skybox, *skybox1, *skybox2, *skybox3, *skybox4, *skybox5;

// Creates the triangle object used in this sample code
void createTriangle ()
{
	static const GLfloat vertex_buffer_data [] = {
		0, 1,0, // vertex 0
		-1,-1,0, // vertex 1
		1,-1,0, // vertex 2
	};

	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 0
		0,1,0, // color 1
		0,0,1, // color 2
	};

	triangle = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

// Creates the rectangle object used in this sample code
VAO * createRectangle (string name, float weight, COLOR colorA, COLOR colorB, COLOR colorC, COLOR colorD, float x, float y, float height, float width, string component)
{
    // GL3 accepts only Triangles. Quads are not supported
    float w=width/2,h=height/2;
    GLfloat vertex_buffer_data [] = {
        -w,-h,0, // vertex 1
        -w,h,0, // vertex 2
        w,h,0, // vertex 3

        w,h,0, // vertex 3
        w,-h,0, // vertex 4
        -w,-h,0  // vertex 1
    };

    GLfloat color_buffer_data [] = {
        colorA.r,colorA.g,colorA.b, // color 1
        colorB.r,colorB.g,colorB.b, // color 2
        colorC.r,colorC.g,colorC.b, // color 3

        colorC.r,colorC.g,colorC.b, // color 4
        colorD.r,colorD.g,colorD.b, // color 5
        colorA.r,colorA.g,colorA.b // color 6
    };

  // cout<<"create3dobject called\n";
  VAO *rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
  dimension[component] = MP(height,width);
  dimension[name] = MP(height,width);
  return rectangle;
}

long long createModel (string name, float x_pos, float y_pos, float z_pos, float x_scale, float y_scale, float z_scale, string filename, string layer, int status, int weight) //Create object from blender
{
    int start=0;
    float a,b,c;
    vector<Point> points;
    int len=0;
    string line;
    GLfloat color_buffer_data [100000] = {    };
    GLfloat vertex_buffer_data [100000] = {    };
    ifstream myfile;
    myfile.open(filename.c_str());
    if (myfile.is_open()){
        while (myfile >> line){
            if(line.length()==1 && line[0]=='v'){
                myfile >> a >> b >> c;
                Point cur_point = {};
                cur_point.x=a;
                cur_point.y=b;
                cur_point.z=c;
                points.push_back(cur_point);
            }
        }
        myfile.close();
    }
    int t[3],temp;
    int bcount=0,ccount=0;
    myfile.open(filename.c_str());
    if (myfile.is_open()){
        while (myfile >> line){
            if(line.length()==1 && line[0]=='f'){
                string linemod;
                getline(myfile, linemod);
                int j,ans=0,tt=0,state=0;
                float zzz;
                	for (int i = 0; i < 1400; i+=1){
						zzz = sqrt(zzz);
						zzz += 10;
						/* code */
					}
                for(j=0;j<linemod.length();j+=1){
                    if(linemod[j]==' '){
                        ans=0;
                        state=1;
                    }
                    else if(linemod[j]=='/' && ans!=0 && state==1){
                        t[tt]=ans;
                        tt++;
                        state=0;
                    }
                    else if(linemod[j]!='/'){
                        ans=ans*10+linemod[j]-'0';
                    }
                }
                t[tt]=ans;
                Triangle my_triangle = {};
                my_triangle.p1=t[0]-1;
                my_triangle.p2=t[1]-1;
                my_triangle.p3=t[2]-1;
                vertex_buffer_data[bcount+8]=points[my_triangle.p3].z*z_scale;
                vertex_buffer_data[bcount+7]=points[my_triangle.p3].y*y_scale;
                vertex_buffer_data[bcount+6]=points[my_triangle.p3].x*x_scale;
                vertex_buffer_data[bcount+5]=points[my_triangle.p2].z*z_scale;
                vertex_buffer_data[bcount+4]=points[my_triangle.p2].y*y_scale;
                vertex_buffer_data[bcount+3]=points[my_triangle.p2].x*x_scale;
                vertex_buffer_data[bcount+2]=points[my_triangle.p1].z*z_scale;
                vertex_buffer_data[bcount+1]=points[my_triangle.p1].y*y_scale;
                vertex_buffer_data[bcount]=points[my_triangle.p1].x*x_scale;
                bcount+=9;
            }
            if(line.length()==1 && line[0]=='c'){
                float r1,g1,b1,r2,g2,b2,r3,g3,b3;
                myfile >> r1 >> g1 >> b1 >> r2 >> g2 >> b2 >> r3 >> g3 >> b3;
                color_buffer_data[ccount+8]=b3/255.0;
                color_buffer_data[ccount+7]=g3/255.0;
                color_buffer_data[ccount+6]=r3/255.0;
                color_buffer_data[ccount+4]=g2/255.0;
                color_buffer_data[ccount+5]=b2/255.0;
                color_buffer_data[ccount+3]=r2/255.0;
                color_buffer_data[ccount+2]=b1/255.0;
                color_buffer_data[ccount+1]=g1/255.0;
                color_buffer_data[ccount]=r1/255.0;
                ccount+=9;
            }
        }
        myfile.close();
    }
    VAO* myobject = create3DObject(GL_TRIANGLES, bcount/3, vertex_buffer_data, color_buffer_data, GL_FILL);
    Sprite vishsprite = {};
    vishsprite.z_scale=z_scale;
    vishsprite.y_scale=y_scale;
    vishsprite.x_scale=x_scale;
    vishsprite.weight=weight;
    vishsprite.health=100;
    vishsprite.friction=0.4;
    vishsprite.fixed=0;
    vishsprite.status=status;
    vishsprite.z=z_pos;
    vishsprite.y=y_pos;
    vishsprite.x=x_pos;
    vishsprite.object = myobject;
    vishsprite.name = name;
    if(layer=="player")
    	playerObjects[name]=vishsprite;
    else
    	objects[name]=vishsprite;
    return 1;
}

float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;
double prev_mouse_x;
double prev_mouse_y;
float gravity=0.5;
float trapTimer=0;
int justInAir=0;
float player_speed=1.5;

float previous_mouse_y,previous_mouse_x;
float previous_mouse_y2,previous_mouse_x2;
float previous_mouse_y3,previous_mouse_x3;
int previous=0;


/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw(GLFWwindow* window, int c_ortho)
{
    // glm::mat4 VP = Matrices.projection * Matrices.view;
	// int fbwidth=700, fbheight=1400;
	// 	GLfloat fov = camera_fov; //Use from 1 to 2

    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 7000.0f);
	if(flag_t)
	{
		camera_fov=1.3;
		fps_head_offset_x=0;
		fps_head_offset=0;
		target_z=objects["player"].z;
		eye_x = objects["player"].x+camera_radius*cos(angle*M_PI/180);
		target_y=0;
		eye_z = objects["player"].z+camera_radius*sin(angle*M_PI/180);
		eye_y=1100;
		target_x=objects["player"].x;
		camera_top=1;
		camera_radius=1; //Top view
		camera_fps=0;
		camera_follow=0;
		camera_disable_rotation=1;
		reshapeWindow(window,700,1400);
		flag_t = 0;

	}
	if(flag_r)
	{

		camera_fov=1.3;
		fps_head_offset_x=0;
		fps_head_offset=0;
		target_z=-50;
		target_y=0;
		target_x=-50;
		eye_y=1100;
		eye_x = -50+camera_radius;
		eye_z = -50;
		camera_radius=800; //Tower view
		camera_top=0;
		camera_fps=0;
		camera_follow=0;
		camera_disable_rotation=1;
		reshapeWindow(window,700,1400);
		flag_r = 0;
	}
	if(flag_y)
	{
		camera_fov=1.3;
		fps_head_offset_x=0;
		fps_head_offset=0;
		target_z=-50;
		target_y=0;
		target_x=-50;
		eye_y=1100;
		eye_z = camera_radius*sin(angle*M_PI/180);
		eye_x = camera_radius*cos(angle*M_PI/180);
		eye_x-=50;
		eye_z-=50;
		camera_radius=800; //Tower view
		camera_top=0;
		camera_fps=0;
		camera_follow=0;
		camera_disable_rotation=0;
		reshapeWindow(window,700,1400);		
		flag_y = 0;

	}
	if(flag_i)
	{
		camera_fov=1.3;
		fps_head_offset_x=0;
		fps_head_offset=0;
		camera_top=0;
		camera_fps=1;
		camera_follow=0;
		camera_disable_rotation=1;
		reshapeWindow(window,700,1400);		
		flag_i = 0;

	}
	if(flag_u)
	{
		camera_fov=1.3;
		fps_head_offset_x=0;
		fps_head_offset=0;
		camera_follow=1;
		camera_top=0;
		camera_fps=0;
		camera_disable_rotation=1;
		reshapeWindow(window,700,1400);
		flag_u = 0;
	}


	if(objects["player"].y<-0){
		cout << "Player Died" << endl;
		super_jump_mode=0;
		player_score-=50;
		player_score=max(0,player_score);
		currentLevel--;
		// goToNextLevel(window);
	}
	if(camera_top){
		eye_z = camera_radius*sin(angle*M_PI/180);
		target_z=objects["player"].z;
		target_y=0;
		target_x=objects["player"].x ;
		eye_y=1100;
		eye_z+=objects["player"].z;
		eye_x = objects["player"].x + camera_radius*cos(angle*M_PI/180);
		target_z+=100;target_x+=100;
		eye_z+= 200;
	}
	if(camera_follow==1){
		target_x=objects["player"].x;
		target_y=objects["player"].y;
		target_z=objects["player"].z;
		int dist=20;
		int i,j,p,state1=1;
		string namex;
		if(camera_follow_adjust==1){
			dist=0;
			while(dist<=130){
				eye_z=target_z;
				eye_x=target_x - dist*sin(objects["player"].angle_y*M_PI/180);
				eye_y=target_y;
				eye_z -= dist*cos(objects["player"].angle_y*M_PI/180);
				objects["player"].x=eye_x;
				objects["player"].y=eye_y + (250-dist);
				objects["player"].z=eye_z;
				double zzz=0;
				for (int i = 0; i < 1400; i+=1){
					zzz = sqrt(zzz);
					zzz += 10;
					/* code */
				}
				for(i=0;i<10 && state1==1;i+=1){
					for(j=0;j<10 && state1==1;j+=1){
						for(p=1;p<gameMap[i][j] && state1==1;p++){
							namex = "floorcube";
							namex.append(convertInt(i)+convertInt(j)+convertInt(p));
					}
					}
				}
				if(state1==0)
					break;
				dist+=30;
			}
			eye_z=eye_z_fin;
			objects["player"].z=target_z;
			eye_x=eye_x_fin;
			objects["player"].x=target_x;
			eye_y=eye_y_fin;
			objects["player"].y=target_y;
		}
		else{
			eye_z=target_z;
			eye_x=target_x-150*sin(objects["player"].angle_y*M_PI/180);
			eye_y=target_y;
			eye_y += 150;
			eye_z -= 150*cos(objects["player"].angle_y*M_PI/180);;
		}
	}
	if(camera_fps==1){
		double new_mouse_x,new_mouse_y;
		glfwGetCursorPos(window,&new_mouse_x,&new_mouse_y);
		if(new_mouse_x<=10 || new_mouse_x>=1355){
			if(new_mouse_x<=10){
				objects["player"].angle_y+=1.5;
			}
			else{
				objects["player"].angle_y-=1.5;
			}
		}
		else if(abs(new_mouse_x-previous_mouse_x2)>=1){
			objects["player"].angle_y-=(new_mouse_x-previous_mouse_x2)/8;
			previous_mouse_x2=new_mouse_x;
		}
		if(new_mouse_y<=10 || new_mouse_y>=655){
			if(new_mouse_y<=10){
				fps_head_offset-=-0.3;
			}
			else{
				fps_head_offset-=0.3;
			}
		}
		else if(abs(new_mouse_y-previous_mouse_y2)>=1){
			fps_head_offset-=(new_mouse_y-previous_mouse_y2)/13;
			previous_mouse_y2=new_mouse_y;
		}
		if(fps_head_offset<=-30){
			fps_head_offset=-30;
		}
		if(fps_head_offset>=30){
			fps_head_offset=30;
		}
		eye_z=objects["player"].z+42*cos(objects["player"].angle_y*M_PI/180)-10*cos(objects["player"].angle_y*M_PI/180);
		target_z=objects["player"].z+42*cos((objects["player"].angle_y)*M_PI/180);
		eye_y=objects["player"].y+60;
		eye_x=objects["player"].x+42*sin(objects["player"].angle_y*M_PI/180)-10*sin(objects["player"].angle_y*M_PI/180);
		target_y=objects["player"].y+60+fps_head_offset;
		target_x=objects["player"].x+42*sin((objects["player"].angle_y)*M_PI/180);
	}

	if(inAir==1){
		objects["player"].y_speed-=gravity;
		if(objects["player"].y_speed > -12.0) ;
		else{
			objects["player"].y_speed=-12.0;
		}

		//Check collision in the y-axis to detect if the player is in air or not
	}

	if(player_rotating!=0){
		objects["player"].angle_y-=player_rotating*2;
	}
	if(player_moving_right!=0){
		objects["player"].z-=(1+player_sprint)*player_speed*cos((objects["player"].angle_y+90)*M_PI/180)*2;

		objects["player"].x-=(1+player_sprint)*player_speed*sin((objects["player"].angle_y+90)*M_PI/180)*2;

	}
	else if(player_moving_left!=0){
		objects["player"].z+=(1+player_sprint)*player_speed*cos((objects["player"].angle_y+90)*M_PI/180)*2;

		objects["player"].x+=(1+player_sprint)*player_speed*sin((objects["player"].angle_y+90)*M_PI/180)*2;

	}
	if(player_moving_backward!=0){
		objects["player"].z-=(1+player_sprint)*player_speed*cos(objects["player"].angle_y*M_PI/180)*2;

		objects["player"].x-=(1+player_sprint)*player_speed*sin(objects["player"].angle_y*M_PI/180)*2;

	}
	else if(player_moving_forward!=0){
		objects["player"].z+=(1+player_sprint)*player_speed*cos(objects["player"].angle_y*M_PI/180)*2;

		objects["player"].x+=(1+player_sprint)*player_speed*sin(objects["player"].angle_y*M_PI/180)*2;

	}

	double new_mouse_x,new_mouse_y;
	glfwGetCursorPos(window,&new_mouse_x,&new_mouse_y);
	prev_mouse_x=new_mouse_x;
	prev_mouse_y=new_mouse_y;
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram (programID);

	glm::vec3 target (target_x, target_y, target_z);
	glm::vec3 eye (eye_x, eye_y, eye_z);
	glm::vec3 up (0, 1, 0);

	static float c = 0;
	c++;
	Matrices.view = glm::lookAt(eye, target, up); // Fixed camera for 2D (ortho) in XY plane
	glm::mat4 VP = Matrices.projection * Matrices.view;

	glm::mat4 MVP;	// MVP = Projection * View * Model
	static int fontScale = 0;

	//Draw the objects
    for(map<string,Sprite>::iterator it=objects.begin();it!=objects.end();it++){
        string current = it->first; //The name of the current object
        glUseProgram (programID);
        if(objects[current].status==0)
            continue;
        glm::mat4 MVP;  // MVP = Projection * View * Model

        Matrices.model = glm::mat4(1.0f);

        glm::mat4 ObjectTransform;
				float bx1, bz1, bx2, bz2;
				ObjectTransform = glm::translate(glm::vec3(-0.5,-0.5,-0.5));
				if(objects[current].name == "player") {
					if(hortodown) {
						if(!standing && horhor) {
								objects[current].z += 150;
								hortodown = false;
								horhor = true;
						}
						else if(!standing && !horhor && (vertroll%4==0 || vertroll%4 == 2)) {
									objects[current].angle_z -= 100;
									objects[current].z += 100;
									objects[current].z += 125;
									objects[current].angle_z += 10;
									hortodown = false;
									standing = !standing;
							}
							else 	if(!standing && (vertroll%4!=0 && vertroll%4!=2)) {
										standing = !standing;
										objects[current].z += 100;
										objects[current].angle_z -= 100;
										objects[current].angle_x += 90;
										objects[current].angle_z += 10;
										objects[current].z += 125;
										hortodown = false;

								}
								else if(standing && vertroll % 2 == 0) {
										objects[current].z += 225;
										objects[current].angle_z -= 90;
										hortodown = false;
										standing = !standing;
								}
								else if(standing && vertroll % 2 != 0) {
										objects[current].z += 225;
										objects[current].angle_z -= 90;
										objects[current].angle_x += 90;
										hortodown = false;
										standing = !standing;
								}
					}
					else if(hortoup) {
						if(!standing && horhor) {
								objects[current].z -= 150;
								hortoup = false;
								horhor = true;
						}
						else if(!standing && (vertroll%4==0 || vertroll%4 == 2)) {
									objects[current].z -= 225;
									objects[current].angle_z += 90;
									hortoup = false;
									standing = !standing;
									horhor = false;
							}
							else 	if(!standing && (vertroll%4!=0 && vertroll%4!=2)) {
										objects[current].z -= 225;
										objects[current].angle_z += 90;
										objects[current].angle_x -= 90;
										hortoup = false;
										standing = !standing;
										horhor = false;
							}
							else if(standing && vertroll % 2 == 0) {
									objects[current].z -= 225;
									objects[current].angle_z += 90;
									hortoup = false;
									horhor = false;
									standing = !standing;
							}
							else if(standing && vertroll % 2 != 0) {
									objects[current].z -= 225;
									objects[current].angle_z += 90;
									objects[current].angle_x -= 90;
									hortoup = false;
									horhor = false;
									standing = !standing;
							}
					}
					
					else if(hortoright) {
						objects[current].angle_x -= 90;
						hortoright = false;
						if(standing) {
								horhor = true;
								standing = !standing;
								objects[current].x += 225;
								roll = true;
						}
						else {
							horhor = false;
							vertroll++;
							if(roll) {
									objects[current].x += 225;
									standing = !standing;
							}
							else {
									objects[current].x += 150;
							}
							roll = false;
						}
					}
					else if(hortoleft) {
						objects[current].angle_x += 90;
						hortoleft = false;
						if(standing) {
							 	standing = !standing;
								objects[current].x -= 225;
								roll = true;
								horhor = true;
						}
						else {
							horhor = false;
							vertroll++;
							if(roll) {
									objects[current].x -= 225;
									standing = !standing;
							}
							else {
									objects[current].x -= 150;
							}
							roll = false;
						}
					}
					if(!standing) {
						objects[current].y = 325;
					}
					if(standing) {
						objects[current].y = 400;
					}
				}

				float ox = objects["player"].x;
				float oz = objects["player"].z;
				if( 1 && (ox <= -750-10 || oz <= -750-10 || ox >= 600+10 || oz >= 600+10)) {
						gameOver = true;
						printf("%s\n", "out of board GAME OVER!");
						return;
				}
				long long i=0;
				while(i<10){
				long long j = 0;
				long long b = 2;
					while(j<10){
						if((b/2 == (b+1)/2) && standing!=0)
						{
							if(((oz <= (i-5)*150 + 75) && (oz >= (i-5)*150 - 75)) && ((ox <= (j-5)*150 + 75) && (ox >= (j-5)*150 - 75))) {
								if(gameMapTrap[i][j] == -1 || (gameMapTrap[i][j] == 5 && bridgeclosed)) {  // no brick
									gameOver = true;
									printf("%s\n", "no brick GAME OVER!");
									printf("Number of moves: %d\n", numMoves);
									// printf("Time taken: %d seconds\n", (int)(glfwGetTime() - start_time));
									return;
								}
								else if(gameMapTrap[i][j] == 9) {  // winning hole
									gameOver = true;
									b += 2;
									printf("%s\n", "YOU WON!");
									return;
								}
								else if(gameMapTrap[i][j] == 8) {  // weak bricks
									gameOver = true;
									b+=4;
									printf("%s\n", "YOU LOST!");
									return;
								}
								else if(gameMapTrap[i][j] == 4) { // draw bridge
									for(map<string,Sprite>::iterator it=objects.begin();it!=objects.end();it++){
											string current = it->first;
											b+=2;
											if(objects[current].weight == 10) {
													objects[current].status = 1;
											}
									}
									bridgeclosed = false;
								}
							}
						}
						if(standing==0 && (b%2==0)) {
							if(horhor) {
								bz1 = oz;
								bz2 = oz;
								bx1 = ox - 75;
								bx2 = ox + 75;
							}
							if(!horhor) {
								bz1 = oz - 75;
								bz2 = oz + 75;
								bx1 = ox;
								bx2 = ox;
							}

							bool t2 = (((bz2 <= (i-5)*150 + 75) && (bz2 >= (i-5)*150 - 75)) && ((bx2 <= (j-5)*150 + 75) && ( bx2 >= (j-5)*150 - 75)));
							bool tt11 = ((bz1 <= (i-5)*150 + 75) && (bz1 >= (i-5)*150 - 75));
							b+=2;
							bool tt12 = ((bx1 <= (j-5)*150 + 75) && ( bx1 >= (j-5)*150 - 75));
							bool t1 = (tt11 && tt12);
							if((t1 || t2) && ((gameMapTrap[i][j] == -1) || (gameMapTrap[i][j] == 5 && bridgeclosed))) {
								gameOver = true;
								printf("%s\n", "Toppled GAME OVER!");
								printf("Number of moves: %d\n", numMoves);
								printf("Time taken %f\n", glfwGetTime() - start_time);
								return;
							}
						}
						j+=1;
						b+=2;
					}
					i+=1;
				}

		ObjectTransform = glm::translate(glm::vec3(-0.5,-0.5,-0.5)) * ObjectTransform;
        glm::mat4 rotateObject;
        rotateObject=(glm::rotate((float)((objects[current].angle_y)*M_PI/180.0f), glm::vec3(0,1,0)))*(glm::rotate((float)((objects[current].angle_x)*M_PI/180.0f), glm::vec3(1,0,0)))*(glm::rotate((float)((objects[current].angle_z)*M_PI/180.0f), glm::vec3(0,0,1)));

        glm::mat4 translateObject = glm::translate (glm::vec3(objects[current].x, objects[current].y, objects[current].z)); // glTranslatef

		ObjectTransform = rotateObject*(glm::scale (glm::vec3(1.0f,1.0f,1.0f))) * ObjectTransform;
        ObjectTransform = translateObject*ObjectTransform;
        Matrices.model *= ObjectTransform;
        MVP = VP * Matrices.model; 

        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

        GLint myUniformLocation = glGetUniformLocation(programID, "objectPosition");
		glUniform3f(myUniformLocation,objects[current].x,objects[current].y,objects[current].z);

		if(current!="player"){
        	myUniformLocation = glGetUniformLocation(programID, "isPlayer");
        	glUniform1f(myUniformLocation,0.0);
        }
        else{
            myUniformLocation = glGetUniformLocation(programID, "isPlayer");
        	glUniform1f(myUniformLocation,1.0);
        }

        myUniformLocation = glGetUniformLocation(programID, "playerAngleY");
        glUniform1f(myUniformLocation,objects["player"].angle_y);

        myUniformLocation = glGetUniformLocation(programID, "playerAngleXZ");
        glUniform1f(myUniformLocation,-fps_head_offset);

        myUniformLocation = glGetUniformLocation(programID, "isNight");
        glUniform1f(myUniformLocation,isNight);

        myUniformLocation = glGetUniformLocation(programID, "playerPosition");
        glUniform3f(myUniformLocation,objects["player"].x,objects["player"].y+60,objects["player"].z);

        draw3DObject(objects[current].object);
    }

	glUseProgram (programID);
    //Draw the player
    for(map<string,Sprite>::iterator it=playerObjects.begin();it!=playerObjects.end();it++){
        string current = it->first; //The name of the current object
        if(playerObjects[current].status==0)
            continue;
        glm::mat4 MVP;  // MVP = Projection * View * Model

        Matrices.model = glm::mat4(1.0f);

        glm::mat4 ObjectTransform;
        glm::mat4 rotateObject; 
    	rotateObject= glm::rotate((float)((objects["player"].angle_y)*M_PI/180.0f), glm::vec3(0,1,0))*glm::rotate((float)((objects[current].angle_x)*M_PI/180.0f), glm::vec3(1,0,0))*glm::rotate((float)((objects[current].angle_z)*M_PI/180.0f), glm::vec3(0,0,1));
        glm::mat4 selfRotate ;
        selfRotate= glm::rotate((float)((playerObjects[current].angle_y)*M_PI/180.0f), glm::vec3(0,1,0))*glm::rotate((float)((playerObjects[current].angle_x)*M_PI/180.0f), glm::vec3(1,0,0))*glm::rotate((float)((playerObjects[current].angle_z)*M_PI/180.0f), glm::vec3(0,0,1));
        glm::mat4 translateSelfOffset = glm::translate (glm::vec3(playerObjects[current].rotation_x_offset,playerObjects[current].rotation_y_offset,playerObjects[current].rotation_z_offset));
        glm::mat4 translateSelfOffsetBack = glm::translate (glm::vec3(-playerObjects[current].rotation_x_offset,-playerObjects[current].rotation_y_offset,-playerObjects[current].rotation_z_offset));
        glm::mat4 translateRelative = glm::translate (glm::vec3(playerObjects[current].x,playerObjects[current].y,playerObjects[current].z));
        glm::mat4 translateRelativeBack = glm::translate (glm::vec3(-playerObjects[current].x,-playerObjects[current].y,-playerObjects[current].z));
        glm::mat4 translateObject = glm::translate (glm::vec3(playerObjects[current].x+objects["player"].x, playerObjects[current].y+objects["player"].y, playerObjects[current].z+objects["player"].z)); // glTranslatef
        ObjectTransform=translateObject*translateRelativeBack*rotateObject*translateRelative*translateSelfOffsetBack*selfRotate*translateSelfOffset;


        Matrices.model *= ObjectTransform;
        MVP = VP * Matrices.model; // MVP = p * V * M

        GLint myUniformLocation = glGetUniformLocation(programID, "objectPosition");
		glUniform3f(myUniformLocation,objects[current].x,objects[current].y,objects[current].z);

        myUniformLocation = glGetUniformLocation(programID, "isPlayer");
        glUniform1f(myUniformLocation,1.0);

        myUniformLocation = glGetUniformLocation(programID, "playerAngleY");
        glUniform1f(myUniformLocation,objects["player"].angle_y);

        myUniformLocation = glGetUniformLocation(programID, "playerAngleXZ");
        glUniform1f(myUniformLocation,-fps_head_offset);

        myUniformLocation = glGetUniformLocation(programID, "isNight");
        glUniform1f(myUniformLocation,isNight);

        myUniformLocation = glGetUniformLocation(programID, "playerPosition");
        glUniform3f(myUniformLocation,objects["player"].x,objects["player"].y+60,objects["player"].z);

        glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

        draw3DObject(playerObjects[current].object);
    }
      int copy = numMoves;
	  int place = 0;
	  int start_points = 350;
	  

	glUseProgram(textureProgramID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
	Matrices.model = (glm::mat4(1.0f)*glm::translate (glm::vec3(2, 0, 0)) * glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)));
	MVP = VP * Matrices.model;

	// Copy MVP to texture shaders
	glUniformMatrix4fv(Matrices.TexMatrixID, 1, GL_FALSE, &MVP[0][0]);

	// Set the texture sampler to access Texture0 memory
	glUniform1i(glGetUniformLocation(textureProgramID, "texSampler"), 0);
	if(c_ortho==1)
	{

		while(1)
		  {
		    setStrokes(copy%10  + '0');
		    if(sevenseg["top"])
		    {
		      createReqObj(topO,600,260,0,VP);
		            // cout<<"ad;afjal";

		    }
		    if(sevenseg["left1"])
		    {
		      createReqObj(left1O,600,260,0,VP);
	      // cout<<"ad;afjal";
		    }
		    if(sevenseg["right1"])
		    {

		      createReqObj(right1O,600,260,0,VP);
	      // cout<<"ad;afjal";
		    }  
		    if(sevenseg["middle"])
		    {

		      createReqObj(middleO,600,260,0,VP);
	      // cout<<"ad;afjal";
		    }
		    if(sevenseg["left2"])
		    {

		      createReqObj(left2O,600,260,0,VP);
	      // cout<<"ad;afjal";
		    }
		    if(sevenseg["right2"])
		    {

	      // cout<<"ad;afjal";
		      createReqObj(right2O,600,260,0,VP);
		    }
		    if(sevenseg["bottom"])
		    {

		      createReqObj(bottomO,600,260,0,VP);
	            // cout<<"ad;afjal";
		    }
		    copy /= 10;
		    place++;
		    start_points = 350 - 50*place;
		    if(copy<=0) break;
		  }

		  if(gameOver)
		  {
		 	  if(!said) cout<<"Game is over, press 'P' to play again: "<<endl,said = 1;
		  	  char gameover[] = {'Y','O','U','L','O','S','E'};
		  	  int start_points = 1000;
			  int points_offset = 0;
			  for (int i = 6; i >= 0; --i)
			  {
				setStrokes(gameover[6-i]);
				if(sevenseg["top"])
			    {
			      createReqObj(topO_big,start_points - 40 -100*i,385-300,0,VP);
			    }
			    if(sevenseg["left1"])
			    {
			      createReqObj(left1O_big,start_points - 80 -100*i,365-300-20,0,VP);
			    }
			    if(sevenseg["right1"])
			    {

			      createReqObj(right1O_big,start_points-100*i,365-300-20,0,VP);
			    }  
			    if(sevenseg["middle"])
			    {

			      createReqObj(middleO_big,start_points- 40 -100*i,345-300-40,0,VP);
			    }
			    if(sevenseg["middle1"])
			    {

			      createReqObj(middle1O_big,start_points- 40 -100*i,365-300-20,0,VP);
			    }
			    if(sevenseg["middle2"])
			    {

			      createReqObj(middle2O_big,start_points- 40 -100*i,325-300-60,0,VP);
			    }
			    if(sevenseg["left2"])
			    {

			      createReqObj(left2O_big,start_points - 80 -100*i,325-300-60,0,VP);
			    }
			    if(sevenseg["right2"])
			    {

			      createReqObj(right2O_big,start_points-100*i,325-300-60,0,VP);
			    }
			    if(sevenseg["bottom"])
			    {

			      createReqObj(bottomO_big,start_points-100*i-40,305-300-80,0,VP);
			    }
			}
		}
		reshapeWindow(window,700,1400);

	}

	// Increment angles
	float increments = 0;

}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
	GLFWwindow* window; // window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	glfwSetWindowCloseCallback(window, quit);

	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	glfwSetScrollCallback(window, mousescroll); // mouse scroll
  COLOR color = red;
  int offset = 100;
  float seg_h = 2;
  float seg_w = 30;
  string layer = "sevenseg";
  topO = createRectangle("top",100000,color,color,color,color,0,offset,seg_h,seg_w,layer);
  bottomO = createRectangle("bottom",100000,color,color,color,color,0,-offset,seg_h,seg_w,layer);
  middleO = createRectangle("middle",100000,color,color,color,color,0,0,seg_h,seg_w,layer);
  left1O = createRectangle("left1",100000,color,color,color,color,-offset/2,offset/2,seg_w,seg_h,layer);
  left2O = createRectangle("left2",100000,color,color,color,color,-offset/2,-offset/2,seg_w,seg_h,layer);
  right1O = createRectangle("right1",100000,color,color,color,color,offset/2,offset/2,seg_w,seg_h,layer);
  right2O = createRectangle("right2",100000,color,color,color,color,offset/2,-offset/2,seg_w,seg_h,layer);
  middle1O = createRectangle("middle1",100000,color,color,color,color,0,offset/2,seg_w,seg_h,layer);
  middle2O = createRectangle("middle2",100000,color,color,color,color,0,-offset/2,seg_w,seg_h,layer);
  color = red;
  seg_h = 10;
  seg_w = 60;
  layer = "youlose";
  topO_big = createRectangle("top",100000,color,color,color,color,0,offset,seg_h,seg_w,layer);
  bottomO_big = createRectangle("bottom",100000,color,color,color,color,0,-offset,seg_h,seg_w,layer);
  middleO_big = createRectangle("middle",100000,color,color,color,color,0,0,seg_h,seg_w,layer);
  left1O_big = createRectangle("left1",100000,color,color,color,color,-offset/2,offset/2,seg_w,seg_h,layer);
  left2O_big = createRectangle("left2",100000,color,color,color,color,-offset/2,-offset/2,seg_w,seg_h,layer);
  right1O_big = createRectangle("right1",100000,color,color,color,color,offset/2,offset/2,seg_w,seg_h,layer);
  right2O_big = createRectangle("right2",100000,color,color,color,color,offset/2,-offset/2,seg_w,seg_h,layer);
  middle1O_big = createRectangle("middle1",100000,color,color,color,color,0,offset/2,seg_w,seg_h,layer);
  middle2O_big = createRectangle("middle2",100000,color,color,color,color,0,-offset/2,seg_w,seg_h,layer);


	return window;
}

void initGL (GLFWwindow* window, int width, int height)
{
	// Load Textures
	for (int i = 0; i < 10; i+=1)
	{
		for (int j = 0; j < 10; j+=1)
		{
			gameMapTrap[i][j] = gameMapTrap_trans[i][j];
			gameMap[i][j] = gameMap_trans[i][j];
			/* code */
		}
		/* code */
	}
	glActiveTexture(GL_TEXTURE0);

	float scale=1;

	int k,stus;
	long long i = 0;
	long long j = 0;
	while(i<10){
		j = 0;
		while(j<10){

			if(gameMap[i][j]!=0)
			{
 				if(gameMapTrap[i][j] == -1) ;
 				else{
					int p;
					for(p=0;p<gameMap[i][j];p++){
						string name = "floorcube";
						name.append(convertInt(i)+convertInt(j)+convertInt(p));
						if(gameMapTrap[i][j] == 9) {
							continue;
						}
						else if(gameMapTrap[i][j] == 8 && gameMapTrap[i][j]!=4) {
							createModel (name,(j-5)*150,1*150+150/2,(i-5)*150,150,30,150,"Models/redcube.data","",1, 5);
						}
						else if(gameMapTrap[i][j]!=8 && gameMapTrap[i][j] == 4) {
							createModel (name,(j-5)*150,1*150+150/2,(i-5)*150,150,30,150,"Models/greencube.data","",1, 5);
						}
						else if(gameMapTrap[i][j]!=0 && gameMapTrap[i][j] == 5) {
							createModel (name,(j-5)*150,1*150+150/2,(i-5)*150,150,30,150,"Models/bluecube.data","",0, 10);
						}
						else {
							if((i+j)%2)
						createModel (name,(j-5)*150,1*150+150/2,(i-5)*150,150,30,150,"Models/cube.data","",1, 5);
						else
						createModel (name,(j-5)*150,1*150+150/2,(i-5)*150,150,30,150,"Models/oddtile.data","",1, 5);
					}
					}
				}
			}
			j+=1;
		}
		i+=1;
	}

	createModel("player",-750,325,-750+75,300*scale,150*scale,150*scale,"Models/cuboid.data","",1, 5); //The player's body

	playerObjects["playerhat"].angle_y=-90;
	if(!isNight)
		playerObjects["playerhat"].status=0;
	else
		playerObjects["playerhat"].status=1;
	objects["player"].angle_y=-90;

	programID = LoadShaders( "shader.vert", "shader.frag" );
	waterProgramID = LoadShaders ( "watershader.vert", "watershader.frag");
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



time_t old_time;

int main (int argc, char** argv)
{
	srand(time(NULL));

	old_time = time(NULL);
    audio_init();

	width = 1400;
	height = 700;
	camera_radius=800;
	angle=135;
	eye_x = -50+camera_radius*cos(angle*M_PI/180);
	eye_y = 1100;
    eye_z = -50+camera_radius*sin(angle*M_PI/180);

	GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);
	int rx = rand()%740;
    while((rx-310+50-20 < 150+35 && rx-310+50+20 > 150-35) || (rx-310+50-20 < -150+35 && rx-310+50+20 > -150-35)) { rx = rand()%740;}
    int rcol = rand()%3;
    int spc_brick = rand()%10;
    int bul_color = rand()%4;

	start_time = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		if(!gameOver) audio_play();

        // glfwGetCursorPos(window,&mouse_x,&mouse_y);
        // cout<<"mouse_x<< "<<mouse_x<<endl<<"mouse_y<<"<<mouse_y<<endl;

		if(time(NULL)-old_time>=93){
			old_time=time(NULL);
			// thread(play_audio,"Sounds/background.mp3").detach();
		}
		// OpenGL Draw commands
		draw(window,0);
		draw(window,1);
		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);

		// Poll for Keyboard and mouse events
		glfwPollEvents();
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
