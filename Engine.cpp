#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <ctime>
#include <sstream>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
#define PI 3.14159265358979323846


float moveX = 0;
float moveZ = 0;
float angle = 0;

float rotation = 0;
float rotationGoal = 0;

bool keyPressed = false;

int timer = 30;

bool gameOver = false;
bool collisionOccurred = false;



class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}
};

Camera camera;


void drawGround(double size) {
	glPushMatrix();
	glScaled(size, 0.1, size);
	glutSolidCube(1);
	glPopMatrix();
}

void drawCylinder(float radius, float height, int slices, int stacks) {
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluCylinder(quadric, radius, radius, height, slices, stacks);
	gluDeleteQuadric(quadric);
}

void drawSphere(float radius, int slices, int stacks) {
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluSphere(quadric, radius, slices, stacks);
	gluDeleteQuadric(quadric);
}

void drawCircle(float x, float y, float radius, float r, float g, float b) {
	int i;
	int triangleAmount = 100;

	glColor3f(r, g, b);

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (i = 0; i <= triangleAmount; i++) {
		float angle = i * 2.0f * 3.14159 / triangleAmount;
		float dx = radius * cos(angle);
		float dy = radius * sin(angle);
		glVertex2f(x + dx, y + dy);
	}
	glEnd();
}

void drawStreetLamp() {
	glPushMatrix();
	glTranslatef(-0.8, 0.13, 0);
	glRotatef(rotationGoal, 0, 1, 0);
	glScalef(0.06, 0.1, 0.06);

	glColor3f(0.5, 0.5, 0.5);
	drawCylinder(0.1, 2.0, 20, 20);

	glTranslatef(0.0, 2.0, 0.0);
	glColor3f(0.8, 0.8, 0.0);
	drawSphere(0.5, 20, 20);

	glTranslatef(0.0, 0.5, 0.0);
	glColor3f(1.0, 1.0, 1.0);
	drawSphere(0.1, 10, 10);
	glPopMatrix();
}

void drawDog() {
	glPushMatrix();
	glTranslatef(0.8, 0.1, -0.8);
	glRotatef(rotation, 0, 1, 0);
	glScalef(0.06, 0.1, 0.06);

	glColor3f(0.8, 0.6, 0.4);
	glutSolidSphere(1.0, 20, 20);


	glColor3f(0.8, 0.6, 0.4);
	glPushMatrix();
	glTranslatef(0.0, 1.5, 0.0);
	glutSolidSphere(0.8, 20, 20);
	glPopMatrix();

	glColor3f(0.4, 0.2, 0.0);
	glPushMatrix();
	glTranslatef(-0.5, -1.0, 0.5);
	drawCylinder(0.2, 1.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, -1.0, 0.5);
	drawCylinder(0.2, 1.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.5, -1.0, -0.5);
	drawCylinder(0.2, 1.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.5, -1.0, -0.5);
	drawCylinder(0.2, 1.0, 20, 20);
	glPopMatrix();


	glColor3f(0.4, 0.2, 0.0);
	glPushMatrix();
	glTranslatef(0.8, 0.5, 0.0);
	glRotatef(90, 0, 1, 0);
	drawCylinder(0.1, 0.8, 20, 20);
	glPopMatrix();
	glPopMatrix();

}

void drawTree() {
	glPushMatrix();
	glTranslatef(-0.8, 0.1, 0.8);
	glRotatef(rotation, 0, 1, 0);

	glScalef(0.06, 0.1, 0.06);

	glColor3f(0.5, 0.35, 0.15);
	glPushMatrix();
	glTranslatef(0.0, -1.0, 0.0);
	glRotatef(-90, 1, 0, 0);
	drawCylinder(0.2, 1.0, 20, 20);
	glPopMatrix();


	glColor3f(0.0, 0.8, 0.0);
	glPushMatrix();
	glTranslatef(0.0, 0.5, 0.0);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();


	glColor3f(0.5, 0.35, 0.15);
	glPushMatrix();
	glTranslatef(0.0, 0.5, 0.0);
	drawCylinder(0.1, 0.5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.5, 0.0);
	glRotatef(45, 0, 1, 0);
	drawCylinder(0.1, 0.5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.5, 0.0);
	glRotatef(-45, 0, 1, 0);
	drawCylinder(0.1, 0.5, 20, 20);
	glPopMatrix();
	glPopMatrix();
}

