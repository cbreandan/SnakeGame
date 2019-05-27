/*
CS 349 A1 Skeleton Code - Snake

- - - - - - - - - - - - - - - - - - - - - -

Commands to compile and run:

    g++ -o snake snake.cpp -L/usr/X11R6/lib -lX11 -lstdc++
    ./snake

Note: the -L option and -lstdc++ may not be needed on some machines.
*/

#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <vector>

/*
 * Header files for X functions
 */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;
 
/*
 * Global game state variables
 */
const int Border = 1;
const int BufferSize = 10;
int FPS = 30;
const int width = 800;
const int height = 600;
bool game_over = false;
int speed = 5;
char current_direction = 'E';
int score_count = 0;
vector<int> x_coords;
vector<int> y_coords;

vector<int> x_obstacles;
vector<int> y_obstacles;

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display	 *display;
	int		 screen;
	Window	 window;
	GC		 gc[3];
	int		width;		// size of window
	int		height;
};


/*
 * Function to put out a message on error exits.
 */
void error( string str ) {
  cerr << str << endl;
  exit(0);
}


/*
 * An abstract class representing displayable things. 
 */
class Displayable {
	public:
		virtual void paint(XInfo &xinfo) = 0;
};

class Fruit : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
			XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], x, y, 15, 15);
        }

        Fruit() {
            // ** ADD YOUR LOGIC **
            // generate the x and y value for the fruit
            srand(time(NULL));
            x = 250;
            y = 450;
        }

        // ** ADD YOUR LOGIC **
        /*
         * The fruit needs to be re-generated at new location every time a snake eats it. See the assignment webpage for more details.
         */
        int getX() {
            return x;
        }

        int getY() {
            return y;
        }

        void regenerate() {
            srand(time(NULL));
            x = rand() % 790;
            y = rand() % 590;
        }

    private:
        int x;
        int y;
};

Fruit fruit;

class Obstacle : public Displayable {
    public:
        virtual void paint(XInfo &xinfo) {
            XColor red;
            Colormap screen_colormap;
            screen_colormap = DefaultColormap(xinfo.display, DefaultScreen(xinfo.display));
            XAllocNamedColor(xinfo.display, screen_colormap, "red", &red, &red);
            XSetForeground(xinfo.display, xinfo.gc[1], red.pixel);
            for (int i=0; i<x_obstacles.size(); i++) {
                XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[1], x_obstacles[i], y_obstacles[i], 10, 10);
            }
        }
        Obstacle(){
            int x;
            int y;
            srand(time(NULL));
            for (int i=0; i<5; i++) {
                x = rand() % 790;
                y = rand() % 590;

                x_obstacles.push_back(x);
                y_obstacles.push_back(y);
            }
        }
};

Obstacle obstacle;

class Snake : public Displayable {
	public:
		virtual void paint(XInfo &xinfo) {
            for (int i=0; i<x_coords.size(); i++) {
                XFillRectangle(xinfo.display, xinfo.window, xinfo.gc[0], x_coords[i], y_coords[i], 10, blockSize);
            }
		}

		void move(XInfo &xinfo) {
		    if (current_direction == 'E') {
		        x = x + direction;
		        x_coords.insert(x_coords.begin(),x);
		        y_coords.insert(y_coords.begin(), y_coords[0]);
		        x_coords.pop_back();
		        y_coords.pop_back();
		    } else if (current_direction == 'S') {
		        y = y + direction;
		        y_coords.insert(y_coords.begin(), y);
                x_coords.insert(x_coords.begin(),x);
                x_coords.pop_back();
                y_coords.pop_back();
		    } else if (current_direction == 'W') {
		        x = x - direction;
		        x_coords.insert(x_coords.begin(),x);
		        y_coords.insert(y_coords.begin(), y);
		        x_coords.pop_back();
                y_coords.pop_back();
		    } else {
		        y = y - direction;
		        y_coords.insert(y_coords.begin(), y);
		        x_coords.insert(x_coords.begin(),x);
		        x_coords.pop_back();
                y_coords.pop_back();
		    }

            // ** ADD YOUR LOGIC **
            // Here, you will be performing collision detection between the snake,
            // the fruit, and the obstacles depending on what the snake lands on.
            if (didEatFruit()) {
                score_count += 1;

                if (current_direction == 'E') {
                    x += 10;
                    x_coords.insert(x_coords.begin(), x);
                    y_coords.insert(y_coords.begin(), y);
                } else if (current_direction == 'S') {
                    y += 10;
                    x_coords.insert(x_coords.begin(), x);
                    y_coords.insert(y_coords.begin(), y);
                } else if (current_direction == 'W') {
                    x -= 10;
                    x_coords.insert(x_coords.begin(), x);
                    y_coords.insert(y_coords.begin(), y);
                } else {
                    y -= 10;
                    x_coords.insert(x_coords.begin(), x);
                    y_coords.insert(y_coords.begin(), y);
                }
                fruit.regenerate();
            }

            if (didHitWall() || didHitObstacle() || didHitSelf()) {
                game_over = true;
            }
		}

