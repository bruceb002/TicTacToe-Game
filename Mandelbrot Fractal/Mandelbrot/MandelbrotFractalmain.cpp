#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

sf::RenderWindow window;
sf::Image imageFractal;
sf::Texture textureFractal;
sf::Sprite spriteFractal;

sf::Vector2<double> centerPoint;
//set initial mouse down position to be outside of the screen
sf::Vector2i mouseDownPosition = sf::Vector2i(-1, -1);
sf::Vector2i mouseUpPosition;

sf::RectangleShape rectSelect;

sf::Cursor cursorWait;
sf::Cursor cursorArrow;

//define screen size
const int WIDTH = 800;
const int HEIGHT = 800;

//Mandelbrot Set boundary is ((-2,1.25),(0.5,-1.25)), so it is 2.5x2.5 region, center at (-0.75,0). 
//Plot window: whole image boundary should be ((-2,2),(2,-2)). center at (0,0)  radius of 2
const double REAL_LEFT = -2;
const double REAL_RIGHT = 2;
const double IMG_TOP = 2;
const double IMG_BOTTOM = -2;

// view image area project to complex plane.
double originalScale = ((REAL_RIGHT - REAL_LEFT) + (IMG_TOP - IMG_BOTTOM)) / WIDTH / 2;
double scale = originalScale;
sf::Vector2<double> delta = sf::Vector2<double>(WIDTH / 2.0, HEIGHT / 2.0);

//the center of the Mandelbrot SET complex plane which is (0,0)
sf::Vector2<double> originalCenterPoint = sf::Vector2<double>((REAL_LEFT + REAL_RIGHT) / 2, (IMG_TOP + IMG_BOTTOM) / 2);


//the maximum number of iterations before a point is assumed to be inside the fractal set
int maxIterataions = 512;

sf::Color getColor(int iteration, int maxIterataions) {
	if (iteration <= maxIterataions && iteration >= 0) {

		return sf::Color((int)255 * pow(cos(sqrt(iteration)), 2), (int)255 * pow(cos(sqrt(iteration) * 2 + 120), 2), (int)255 * pow(cos(sqrt(iteration) * 3 + 240), 2));
	}
	else return sf::Color::Black; //iteration = -1
}

void createFractal()
{
	//always full screen
	sf::IntRect rect = sf::IntRect(sf::Vector2i(0, 0), (sf::Vector2i)(textureFractal.getSize()));
	//SFML top left pixel is at (0,0),bottom right pixel is at (WIDTH,HEIGHT), eg. (800,800). image center is (WIDTH/2,HEIGHT/2) (400,400)
	//Mandebrot set complex plane is ((-2,2),(2,-2)) 4x4 size with center at (0,0)
	//so, point (WIDTH / 2,HEIGHT/ 2) is image center map to complex plane's center (0,0)
	//conversion formula is (x-delta.x)*scale or (delta.y-y)*scale, where delta.x=WIDTH/2.0 and delta.y=HEIGHT/2.0
	//if center point is not (0,0), we need to use center point as offset

	int iteration = 0;

	sf::Vector2<double> c = sf::Vector2<double>(0, 0);
	sf::Vector2<double> z = sf::Vector2<double>(0, 0);
	long double tmp = 0;

	//Escape time algorithm
	//The color of each point represents how quickly the values reached the escape point.
	//Black is used to show values that fail to escape before the iteration limit, 
	//and gradually brighter colors are used for points that escape.
	//For values within the Mandelbrot set, escape will never occur.
	for (int x = rect.left; x < rect.width; ++x)
		for (int y = rect.top; y < rect.height; ++y)
		{
			//convert view coordinate(x,y) to complex number; originally center point is (0,0). e.g. map top left pixel (0,0) to (-2,2) 
			c.x = centerPoint.x + (x - delta.x)*scale;	//first pixel(0,0): 0+(0-400)*4/800=-2
			c.y = centerPoint.y + (delta.y - y)*scale;	//					0+(400-0)*4/800=2	
			z.x = 0;
			z.y = 0;
			iteration = 0;

			//no complex number with a real or imaginary part greater than 2 can be part of the set, a common bailout is to escape when either coefficient exceeds 2
			//or equivalently, when the sum of the squares of the real and imaginary parts exceed 4

			//f(x)=z*z +c = x*x -y*y +2*x*y*i +cx+cy*i= (x*x -y*y+cx)+(2*x*y+cy)*i 
			//where c=cx+cy*i and z=x+y*i; squared modulus is x*x+y*y; 
			while (iteration++ < maxIterataions)
			{
				if ((z.x*z.x) + (z.y*z.y) <= 4) {//squared modulus of z
					tmp = z.x * z.x - z.y * z.y + c.x;// real part of z*z +c 
					z.y = 2 * z.x*z.y + c.y;//imaginary part of z*z +c 
					z.x = tmp;
				}
				else {
					break;
				}
			}
			//point in the set will color as black(fail to escape, part of the Mandelbrot set, set iteration to -1 which will be set as black)
			//All points that never go to infinity are part of the Mandelbrot-Set. aka if iterate to max, the point is part of the set
			if (iteration >= maxIterataions) {
				iteration = -1;
			}

			// use iteration to select the RGB color
			sf::Color color = getColor(iteration, maxIterataions);
			//set the image pixel with selected color
			imageFractal.setPixel(x, y, color);

		}
}