void drawTrunk() {
	glColor3f(0.4, 0.2, 0.0);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -1.0);
	drawCylinder(0.5, 4.0, 20, 20);
	glPopMatrix();
}

void drawLeaves() {
	glColor3f(0.0, 0.8, 0.0);

	for (int i = 0; i < 360; i += 30) {
		glPushMatrix();
		glTranslatef(0.0, 0.0, 3.0);
		glRotatef(i, 0.0, 0.0, 1.0);
		glTranslatef(0.0, 0.0, 1.0);
		glutSolidCone(2.0, 5.0, 20, 20);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0.3, -0.8, 7.5);
	glColor3f(0.5, 0.3, 0.0);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.3, -1.8, 5.6);
	glColor3f(0.5, 0.3, 0.0);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -1.5, 6.7);
	glColor3f(0.5, 0.3, 0.0);
	glutSolidSphere(0.5, 20, 20);
	glPopMatrix();
}

void drawPalmTree() {
	glPushMatrix();
	glTranslatef(0.8, 0, 0.8);
	glRotatef(rotation, 0, 1, 0);
	glScalef(0.06, 0.1, 0.06);
	glRotatef(-90, 1, 0, 0);


	drawTrunk();
	drawLeaves();
	glPopMatrix();
}

GLfloat wallColors[6][3];
int currentColorIndex = 0;

void updateWallColors() {
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 3; ++j) {
			wallColors[i][j] = (float)rand() / RAND_MAX;
		}
	}
}

void drawBoundaryWalls() {
	static int lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	if (currentTime - lastUpdateTime > 2000) {
		lastUpdateTime = currentTime;
		updateWallColors();
	}

	drawGround(2.0);

	for (int i = 0; i < 6; ++i) {
		glColor3fv(wallColors[i]);

		glPushMatrix();
		switch (i) {
		case 0:

			glTranslated(0.0, 0.5, -1.0);
			glScaled(2.0, 1.0, 0.02);
			glutSolidCube(1);
			break;
		case 1:

			glTranslated(1.0, 0.5, 0.0);
			glRotated(90, 0, 1, 0);
			glScaled(2.0, 1.0, 0.02);
			glutSolidCube(1);
			break;
		case 2:

			glTranslated(-1.0, 0.5, 0.0);
			glRotated(-90, 0, 1, 0);
			glScaled(2.0, 1.0, 0.02);
			glutSolidCube(1);
			break;

		}

		glPopMatrix();
	}
}

void drawTableLeg(double thick, double len) {
	glPushMatrix();

	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawTable(double topWid, double topThick, double legThick, double legLen) {
	glPushMatrix();
	glColor3f(0.4, 0.2, 0.0);
	glTranslatef(-0.7, 0, -0.7);
	glRotatef(rotation, 0, 1, 0);

	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95 * topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
	glPopMatrix();
}

void drawRock() {
	glPushMatrix();
	glTranslatef(0.5, 0.1, 0.6);
	glRotatef(rotation, 0, 1, 0);
	glScalef(0.1, 0.1, 0.1);

	glColor3f(0.5, 0.5, 0.5);
	glPushMatrix();
	glTranslatef(0.0, 1.0, 0.0);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	glColor3f(0.3, 0.3, 0.3);
	glPushMatrix();
	glTranslatef(0.0, 0.5, 0.0);
	glScalef(1.5, 1.0, 1.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glColor3f(0.2, 0.2, 0.2);
	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.0);
	glRotatef(90, 1, 0, 0);
	drawCylinder(1.0, 1.5, 20, 20);
	glPopMatrix();

	glPopMatrix();
}

void drawPlayer() {
	glPushMatrix();
	glTranslatef(0.0 + moveX, 0.14, 0.0 + moveZ);
	glRotatef(angle, 0, 1, 0);
	glScalef(1.6, 2.2, 1.6);

	glColor3f(1.0, 0.6, 0.0);
	glPushMatrix();
	glTranslatef(0.0, 0.05, 0.011);
	glutSolidSphere(0.02, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.035, 0.0);
	glScalef(0.04, 0.08, 0.02);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.035, 0.04, 0.0);
	glScalef(0.02, 0.05, 0.02);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.035, 0.04, 0.0);
	glScalef(0.02, 0.05, 0.02);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.015, -0.01, 0.0);
	glScalef(0.02, 0.05, 0.02);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.015, -0.01, 0.0);
	glScalef(0.02, 0.05, 0.02);
	glColor3f(1.0, 1.0, 0.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();

}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, 1000 / 600, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

