#include <SFML/Graphics.hpp>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <Windows.h>
#include <string>

using namespace sf;
using namespace std;

int dir=1,siz = 16,num=1,BestScore=0;
float timer, delay = 0.2f;
int N = 20, M = 20, w = M * siz, h = N * siz;
struct { int x, y; }s[100],f;
bool gameOver = false, supra = false, onetime = false, r = false;
string str;
Vector2i mousepos;

int Update() {
	for (int i = num; i > 0; i--)
		s[i] = s[i - 1];

	if		(dir == 1 ) { s[0].y -= 1; }
	else if (dir == 2 ) { s[0].x += 1; }
	else if (dir == 3 ) { s[0].y += 1; }
	else if (dir == 4 ) { s[0].x -= 1; }

	if (s[0].x == s[2].x && s[0].y == s[2].y) {
		if (dir == 1)dir = 3, s[0].y += 2;
		else if (dir == 2)dir = 4, s[0].x -= 2;
		else if (dir == 3)dir = 1, s[0].y -= 2;
		else dir = 2, s[0].x += 2;
	}

	if (s[0].x == f.x && s[0].y == f.y) {
		num++;
		do {
			supra = false;
			f.x = rand() % N; f.y = rand() % M;
			for (int i = 0; i < num; i++) {
				if (s[i].x == f.x && s[i].y == f.y) {
					supra = true;
					break;
				}
			}
		} while (supra == true);
	}
	if (num - 1 > BestScore)BestScore = num - 1;

	if (s[0].x > N || s[0].x<0 || s[0].y>M || s[0].y < 0) {
		cout << "GAME OVER!\n"; return 1;
	}
	for (int i = 2; i <= num; i++) {
		
		if (s[i].x == s[0].x && s[i].y == s[0].y) {
			cout << "GAME OVER!\n"; return 1;
		}
	}
	return 0;
}

int main() 
{
	srand((unsigned int)time(0));
	
	RenderWindow window(VideoMode(w, h), "Snake Game", Style::Close | Style::Titlebar);

	Vector2f size = { 110.0f, 30.0f }, s2 = { 119.0f,48.0f };
	RectangleShape b(size);
	RectangleShape playbutton(s2);
	
	//Loading textures
	Texture app;	app.loadFromFile("Resources/Images/red.png");
	Texture back;	back.loadFromFile("Resources/Images/white.png");
	Texture snak;	snak.loadFromFile("Resources/Images/green.png");
	Texture repl;	repl.loadFromFile("Resources/Images/Replay.png");
	Font hp; hp.loadFromFile("Resources/Fonts/game-over-cufonfonts/game_over.ttf");
	
	playbutton.setTexture(&repl);

	Text gover,S,nscor,best;
	
	Sprite apple(app),background(back),snake(snak); 

	best.setFont(hp);//Bestscore
	gover.setFont(hp);//Game Over
	S.setFont(hp);
	nscor.setFont(hp);//Increasing Score

	best.setCharacterSize(60);
	gover.setCharacterSize(72); 
	S.setCharacterSize(60);//size of texts
	nscor.setCharacterSize(60);

	gover.setString("GAME OVER!");
	S.setString("Score: ");

	best.setFillColor(Color(0, 76, 153, 255));
	gover.setFillColor(Color(0, 51, 102, 255)); 
	S.setFillColor(Color(255,128,0,255));
	nscor.setFillColor(Color(255,128,0,255));
	b.setFillColor(Color(51, 51, 255, 150));

	gover.setOrigin(85.0f, 100.0f);
	best.setOrigin(75.0f, 55.0f);

	best.setPosition((float)h / 2, (float)w / 2-40 );
	gover.setPosition((float)h / 2, (float)w / 2-40); 
	S.setPosition(0.0f, -38.0f); 
	nscor.setPosition(78.0f, -38.0f);
	b.setPosition(0.0f, 0.0f);
	playbutton.setPosition(98.0f, 160.0f);

	best.setStyle(Text::Bold);
	S.setStyle(Text::Bold);
	nscor.setStyle(Text::Bold);
	
	s[0].x = s[0].y= M / 2;//Initial position of snake 
	
	f.x = f.y = rand() % M;//Initial position of fruit
	
	Clock clock;
	
	HERE:
	
	while (window.isOpen()) 
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;
		Event e;
		while (window.pollEvent(e))
			if (e.type == Event::Closed)
			{
				window.close();
			}
		if (onetime == true && r == false) {
			r = true;
			while (!(Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) ||
				Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D) ||
				Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) ||
				Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)));
		}
		if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) dir = 1;
		if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) dir = 2;
		if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))  dir = 3;
		if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) dir = 4;

		onetime = true;
		
		if (timer >= delay && !gameOver) {
			timer = 0;
			int r=Update();
			if (r == 1) {
				window.clear(Color(192, 192, 192, 100));
				window.draw(gover);
				str = "Bestscore: " + to_string(BestScore);
				best.setString(str);
				window.draw(best);
				window.draw(playbutton);
				window.display();
				gameOver = true;
			}
		}
		
		if (gameOver) {
			if (Mouse::isButtonPressed(Mouse::Button::Left)) {
				mousepos = Mouse::getPosition(window);
				if (mousepos.x >= 98 && mousepos.x <= 217 && mousepos.y >= 160 && mousepos.y <= 208) {
					gameOver = false; num = 1; 
					onetime = false; r = false;
					s[0].x = s[0].y = N / 2;
					goto HERE;
				}
			}
		}

		if (!gameOver) {
			window.clear();
			for (int i = 0; i < M; i++)
				for (int j = 0; j < N; j++)
				{
					background.setPosition(float(i * siz), float(j * siz)); window.draw(background);
				}
			for (int i = 0; i < num; i++) {
				snake.setPosition(float(s[i].x * siz), float(s[i].y * siz)); window.draw(snake);
			}
			apple.setPosition(float(f.x * siz), float(f.y * siz)); window.draw(apple);
			window.draw(b);
			window.draw(S);
			str = to_string(num - 1);
			nscor.setString(str);
			window.draw(nscor);
			window.display();
		}
		
	}

	return 0;
}