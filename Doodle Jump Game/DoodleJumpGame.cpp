#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Constants.h"
#include <vector>

bool intersect(std::vector<sf::Sprite>& spritePlatform, sf::Sprite spriteDoodler, int velocity)
{
	for (int i = 0; i < spritePlatform.size(); i++)
	{
		//only consider intersect when doodler is dropping(velocity>0)
		if (spriteDoodler.getGlobalBounds().intersects(spritePlatform[i].getGlobalBounds()) && velocity>0)
		{
			return true;
		}
	}
	return false;
}

void jump(sf::Sprite& spriteDoodler, bool& change, sf::Vector2f& doodlerOriginalPosition, sf::Clock& clock, int& velocity, std::vector<sf::Sprite>& spritePlatform)
{
	int yDirection = 1;
	int xDirection = 0;
	//change is true when left/right key pressed, reset only the horizonal position but not the vertical position
	if (change) {
		doodlerOriginalPosition.x = spriteDoodler.getGlobalBounds().left;
		change = false;
	}

	if (clock.getElapsedTime().asMilliseconds() > DURATION * 1000) {
		velocity += (int)(GRAVITY * DURATION);
		sf::Vector2f new_pos(doodlerOriginalPosition.x, spriteDoodler.getGlobalBounds().top + yDirection * velocity *DURATION);
		if (new_pos.y <= doodlerOriginalPosition.y - HORIZONTAL_MOVEMENT) { // top of range
			yDirection *= -1;
			new_pos.y = doodlerOriginalPosition.y - HORIZONTAL_MOVEMENT + yDirection * velocity * DURATION;
			velocity = 0;
		}
		else if(intersect(spritePlatform, spriteDoodler,velocity)) { // if the doodler intersects a platform, reset original position & speed
			yDirection *= -1;
			//set doodler's new original position
			doodlerOriginalPosition = spriteDoodler.getPosition();
			change = false;
			new_pos.y = doodlerOriginalPosition.y + yDirection * velocity * DURATION;
			velocity = REBOUND_VELOCITY;

		}
		spriteDoodler.setPosition(new_pos);
		clock.restart();
	}

}

void warp(sf::Sprite& spriteDoodler)
{
	sf::Vector2f new_pos; 
	if (spriteDoodler.getGlobalBounds().left >= FRAME_WIDTH - DOODLER_WIDTH) {
		new_pos.x = 1;
		new_pos.y = spriteDoodler.getGlobalBounds().top;
		spriteDoodler.setPosition(new_pos);
	}
	if (spriteDoodler.getGlobalBounds().left <= 0) {
		new_pos.x = FRAME_WIDTH - PLATFORM_WIDTH;
		new_pos.y = spriteDoodler.getGlobalBounds().top;
		spriteDoodler.setPosition(new_pos);
	}
}

//create sprite base on image name/width/height etc. return the created sprite and the texture(if texture is not returned, sprite would not work)
sf::Sprite createSprite(sf::Texture& texture, sf::String imageName, int imageWidth, int imageHeight, int scaleWidth, int scaleHeight, float xInitPosition, float yInitPosition) {
	sf::Sprite sprite;

	if (texture.getSize().x == 0) { //only load when the texture was not loaded before
		if (!texture.loadFromFile(imageName, sf::IntRect(0, 0, imageWidth, imageHeight)))
		{
			return sprite;
		}
	}
	sprite.setTexture(texture);
	sprite.move(xInitPosition, yInitPosition);
	sprite.setScale(
		scaleWidth / sprite.getLocalBounds().width,
		scaleHeight / sprite.getLocalBounds().height);
	return sprite;
}

/*This method generates platforms in a random manner so the Doodle will always be
able to reach the next platform, both in the x and y direction. It also
puts constraints on the platforms so they can only be generated on screen.
 */
void generatePlatforms(std::vector<sf::Sprite>& spritePlatform,sf::Texture& texturePlatform, int vectorPosition) {

	int i = vectorPosition+1;

	while (spritePlatform[i-1].getPosition().y>0) {
		/*
		 * creates variables that constrain the platform generation based on how far the Doodle can move 
		 * in the the previous platform, minimal movement is the platform width, either left or right.
		 */
		int xMovement = (HORIZONTAL_MOVEMENT- (int) PLATFORM_WIDTH - rand() % ((HORIZONTAL_MOVEMENT- (int)PLATFORM_WIDTH) * 2));
		float n;
		if (xMovement > 0) {
			n = spritePlatform[i - 1].getPosition().x + PLATFORM_WIDTH + xMovement;
		}
		else {
			n = spritePlatform[i - 1].getPosition().x - PLATFORM_WIDTH + xMovement;
		}
		float j = spritePlatform[i - 1].getPosition().y - (rand() % (VERTICAL_MAX - VERTICAL_MIN) + VERTICAL_MIN);
		//when reach top of the window, we are done when platform generation
		if (j < 0) {
			break;
		}
		// these next two if statements puts constraints on the platforms so they cannot generate off-screen.
		if (n <= 0) {
			n = spritePlatform[i - 1].getPosition().x + PLATFORM_WIDTH;
		}
		if (n >= FRAME_WIDTH - PLATFORM_WIDTH) {
			n = (float)(FRAME_WIDTH - PLATFORM_WIDTH*(rand()%100)/100 - PLATFORM_WIDTH);
		}

		// add the new platfrom sprite to the vector
		spritePlatform.push_back(createSprite(texturePlatform, "platform.png", PLATFORM_IMG_WIDTH, PLATFORM_IMG_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, n,j));
		i++;
	}
}