void zoom2Selection(const sf::Vector2i& topLeft, const sf::Vector2i& bottomRight, double zoomDelta)
{
	sf::Vector2<double> tmp;

	window.setMouseCursor(cursorWait);
	int xLeft, xRight;

	if (zoomDelta != 0) { //zoom, mouse wheel moved
		tmp.x = centerPoint.x;
		tmp.y = centerPoint.y;

		if (zoomDelta > 0) { //wheel up zoom in
			scale = scale / (1 + abs(zoomDelta));
		}
		else {//wheel down zoon out
			scale = scale * (1 + abs(zoomDelta));
		}
	}
	else
	{
		//make sure we really have the topleft and bottomright 
		if (bottomRight.x < topLeft.x) {
			xLeft = bottomRight.x;
			xRight = topLeft.x;
		}
		else {
			xLeft = topLeft.x;
			xRight = bottomRight.x;
		}
		int yTop, yBottom;
		if (bottomRight.y < topLeft.y) {
			yTop = bottomRight.y;
			yBottom = topLeft.y;
		}
		else {
			yTop = topLeft.y;
			yBottom = bottomRight.y;
		}

		//convert new center point view coordinate to complex number
		//conversion formula is (x-delta.x)*scale or (delta.y-y)*scale, where delta.x=WIDTH/2.0 and delta.y=HEIGHT/2.0
		//if center point is not (0,0), we need to use center point as offset

		tmp.x = centerPoint.x + ((xLeft + (xRight - xLeft) / 2.0) - delta.x) * scale;
		tmp.y = centerPoint.y + (delta.y - (yTop + (yBottom - yTop) / 2.0)) * scale;
		//recalculate the scaling factor
		scale = scale * ((xRight - xLeft) / (double)WIDTH + (yBottom - yTop) / (double)HEIGHT) / 2.;
	}
	centerPoint = tmp;


	createFractal();
	textureFractal.loadFromImage(imageFractal);
	window.setMouseCursor(cursorArrow);
}

void initWindow(const sf::Vector2u& size)
{
	centerPoint = originalCenterPoint;
	//initialize the image
	imageFractal.create(size.x, size.y, sf::Color::Black);
	textureFractal.loadFromImage(imageFractal);
	spriteFractal.setTexture(textureFractal, true);

	zoom2Selection(sf::Vector2i(0, 0), (sf::Vector2i)(textureFractal.getSize()), 0);
}

