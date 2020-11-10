#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#define ROWS 24
#define COL 32
#define CUBE_RESIZE 0.2f
#define CUBE_SIZE_PX 40
#define CUBE_MENIU_RESIZE 0.5f
#define CUBE_MENU_SIZE_PX 80
#define DIRT 0
#define GRASS 1
#define WATER 2
#define BRICK 3
#define TREE 4
#define CHEST 5
#define MAX_LEVELS 17

using namespace std;
using namespace sf;

int width = ROWS * CUBE_SIZE_PX;
int height = COL * CUBE_SIZE_PX;

int selected_cube = 0;
int level_selected = 0;
int number_of_levels = 0;

bool in_main_menu = true;
bool is_loading_level = false;
bool was_load_new_one = true;
bool changes_made = true;
bool is_drawing = false;
bool should_clear_screen = false;
bool new_loaded = true;

/*
	0-dirt
	1-grass
	2-water
	3-brick
	4-tree
	5-chest
*/

RenderWindow window(VideoMode(height, width), "Level Editor", Style::Close | Style::Titlebar);

// Texture and Sprites
Texture dirt_texture, grass_texture, tree_texture, water_texture, brick_texture, chest_texture;
Sprite dirt_sprite, grass_sprite, tree_sprite, water_sprite, brick_sprite, chest_sprite;
Sprite dirt_menu, grass_menu, tree_menu, water_menu, brick_menu, chest_menu;

class Block
{
public:
	// the coordinates of the upper corner and lower corner and the value of block
	float xs, ys, xj, yj, value;

	// setting the coordinates of corners and the value of block
	void Set(float x, float y, int val)
	{
		ys = x;
		xs = y;
		xj = x + CUBE_MENU_SIZE_PX + 20;
		yj = y + CUBE_MENU_SIZE_PX + 45;
		value = val;
	}
};

// Matrix or (2d array) of objects
Block brd[ROWS][COL];

class Board
{
private:
	Vector2f size;
	int rows, columns, cube_size;

	void load_textures()
	{
		// loading textures
		dirt_texture.loadFromFile("Resources/ground.jpg");
		dirt_sprite.setTexture(dirt_texture);
		dirt_menu.setTexture(dirt_texture);
		dirt_sprite.scale(size);

		grass_texture.loadFromFile("Resources/grass.jpg");
		grass_sprite.setTexture(grass_texture);
		grass_menu.setTexture(grass_texture);
		grass_sprite.scale(size);

		tree_texture.loadFromFile("Resources/tree.jpg");
		tree_sprite.setTexture(tree_texture);
		tree_menu.setTexture(tree_texture);
		tree_sprite.scale(size);

		water_texture.loadFromFile("Resources/water.jpg");
		water_sprite.setTexture(water_texture);
		water_menu.setTexture(water_texture);
		water_sprite.scale(size);

		brick_texture.loadFromFile("Resources/brick.jpg");
		brick_sprite.setTexture(brick_texture);
		brick_menu.setTexture(brick_texture);
		brick_sprite.scale(size);

		chest_texture.loadFromFile("Resources/chest.jpg");
		chest_sprite.setTexture(chest_texture);
		chest_menu.setTexture(chest_texture);
		chest_sprite.scale(size);

	}
public:

	Board(int no_of_rows, int no_of_columns, int c_size)
	{
		size.x = CUBE_RESIZE;
		size.y = CUBE_RESIZE;
		load_textures();

		cube_size = c_size;
		rows = no_of_rows;
		columns = no_of_columns;

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
				brd[i][j].Set(i * cube_size, j * cube_size, DIRT);
	}

	void draw()
	{
		window.clear();

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
			{
				if (should_clear_screen)
				{
					changes_made = true;
					brd[i][j].value = DIRT;
				}

				if (brd[i][j].value == (int)DIRT)
				{
					dirt_sprite.setPosition(brd[i][j].xs, brd[i][j].ys);
					window.draw(dirt_sprite);
				}
				else if (brd[i][j].value == (int)GRASS)
				{
					grass_sprite.setPosition(brd[i][j].xs, brd[i][j].ys);
					window.draw(grass_sprite);
				}
				else if (brd[i][j].value == (int)WATER)
				{
					water_sprite.setPosition(brd[i][j].xs, brd[i][j].ys);
					window.draw(water_sprite);
				}
				else if (brd[i][j].value == (int)BRICK)
				{
					brick_sprite.setPosition(brd[i][j].xs, brd[i][j].ys);
					window.draw(brick_sprite);
				}
				else if (brd[i][j].value == (int)TREE)
				{
					tree_sprite.setPosition(brd[i][j].xs, brd[i][j].ys);

					window.draw(tree_sprite);
				}
				else if (brd[i][j].value == (int)CHEST)
				{
					chest_sprite.setPosition(brd[i][j].xs, brd[i][j].ys);
					window.draw(chest_sprite);
				}
			}
		should_clear_screen = false;
	}