		int getX() {
			return x;
		}

		int getY() {
			return y;
		}

        /*
         * ** ADD YOUR LOGIC **
         * Use these placeholder methods as guidance for implementing the snake behaviour.
         * You do not have to use these methods, feel free to implement your own.
         */
        void restart () {
            score_count = 0;
            x = 50;
            y = 50;
            current_direction = 'E';
        }

        void changeSpeed() {
            direction = speed;
        }

        bool didEatFruit() {
            if (x+10 >= fruit.getX() && x <= fruit.getX()+20 && y+10 >= fruit.getY() && y <= fruit.getY()+20) {
                return true;
            } else {
                return false;
            }
        }

        bool didHitSelf() {
            for (int i=1; i<x_coords.size(); i++) {
                if (x == x_coords[i] && y == y_coords[i]) {
                    return true;
                }
            }
            return false;
        }

        bool didHitWall() {
            if (x > width || x < 0 || y > height || y < 0) {
                return true;
            } else {
                return false;
            }
        }

        bool didHitObstacle() {
            for (int i=0; i<x_obstacles.size(); i++) {
                if (x+10 >= x_obstacles[i] && x <= x_obstacles[i]+10 && y+10 >= y_obstacles[i] && y <= y_obstacles[i]+10) {
                    return true;
                }
            }
            return false;
        }

        void turnLeft() {
        }

        void turnRight() {
        }

		Snake(int x, int y): x(x), y(y) {
		    x_coords.push_back(x);
		    y_coords.push_back(y);

		    x_coords.push_back(x-10);
		    y_coords.push_back(y);
			direction = speed;
            blockSize = 10;
		}

	private:
		int x;
		int y;
		int blockSize;
		int direction;
};



class Score : public Displayable {
    public:
      virtual void paint(XInfo& xinfo) {
        string s = "Score: " + to_string(score_count) + " FPS: " + to_string(FPS) + " Speed: " + to_string(speed);
        XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],
                          this->x, this->y, s.c_str(), s.length() );
      }

    // constructor
    Score(int x, int y): x(x), y(y)  {}

    private:
      int x;
      int y;
};

class PauseMessage : public Displayable {
    public:
      virtual void paint(XInfo& xinfo) {
        string s = "Press any key to continue!";
        XDrawImageString( xinfo.display, xinfo.window, xinfo.gc[0],
                          this->x, this->y, s.c_str(), s.length() );
      }

    // constructor
    PauseMessage(int x, int y): x(x), y(y)  {}

    private:
      int x;
      int y;
};

list<Displayable *> dList;           // list of Displayables
Snake snake(100, 450);
Score score(10, 20);



/*
 * Initialize X and create a window
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black;

   /*
	* Display opening uses the DISPLAY	environment variable.
	* It can go wrong if DISPLAY isn't set, or you don't have permission.
	*/	
	xInfo.display = XOpenDisplay( "" );
	if ( !xInfo.display )	{
		error( "Can't open display." );
	}
	
   /*
	* Find out some things about the display you're using.
	*/
	xInfo.screen = DefaultScreen( xInfo.display );

	white = XWhitePixel( xInfo.display, xInfo.screen );
	black = XBlackPixel( xInfo.display, xInfo.screen );

	hints.x = 100;
	hints.y = 100;
	hints.width = 800;
	hints.height = 600;
	hints.flags = PPosition | PSize;

	xInfo.window = XCreateSimpleWindow( 
		xInfo.display,				// display where window appears
		DefaultRootWindow( xInfo.display ), // window's parent in window tree
		hints.x, hints.y,			// upper left corner location
		hints.width, hints.height,	// size of the window
		Border,						// width of window's border
		black,						// window border colour
		white );					// window background colour
		
	XSetStandardProperties(
		xInfo.display,		// display containing the window
		xInfo.window,		// window whose properties are set
		"animation",		// window's title
		"Animate",			// icon's title
		None,				// pixmap for the icon
		argv, argc,			// applications command line args
		&hints );			// size hints for the window

	/* 
	 * Create Graphics Contexts
	 */
	int i = 0;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);

	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i],
	                     1, LineSolid, CapButt, JoinRound);

	i = 1;
    	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);

    	XSetForeground(xInfo.display, xInfo.gc[i], BlackPixel(xInfo.display, xInfo.screen));
    	XSetBackground(xInfo.display, xInfo.gc[i], WhitePixel(xInfo.display, xInfo.screen));
    	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
    	XSetLineAttributes(xInfo.display, xInfo.gc[i],
    	                     1, LineSolid, CapButt, JoinRound);

	XSelectInput(xInfo.display, xInfo.window, 
		ButtonPressMask | KeyPressMask | 
		PointerMotionMask | 
		EnterWindowMask | LeaveWindowMask |
		StructureNotifyMask);  // for resize events

	/*
	 * Put the window on the screen.
	 */
	XMapRaised( xInfo.display, xInfo.window );
	XFlush(xInfo.display);
}