bool checkCollision(float lampX, float lampZ, float playerX, float playerZ, float radius, float distanceThreshold) {
	float distance = sqrt((lampX - playerX) * (lampX - playerX) + (lampZ - playerZ) * (lampZ - playerZ));
	return distance < radius && distance < distanceThreshold;
}

void Anim() {
	if (keyPressed) {
		rotation += 0.05;
	}
	rotationGoal += 0.08;

	glutPostRedisplay();

}

void Timer(int value) {
	if (timer > 0) {
		timer--;
	}
	else {
		gameOver = true;
	}
	glutTimerFunc(1000, Timer, 0);
	glutPostRedisplay();
}

void Display() {

	glClearColor(0.4f, 0.4f, 0.8f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setupCamera();
	setupLights();

	drawBoundaryWalls();
	drawPlayer();
	drawPalmTree();
	drawTable(0.4, 0.02, 0.02, 0.3);
	drawTree();
	drawDog();
	drawRock();

	float collisionDistanceThreshold = 0.08;

	if (!checkCollision(-0.8, 0.13, moveX, moveZ, 0.5, collisionDistanceThreshold)) {
		drawStreetLamp();
	}
	else {
		collisionOccurred = true;
	}


	if (collisionOccurred) {
		glClearColor(0.0f, 1.0f, 0.0f, .5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(1.0f, 1.0f, 0.0f);
		glRasterPos2f(-0.3, 0.0);
		const char* text = "YOU WIN CONGRATS!";
		for (int i = 0; text[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
		}
	}
	else if (gameOver) {
		glClearColor(0.8f, 0.0f, 0.0f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(1.0f, 1.0f, 1.0f);
		glRasterPos2f(-0.3, 0.0);
		const char* text = "YOU LOST";
		for (int i = 0; text[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
		}
	}
	else {

		glMatrixMode(GL_PROJECTION);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glRasterPos2i(glutGet(GLUT_WINDOW_WIDTH) - 90, 20);

		glColor3f(1.0f, 1.0f, 1.0f);
		std::string timerText = "Timer: " + std::to_string(timer);
		for (const char& c : timerText) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}

	}
	glFlush();
	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y) {
	float d = 0.1;
	float a = 2;


	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'e':
		camera.moveZ(d);
		break;
	case 'q':
		camera.moveZ(-d);
		break;

	case 'i':
		camera.rotateX(a);
		break;
	case 'k':
		camera.rotateX(-a);
		break;
	case 'j':
		camera.rotateY(a);
		break;
	case 'l':
		camera.rotateY(-a);
		break;

	case 'n':
		keyPressed = true;;
		break;
	case 'm':
		keyPressed = false;;
		break;

	case '1':
		camera.eye = Vector3f(0.8f, 0.4f, 0.0f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 1.0f, 0.0f);
		break;
	case '2':
		camera.eye = Vector3f(0.0f, 2.8, 0.0f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 0.0f, -1.0f);
		break;
	case '3':
		camera.eye = Vector3f(0.0f, 0.24f, 1.8f);
		camera.center = Vector3f(0.0f, 0.0f, 0.0f);
		camera.up = Vector3f(0.0f, 1.0f, 0.0f);
		break;

	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;
	float d = 0.03;

	switch (key) {
	case GLUT_KEY_UP:

		moveZ -= d;
		angle = 180;
		if (moveZ <= -1) {
			moveZ = -1;
		}
		break;
	case GLUT_KEY_DOWN:
		moveZ += d;
		angle = 0;
		if (moveZ >= 1) {
			moveZ = 1;
		}
		break;
	case GLUT_KEY_LEFT:
		moveX -= d;
		angle = -90;
		if (moveX <= -1) {
			moveX = -1;
		}
		break;
	case GLUT_KEY_RIGHT:
		moveX += d;
		angle = 90;
		if (moveX >= 1) {
			moveX = 1;
		}
		break;
	}



	glutPostRedisplay();
}

void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(1000, 600);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("Assignment 2");
	glutDisplayFunc(Display);
	glutIdleFunc(Anim);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutTimerFunc(1000, Timer, 0);

	glutMainLoop();
}