	void draw_on_screen()
	{
		Vector2i m_pos = Mouse::getPosition(window);

		int c_x, c_y;

		c_x = m_pos.y / CUBE_SIZE_PX;
		c_y = m_pos.x / CUBE_SIZE_PX;

		brd[c_x][c_y].value = selected_cube;
	}
};

class Menu
{
private:
	// Auxiliary variables and data structures

	Block select_block[6];
	int h = 4 * CUBE_SIZE_PX;
	Vector2f size = { h, width };
	Vector2f l_position = { 0, 0 };
	Vector2f r_position = { height - size.x , 0 };

	Vector2f block_size = { CUBE_MENIU_RESIZE, CUBE_MENIU_RESIZE };

	const int letter_h = 20;
	const int lenght_main_words = 40;

	struct {
		int xs, ys;
		char option[20];
	}o_buttons[5], main_buttons[5];

	Font font;
	Text text;

	int x_block, x_buttons = 65, y_buttons = 27;

	RectangleShape l_rect;
	RectangleShape r_rect;
public:

	Menu()
	{
		strcpy(o_buttons[0].option, "Clear");
		strcpy(o_buttons[1].option, "Load");
		strcpy(o_buttons[2].option, "Load New");
		strcpy(o_buttons[3].option, "Save");
		strcpy(o_buttons[4].option, "Exit");

		strcpy(main_buttons[0].option, "Load Level");
		strcpy(main_buttons[1].option, "Level Editor");
		strcpy(main_buttons[2].option, "Delete levels");
		strcpy(main_buttons[3].option, "Exit");

		int d_between = 70, xstd = 500, ystart = 300;

		for (int i = 0; i < 4; i++)
		{
			main_buttons[i].xs = xstd;
			main_buttons[i].ys = i * d_between + ystart;
		}

		int y1 = 350, distance = 60, x1 = 40;

		for (int i = 0; i < 5; i++)
		{
			o_buttons[i].xs = x1;
			o_buttons[i].ys = y1 + i * distance;
		}

		font.loadFromFile("Resources/Fonts/Roboto-Black.ttf");
		text.setFont(font);
		text.setCharacterSize(24);
		text.setFillColor(Color::White);

		l_rect.setSize(size);
		l_rect.setFillColor(Color(0, 0, 0, 200));
		l_rect.setPosition(l_position);

		r_rect.setSize(size);
		r_rect.setFillColor(Color(0, 0, 0, 200));
		r_rect.setPosition(r_position);


		x_block = (height - size.x) + (h / 2) - (CUBE_MENU_SIZE_PX / 2) - 4;

		for (int i = 0; i < 6; i++)
		{
			select_block[i].Set(x_block, ((width / 6) * i + (width / 6 - CUBE_MENU_SIZE_PX) / 2 - 20), i);
			select_block[i].xs = x_block;
			select_block[i].ys = (width / 6) * i + (width / 6 - CUBE_MENU_SIZE_PX) / 2 - 20;
			select_block[i].value = i;
		}


		dirt_menu.scale(block_size);
		grass_menu.scale(block_size);
		tree_menu.scale(block_size);
		water_menu.scale(block_size);
		brick_menu.scale(block_size);
		chest_menu.scale(block_size);
	}

	bool is_on_mouse_pos(int xs, int ys, int xj, int yj)
	{
		Vector2i pos = Mouse::getPosition(window);
		if (xs <= pos.x && xj >= pos.x && ys <= pos.y && yj >= pos.y)
			return true;
		return false;
	}