/*
 * Function to repaint a display list
 */
void repaint( XInfo &xinfo) {
	list<Displayable *>::const_iterator begin = dList.begin();
	list<Displayable *>::const_iterator end = dList.end();

	XClearWindow( xinfo.display, xinfo.window );
	
	// get height and width of window (might have changed since last repaint)

	XWindowAttributes windowInfo;
	XGetWindowAttributes(xinfo.display, xinfo.window, &windowInfo);
	unsigned int height = windowInfo.height;
	unsigned int width = windowInfo.width;

	// big black rectangle to clear background
    
	// draw display list
	while( begin != end ) {
		Displayable *d = *begin;
		d->paint(xinfo);
		begin++;
	}
	XFlush( xinfo.display );
}

void handleKeyPress(XInfo &xinfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	
	/*
	 * Exit when 'q' is typed.
	 * This is a simplified approach that does NOT use localization.
	 */
	int i = XLookupString( 
		(XKeyEvent *)&event, 	// the keyboard event
		text, 					// buffer when text will be written
		BufferSize, 			// size of the text buffer
		&key, 					// workstation-independent key symbol
		NULL );					// pointer to a composeStatus structure (unused)
	if ( i == 1) {
		printf("Got key press -- %c\n", text[0]);
		if (text[0] == 'q') {
			error("Terminating normally.");
		} else if (text[0] == 'p') {
		    PauseMessage pauseMessage(320,300);
		    pauseMessage.paint(xinfo);
            while (true) {
                XNextEvent( xinfo.display, &event );
                if (event.type == KeyPress) {
                    break;
                }
            }
		} else if (text[0] == 'r') {
		    //snake.restart();
		} else if (text[0] == 'w') {
		    if (current_direction != 'S') {
		        current_direction = 'N';
		    }
		} else if (text[0] == 'a') {
		    if (current_direction != 'E') {
		        current_direction = 'W';
		    }
		} else if (text[0] == 's') {
		    if (current_direction != 'N') {
		        current_direction = 'S';
		    }
		} else if (text[0] == 'd') {
		    if (current_direction != 'W') {
		        current_direction = 'E';
		    }
		}
	}
}

void handleAnimation(XInfo &xinfo, int inside) {
    /*
     * ADD YOUR OWN LOGIC
     * This method handles animation for different objects on the screen and readies the next frame before the screen is re-painted.
     */
	snake.move(xinfo);
}

// get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {
	// Add stuff to paint to the display list
	dList.push_front(&snake);
    dList.push_front(&fruit);
    dList.push_back(&obstacle);
    dList.push_back(&score);
	
	XEvent event;
	unsigned long lastRepaint = 0;
	int inside = 0;

	while( true ) {
		/*
		 * This is NOT a performant event loop!  
		 * It needs help!
		 */
		
		if (XPending(xinfo.display) > 0) {
			XNextEvent( xinfo.display, &event );
			cout << "event.type=" << event.type << "\n";
			switch( event.type ) {
				case KeyPress:
					handleKeyPress(xinfo, event);
					break;
				case EnterNotify:
					inside = 1;
					break;
				case LeaveNotify:
					inside = 0;
					break;
			}
		}

		if (game_over) {
		    break;
		}

		// now() is a helper function I made
        unsigned long end = now(); // time in microseconds
        if (end - lastRepaint > 1000000/FPS) { // repaint at FPS
            // clear background
            XClearWindow(xinfo.display, xinfo.window);

            handleAnimation(xinfo, inside); // update animation objects
            repaint(xinfo); // my repaint

            XFlush( xinfo.display );

            lastRepaint = now(); // remember when the paint happened
        }
        // IMPORTANT: sleep for a bit to let other processes work
        if (XPending(xinfo.display) == 0) {
            usleep(1000000 / FPS - (end - lastRepaint));
        }
	}
}