int main()
{
	window.create(sf::VideoMode(WIDTH, HEIGHT), "Fractal - Mandelbrot", sf::Style::Close);
	cursorArrow.loadFromSystem(sf::Cursor::Arrow);
	cursorWait.loadFromSystem(sf::Cursor::Wait);
	//set the initial fratal size
	initWindow(sf::Vector2u(WIDTH, HEIGHT));
	window.setFramerateLimit(30);

	//set selected area color
	rectSelect.setFillColor(sf::Color(0, 0, 0, 0));  //transparent black
	rectSelect.setOutlineColor(sf::Color(255, 0, 0, 255)); //opaque red
	rectSelect.setOutlineThickness(1.f);

	double zoomDelta = 0;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				//close window 
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				// remember mouse initial click(down) when mouse button pressed
				mouseDownPosition.x = event.mouseButton.x;
				mouseDownPosition.y = event.mouseButton.y;
				break;
			case sf::Event::MouseButtonReleased:
				// get mouse release position to zoom in the selected area
				//only selected area is bigger than 10x10 pixel, then we will zoom in
				if (abs((double)(mouseDownPosition.x - event.mouseButton.x)) > 10 && abs((double)(mouseDownPosition.y - event.mouseButton.y)) > 10) {
					zoom2Selection(mouseDownPosition, sf::Vector2i(event.mouseButton.x, event.mouseButton.y), 0);
				}
				//reset mouse down
				mouseDownPosition.x = -1;
				mouseDownPosition.y = -1;
				break;
			case sf::Event::MouseWheelMoved:
				//every tick zoom 20%
				zoomDelta = event.mouseWheel.delta / 5.0;
				//zoom in or out the image
				zoom2Selection(sf::Vector2i(0, 0), (sf::Vector2i)(window.getSize()), zoomDelta);
				//reset mouse down
				mouseDownPosition.x = -1;
				mouseDownPosition.y = -1;
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Escape:
					//reset to original image
					centerPoint = originalCenterPoint;
					scale = originalScale;
					zoom2Selection(sf::Vector2i(0, 0), (sf::Vector2i)(textureFractal.getSize()), 0);
					break;
				case sf::Keyboard::Left:
					centerPoint.x += 0.2*scale / originalScale;
					break;
				case sf::Keyboard::Right:
					centerPoint.x -= 0.2*scale / originalScale;
					break;
				case sf::Keyboard::Up:
					centerPoint.y -= 0.2*scale / originalScale;
					break;
				case sf::Keyboard::Down:
					centerPoint.y += 0.2*scale / originalScale;
					break;
				case sf::Keyboard::C:
					//only allow zoom to Mandebrot set area when at the original image
					if (scale == originalScale && centerPoint == originalCenterPoint) {
						//center Mandelbrot area. eg. ((-2,1.25),(0.5,-1.25)) area, center at (-0.75,0) so that the fractal is at the center of the screen instead of center of the circle(0,0)
						// center point is ((REAL_LEFT + REAL_RIGHT) / 2, (IMG_TOP + IMG_BOTTOM) / 2)
						centerPoint = sf::Vector2<double>((0.5 + (-2)) / 2, (1.25 + (-1.25)) / 2);
						//((REAL_RIGHT - REAL_LEFT) + (IMG_TOP - IMG_BOTTOM)) / WIDTH / 2;
						scale = ((0.5 - (-2)) + (1.25 - (-1.25))) / 2 / WIDTH;

					}
					break;
				default:
					break;
				}
				zoom2Selection(sf::Vector2i(0, 0), (sf::Vector2i)(textureFractal.getSize()), 0);
				break;
			default:
				break;
			}
		}
		//selected area
		if (mouseDownPosition.x != -1 && mouseDownPosition.x != -1)
		{
			rectSelect.setPosition((sf::Vector2f)(mouseDownPosition));
			rectSelect.setSize((sf::Vector2f)(sf::Mouse::getPosition(window)) - (sf::Vector2f)(mouseDownPosition));
		}
		else
			rectSelect.setSize(sf::Vector2f(0, 0));

		window.clear();
		window.draw(spriteFractal);
		window.draw(rectSelect);
		window.display();
	}


	return 0;
}