	void draw_main_menu()
	{
		window.clear();
		text.setString("Main");
		text.setCharacterSize(72);
		text.setPosition(30, 20);
		text.setFillColor(Color::White);
		window.draw(text);

		text.setCharacterSize(40);

		for (int i = 0; i < 4; i++)
		{
			text.setString(main_buttons[i].option);

			if (i == 0)
				text.setPosition(main_buttons[i].xs, main_buttons[i].ys);
			else if (i == 1)
				text.setPosition(main_buttons[i].xs - 10, main_buttons[i].ys);
			else if (i == 3)
				text.setPosition(main_buttons[i].xs + 60, main_buttons[i].ys);
			else
				text.setPosition(main_buttons[i].xs - 15, main_buttons[i].ys);

			FloatRect rect = text.getGlobalBounds();

			if (is_on_mouse_pos(rect.left, rect.top, rect.left + rect.width, rect.top + rect.height))
			{
				text.setFillColor(Color(150, 150, 150, 255));

				if (Mouse::isButtonPressed(Mouse::Button::Left))
				{
					if (i == 0 && number_of_levels > 0)
					{
						level_selected = -1;
						in_main_menu = false;
						is_loading_level = true;
						break;
					}
					else if (i == 1)
					{
						level_selected = number_of_levels + 1;
						in_main_menu = false;
						break;
					}
					else if (i == 3)
					{
						window.close();
					}
					else
					{
						ofstream fout("Resources/Levels/no_levels.txt");
						fout << 0;
						fout.close();
					}
				}

			}
			else
				text.setFillColor(Color::White);

			window.draw(text);
		}
	}

	void Load_level()
	{
		if (number_of_levels < 1)
			return;

		window.clear();
		text.setString("Load Level");
		text.setFillColor(Color::White);
		text.setCharacterSize(60);
		text.setPosition(20, 10);
		window.draw(text);

		text.setCharacterSize(35);
		int x_str = height / 2 - 30, y_str = 70, dst = 50, l_nr = 20;

		for (int i = 1; i <= number_of_levels; i++)
		{
			if (i == 10)
				l_nr += 30;
			text.setString(to_string(i));
			text.setPosition(x_str, y_str + (i - 1) * dst);

			if (is_on_mouse_pos(x_str, y_str + (i - 1) * dst, x_str + l_nr, y_str + (i - 1) * dst + 40))
			{
				text.setFillColor(Color(150, 150, 150, 255));
				if (Mouse::isButtonPressed(Mouse::Button::Left))
				{
					level_selected = i;
					is_loading_level = false;
				}
			}
			else
				text.setFillColor(Color::White);

			window.draw(text);
		}

		if (!is_loading_level)
		{
			char aux[50] = "Resources/Levels/";
			char lvl[50] = "";
			if (level_selected < 10)
			{
				lvl[0] = '0' + level_selected;
				lvl[1] = 0;
			}
			else
			{
				lvl[0] = '0' + level_selected / 10;
				lvl[1] = '0' + level_selected % 10;
				lvl[2] = 0;
			}

			strcat(lvl, ".txt");
			strcat(aux, lvl);

			ifstream fin(aux);
			int rt;

			for (int i = 0; i < ROWS; i++)
				for (int j = 0; j < COL; j++)
				{
					fin >> rt;
					brd[i][j].value = rt;
				}
			fin.close();
		}
	}

	void Save_level()
	{
		if (!changes_made)
			return;

		char aux[50] = "Resources/Levels/";
		char lvl[50] = "";

		if (level_selected > MAX_LEVELS)
			level_selected = MAX_LEVELS;

		if (level_selected < 10)
		{
			lvl[0] = '0' + level_selected;
			lvl[1] = 0;
		}
		else
		{
			lvl[0] = '0' + level_selected / 10;
			lvl[1] = level_selected % 10;
			lvl[2] = 0;
		}

		strcat(lvl, ".txt");
		strcat(aux, lvl);

		ofstream fout(aux);

		cout << "Saving...\n";

		for (int i = 0; i < ROWS; i++)
		{
			for (int j = 0; j < COL; j++)
				fout << brd[i][j].value << ' ';
			fout << '\n';
		}
		fout.close();

		ofstream fout2("Resources/Levels/no_levels.txt");
		if (level_selected <= number_of_levels)
			fout2 << number_of_levels;
		else
			fout2 << level_selected;
		fout2.close();

		changes_made = false;
	}