/*
 * Start executing here.
 *	 First initialize window.
 *	 Next loop responding to events.
 *	 Exit forcing window manager to clean up - cheesy, but easy.
 */
int main ( int argc, char *argv[] ) {
	XInfo xInfo;

    Display* display;
    Window window;
    // open splash screen
    display = XOpenDisplay("");
    if (!display) exit (-1);
    int screen = DefaultScreen(display);
    int w = 800;
    int h = 600;
    window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                   10, 10, w, h, 2,
                                   BlackPixel(display, screen), WhitePixel(display, screen));
    XMapRaised(display, window);
    XFlush(display);

    // give server 10ms to get set up before sending drawing commands
    usleep(10 * 1000);

    // drawing demo with graphics context here ...
    GC gc = XCreateGC(display, window, 0, 0);       // create a graphics context
    XSetForeground(display, gc, BlackPixel(display, screen));
    XSetBackground(display, gc, WhitePixel(display, screen));

    //load a larger font
    XFontStruct * font;
    font = XLoadQueryFont (display, "12x24");
    XSetFont (display, gc, font->fid);

    // draw text
    string texts[7] = {};
    texts[0] = "SNAKE GAME";
    XDrawImageString( display, window, gc,
                        330, 50, texts[0].c_str(), texts[0].length());
    texts[1] = "Breandan Choi, ID: 20578568";
        XDrawImageString( display, window, gc,
                            250, 100, texts[1].c_str(), texts[1].length());
    texts[2] = "Control snake by using keys W,A,S,D.";
            XDrawImageString( display, window, gc,
                                200, 200, texts[2].c_str(), texts[2].length());
    texts[3] = "Press P to Pause and R to Restart";
                XDrawImageString( display, window, gc,
                                            220, 250, texts[3].c_str(), texts[3].length());
    texts[4] = "The objective is to eat as many fruits as possible,";
                    XDrawImageString( display, window, gc,
                                    90, 350, texts[4].c_str(), texts[4].length());
    texts[5] = "and avoid red obstacles";
                        XDrawImageString( display, window, gc,
                                        270, 400, texts[5].c_str(), texts[5].length());
    texts[6] = "Press any key to start";
                            XDrawImageString( display, window, gc,
                                            280, 550, texts[6].c_str(), texts[6].length());
    XFlush(display);
    XSelectInput(display, window, KeyPressMask);
    XEvent event;
    while (true) {
        XNextEvent( display, &event );
        if (event.type == KeyPress) {
            XCloseDisplay(display);
            break;
         }
    }
    if (argc == 3) {
        string temp_string = argv[1];
        istringstream iss(temp_string);
        iss >> FPS;

        temp_string = argv[2];
        istringstream iss_2(temp_string);
        iss_2 >> speed;

        snake.changeSpeed();
    }

	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	if (game_over) {
	    Display* display;
        Window window;
        // open display
        display = XOpenDisplay("");
        if (!display) exit (-1);
        int screen = DefaultScreen(display);
        int w = 800;
        int h = 600;
        window = XCreateSimpleWindow(display, DefaultRootWindow(display),
                                   10, 10, w, h, 2,
                                   BlackPixel(display, screen), WhitePixel(display, screen));
        XMapRaised(display, window);
        XFlush(display);

        // give server 10ms to get set up before sending drawing commands
        usleep(10 * 1000);

        // drawing demo with graphics context here ...
        GC gc = XCreateGC(display, window, 0, 0);       // create a graphics context
        XSetForeground(display, gc, BlackPixel(display, screen));
        XSetBackground(display, gc, WhitePixel(display, screen));

        //load a larger font
        XFontStruct * font;
        font = XLoadQueryFont (display, "10x20");
        XSetFont (display, gc, font->fid);

        // draw text
        string text("GAME OVER! YOUR SCORE: " + to_string(score_count) + " Press any key to quit.");
        XDrawImageString( display, window, gc,
                        100, 100, text.c_str(), text.length());

        XFlush(display);
        XSelectInput(display, window, KeyPressMask);
        XEvent event;
        while (true) {
            XNextEvent( display, &event );
            if (event.type == KeyPress) {
                XCloseDisplay(display);
                break;
            }
        }
	}
	XCloseDisplay(xInfo.display);
}