void scroll(sf::Sprite& spriteDoodler, std::vector<sf::Sprite>& spritePlatform, sf::Texture& texturePlatform, sf::Vector2f& doodlerOriginalPosition)
{
	if(spriteDoodler.getGlobalBounds().top+ spriteDoodler.getGlobalBounds().height < (FRAME_HEIGHT / 2))
	{
		int yMove=0;
		for (int i = 0; i < spritePlatform.size(); i++) //loops through vector
		{
			
			if (spritePlatform[i].getPosition().y > doodlerOriginalPosition.y+ spriteDoodler.getGlobalBounds().height+1)
			{
				//erase any platform below doodler's last jump platform
				spritePlatform.erase(spritePlatform.begin() + i);
				i--; //since I removed the first vector element, we are still at index 0
			}
			else
			{
				//move any platform above(include) doodler's last jump platform position down from last jump y position to bottom of the screen
				if (yMove == 0) {
					yMove =(int) (FRAME_HEIGHT - spritePlatform[0].getGlobalBounds().top - spritePlatform[0].getGlobalBounds().height);
				}
				spritePlatform[i].setPosition(spritePlatform[i].getPosition().x, spritePlatform[i].getPosition().y + yMove);
			}
		}
		generatePlatforms(spritePlatform, texturePlatform, (int)(spritePlatform.size()) - 1);
		spriteDoodler.setPosition(spriteDoodler.getPosition().x, spriteDoodler.getPosition().y + yMove); //remember new original position after scrolling 
		doodlerOriginalPosition = spriteDoodler.getPosition();
	}
}