	void draw_menu()
	{
		text.setCharacterSize(24);
		window.draw(l_rect);
		window.draw(r_rect);

		for (int i = 0; i < 5; i++)
		{
			text.setString(o_buttons[i].option);
			text.setPosition(o_buttons[i].xs, o_buttons[i].ys);

			FloatRect rct = text.getGlobalBounds();

			if (is_on_mouse_pos(rct.left, rct.top, rct.left + rct.width, rct.top + rct.height))
			{
				text.setFillColor(Color(150, 150, 150, 255));

				if (Mouse::isButtonPressed(Mouse::Button::Left))
				{
					if (i == 0)
						should_clear_screen = true;

					else if (i == 1 && number_of_levels > 0)
						is_loading_level = true;

					else if (i == 2 && !new_loaded)
					{
						Save_level();
						level_selected++;
						should_clear_screen = true;
						new_loaded = true;
					}
					else if (i == 3)
						Save_level();

					else if (i == 4)
						window.close();

				}

			}
			else
				text.setFillColor(Color::White);

			window.draw(text);
		}

		for (int i = 0; i < 6; i++)
		{
			int xxs = select_block[i].xs + 20, yys = select_block[i].ys + CUBE_MENU_SIZE_PX + 20;
			text.setPosition(xxs, yys);

			if (is_on_mouse_pos(select_block[i].xs, select_block[i].ys, select_block[i].xj, select_block[i].yj)
				&& Mouse::isButtonPressed(Mouse::Button::Left))
				selected_cube = i;

			if (is_on_mouse_pos(xxs, yys, xxs + 70, yys + 30))
			{
				text.setFillColor(Color(150, 150, 150, 255));
			}
			else
				text.setFillColor(Color::White);

			if (i == selected_cube)
				text.setFillColor(Color(1, 102, 204, 255));

			if (select_block[i].value == DIRT)
			{
				dirt_menu.setPosition(select_block[i].xs, select_block[i].ys);
				text.setString("Dirt");
				window.draw(dirt_menu);
			}
			else if (select_block[i].value == GRASS)
			{
				grass_menu.setPosition(select_block[i].xs, select_block[i].ys);
				text.setString("Grass");
				window.draw(grass_menu);
			}
			else if (select_block[i].value == WATER)
			{
				water_menu.setPosition(select_block[i].xs, select_block[i].ys);
				text.setString("Water");
				window.draw(water_menu);
			}
			else if (select_block[i].value == BRICK)
			{
				brick_menu.setPosition(select_block[i].xs, select_block[i].ys);
				text.setString("Brick");
				window.draw(brick_menu);
			}
			else if (select_block[i].value == TREE)
			{
				tree_menu.setPosition(select_block[i].xs, select_block[i].ys);
				text.setString("Tree");
				window.draw(tree_menu);
			}
			else if (select_block[i].value == CHEST)
			{
				chest_menu.setPosition(select_block[i].xs, select_block[i].ys);
				text.setString("Chest");
				window.draw(chest_menu);
			}

			window.draw(text);
		}

	}
};

int main()
{
	int cube_s = 40;
	Board board(ROWS, COL, cube_s);
	Menu menu;

	bool in_menu = false;

	if (number_of_levels > MAX_LEVELS)
		number_of_levels = MAX_LEVELS;

	while (window.isOpen())
	{
		ifstream fin("Resources/Levels/no_levels.txt");
		fin >> number_of_levels;
		fin.close();

		Event evnt;
		while (window.pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
				window.close();
			else if (evnt.type == Event::KeyPressed)
			{
				if (evnt.key.code == Keyboard::Escape && !in_main_menu)
					in_menu = !in_menu;
			}
			else if (evnt.type == Event::MouseButtonPressed)
			{
				if (evnt.mouseButton.button == Mouse::Button::Left && !in_main_menu && !in_menu && !is_loading_level)
				{
					changes_made = true;
					is_drawing = true;
					new_loaded = false;
				}
			}
			else if (evnt.type == Event::MouseButtonReleased)
				is_drawing = false;
		}

		if (in_main_menu)
			menu.draw_main_menu();

		else if (is_loading_level && number_of_levels > 0)
			menu.Load_level();

		else
		{
			if (is_drawing)
				board.draw_on_screen();

			board.draw();
			if (in_menu)
				menu.draw_menu();
		}

		window.display();
	}

}