int main(int argc, const char ** argv[])
{
	
	srand((unsigned)time(NULL));

	bool play = false;
	
	sf::RenderWindow window(sf::VideoMode(FRAME_WIDTH, FRAME_HEIGHT), "Doodle Jump",sf::Style::Close);

	sf::Texture texturePlayButton;
	sf::Texture textureDoodler;
	sf::Texture texturePlatform;
	sf::Texture textureGameOverButton;
	sf::Texture texturePlayAgainButton;
	sf::Texture textureBackground;


	sf::Sprite spriteBackground= createSprite(textureBackground, "background.png", BACKGROUND_IMG_WIDTH, BACKGROUND_IMG_HEIGHT, FRAME_WIDTH, FRAME_HEIGHT, 0,0);
	
	//set Doodler sprite's initial position
	sf::Vector2f doodlerInitPosition;
	doodlerInitPosition.x = (window.getSize().x - (float)DOODLER_WIDTH) / 2;
	doodlerInitPosition.y = window.getSize().y - (float)DOODLER_HEIGHT - (float)PLATFORM_HEIGHT;
	sf::Sprite spriteDoodler=createSprite(textureDoodler, "Doodler.png", DOODLER_IMG_WIDTH, DOODLER_IMG_HEIGHT, DOODLER_WIDTH, DOODLER_HEIGHT, doodlerInitPosition.x, doodlerInitPosition.y);
	
	sf::Sprite spriteGameOverButton=createSprite(textureGameOverButton, "gameover.png", GAMEOVERBUTTON_IMG_WIDTH, GAMEOVERBUTTON_IMG_HEIGHT, GAMEOVERBUTTON_WIDTH, GAMEOVERBUTTON_HEIGHT, (window.getSize().x - (float)GAMEOVERBUTTON_WIDTH) / 2, (window.getSize().y - (float)GAMEOVERBUTTON_HEIGHT) / 2);

	sf::Sprite spritePlayButton = createSprite(texturePlayButton,"play.png", PLAYBUTTON_IMG_WIDTH, PLAYBUTTON_IMG_HEIGHT, PLAYBUTTON_WIDTH, PLAYBUTTON_HEIGHT, (window.getSize().x - (float)PLAYBUTTON_WIDTH) / 2, (window.getSize().y - (float)PLAYBUTTON_HEIGHT) / 2);

	sf::Sprite spritePlayAgainButton=createSprite(texturePlayAgainButton, "playagainbutton.png", PLAYAGAINBUTTON_IMG_WIDTH, PLAYAGAINBUTTON_IMG_HEIGHT, PLAYAGAINBUTTON_WIDTH, PLAYAGAINBUTTON_HEIGHT, (window.getSize().x - (float)PLAYAGAINBUTTON_WIDTH) / 2, (window.getSize().y - (float)PLAYAGAINBUTTON_HEIGHT) / 4);
	

	std::vector<sf::Sprite> spritePlatform;
	spritePlatform.push_back(createSprite(texturePlatform, "platform.png", PLATFORM_IMG_WIDTH, PLATFORM_IMG_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, (window.getSize().x - (float)PLATFORM_WIDTH) / 2, window.getSize().y - (float)PLATFORM_HEIGHT));

	generatePlatforms(spritePlatform, texturePlatform, 0);

	// after scaling, remember its original (bottom-middle) position
	doodlerInitPosition.x = spriteDoodler.getPosition().x;
	doodlerInitPosition.y = spriteDoodler.getPosition().y;

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("Sleep.wav"))
		return -1;
	sf::Sound sound;
	sound.setBuffer(buffer);

	bool change = false; //when left or right key pressed, set this flag to true, used to control bounce
	sf::Vector2f doodlerOriginalPosition = spriteDoodler.getPosition(); //remember original position
	int velocity = REBOUND_VELOCITY;
	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
				case sf::Event::Closed: {
					window.close();
					break;     
				}
				case sf::Event::KeyPressed: {
					//move DOODLE_XMOVE pixels when left/right key pressed, warp when reach side of the window
					//set change flag to true so that jump base(x position) is updated
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
					{
						if(intersect(spritePlatform, spriteDoodler,velocity)) {
							sound.play();
						} 
						sf::Vector2f new_pos(spriteDoodler.getGlobalBounds().left - DOODLE_XMOVE, spriteDoodler.getGlobalBounds().top);
						spriteDoodler.setPosition(new_pos);
						warp(spriteDoodler);
						change = true;
					}
					else if(sf::Keyboard::isKeyPressed (sf::Keyboard::Right))
					{
						if(intersect(spritePlatform, spriteDoodler,velocity)) {
							sound.play();
						} 
						sf::Vector2f new_pos(spriteDoodler.getGlobalBounds().left + DOODLE_XMOVE, spriteDoodler.getGlobalBounds().top);
						spriteDoodler.setPosition(new_pos);
						warp(spriteDoodler);
						change = true;
					}
					break;
				}
				
				case sf::Event::MouseMoved: {
					sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
					sf::Vector2f mousePositionFloat((float)mousePosition.x, (float)mousePosition.y);
					if (spritePlayButton.getGlobalBounds().contains(mousePositionFloat))
					{
						//change color
						spritePlayButton.setColor(sf::Color(255, 190, 152));
					}
					else if (!spritePlayButton.getGlobalBounds().contains(mousePositionFloat))
					{
						//reset to original color
						spritePlayButton.setColor(sf::Color(255, 255, 255));
					}

					if (spritePlayAgainButton.getGlobalBounds().contains(mousePositionFloat)) {
						//change color
						spritePlayAgainButton.setColor(sf::Color(255, 190, 152));
					}
					else if(!spritePlayAgainButton.getGlobalBounds().contains(mousePositionFloat))
					{
						//reset to original color
						spritePlayAgainButton.setColor(sf::Color(255, 255, 255));
					}
					break;
				}

				case sf::Event::MouseButtonPressed: {
					sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
					sf::Vector2f mousePositionFloat((float)mousePosition.x, (float)mousePosition.y);
					if (spritePlayButton.getGlobalBounds().contains(mousePositionFloat))
					{
						play = true;
					}
					if (spritePlayAgainButton.getGlobalBounds().contains(mousePositionFloat))
					{
						spriteDoodler.setPosition(doodlerInitPosition.x, doodlerInitPosition.y);
						doodlerOriginalPosition = spriteDoodler.getPosition(); //remember Doodler's original position
						play = true;
						//new game reset the platform
						spritePlatform.clear();
						spritePlatform.push_back(createSprite(texturePlatform, "platform.png", PLATFORM_IMG_WIDTH, PLATFORM_IMG_HEIGHT, PLATFORM_WIDTH, PLATFORM_HEIGHT, (window.getSize().x - (float)PLATFORM_WIDTH) / 2, window.getSize().y - (float)PLATFORM_HEIGHT));
						generatePlatforms(spritePlatform, texturePlatform, 0);
					}
					break;
				}
				default:
					break;
			}

		}
		if (play) {
			jump(spriteDoodler, change, doodlerOriginalPosition, clock, velocity, spritePlatform);
			scroll(spriteDoodler, spritePlatform, texturePlatform, doodlerOriginalPosition);
		}
		window.clear();
		spriteBackground.setColor(sf::Color(255, 255, 255));
		window.draw(spriteBackground);
		//draw only the created platform
		for(int i = 0; i < spritePlatform.size(); i++)
		{
			window.draw(spritePlatform[i]);
		}
		window.draw(spriteDoodler);
		if (!play) { //remove Play button when playing
			window.draw(spritePlayButton);
		}
		if (spriteDoodler.getGlobalBounds().top > FRAME_HEIGHT)
		{
			window.draw(spriteGameOverButton);
			window.draw(spritePlayAgainButton);
			play = false;
		}
		window.display();
	}
	return 0;
}
