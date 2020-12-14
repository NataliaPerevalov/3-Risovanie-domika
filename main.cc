/*
 * main.cc
 *
 *  Created on: 12 окт. 2020 г.
 *      Author: unyuu
 */

/* Задание:
 * 1. Используя функции SDL (SDL_RenderDraw... и SDL_RenderFill...) нарисовать:
 * 		- поверхность земли
 * 		- траву
 * 		- дом с крышей, окном, дверью и трубой
 * 		- на заднем плане - забор (опционально)
 *
 * 2. Реализовать рисование окружности с использованием алгоритма Брезенхэма
 *
 * 3. Нарисовать у домика круглое окно на чердаке.
 *
 *
 * Пакет с SDL2 для MSYS2:
 * 		mingw-w64-x86_64-SDL2		- 64-битный
 * 		mingw-w64-i686-SDL2			- 32-битный
 */


#include <iostream>

#include <memory>
/*
 * std::shared_ptr - разделяемый указатель
 * std::unique_ptr - уникальный указатель
 * std::weak_ptr - "слабый" указатель
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>

#include <cmath>
constexpr double Pi = acos(-1.);

constexpr int WIDTH = 1280, HEIGHT = 720;

constexpr int cat1_x = 1386/6, cat1_y = 1280/6;//киса
int cat1_x0 = 20, cat1_y0 = HEIGHT - cat1_y;

int bird_x0 = 0, bird_y0 = 0;
int bird_dx = 4, bird_dy = 2;

void DrawCircle(SDL_Renderer * ren, int cx, int cy, int r)
{
	// тут алгоритм Брезенхэма
	int x = 0;
	int y = r;
	int delta = 1 - 2 * r;
	int error = 0;
	while (y >= 0){
		SDL_RenderDrawPoint(ren, cx + x, cy + y);
		SDL_RenderDrawPoint(ren, cx + x, cy - y);
		SDL_RenderDrawPoint(ren, cx - x, cy + y);
		SDL_RenderDrawPoint(ren, cx - x, cy - y);
		error = 2 * (delta + y) - 1;
		if ((delta < 0) && (error <= 0))
			delta += 2* ++x +1;
		else if ((delta > 0) && (error > 0))
			delta -= 2* --y + 1;
		else
			delta += 2 * (++x - --y);
	}
}
void DrawGrass(SDL_Renderer * ren, int number, int x, int y, int height[]){
	SDL_SetRenderDrawColor(ren, 64, 92, 52, 255);
	for (int i = 0; i < number; i++){
		for (int j = 0; j < height[i]; j++){
			SDL_RenderDrawLine(ren, x + height[i] / 2 - j, y + j, x + height[i] / 2 + j, y + j);
		}
		x += height[i];
	}
}

int main(int, char **){
	std::cout << "Начало работы программы" << std::endl;

	SDL_Init(SDL_INIT_EVERYTHING);

	auto window = std::shared_ptr<SDL_Window>(
			SDL_CreateWindow("Это окно нашего приложения",
					SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					WIDTH, HEIGHT,
					SDL_WINDOW_SHOWN),
			SDL_DestroyWindow);
	if (window == nullptr) {
		std::cerr << "Ошибка при создании окна: " <<
				SDL_GetError() << std::endl;
		return 1;
	}

	auto ren = std::shared_ptr<SDL_Renderer>(
			SDL_CreateRenderer(window.get(), -1,
					SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
			SDL_DestroyRenderer);
	if (ren == nullptr) {
		std::cerr << "Ошибка при создании рендерера: " <<
				SDL_GetError() << std::endl;
		return 1;
	}

	
	auto cat1_icon = std::shared_ptr <SDL_Texture>(IMG_LoadTexture(ren.get(), "cat.png"), SDL_DestroyTexture);
	if (cat1_icon == nullptr){
		std::cerr << "Не могу загрузить картинку: " <<
				SDL_GetError() <<std::endl;
		return 1;
	}

	auto cat2_icon = std::shared_ptr <SDL_Texture>(IMG_LoadTexture(ren.get(), "sleep.png"), SDL_DestroyTexture);
	if (cat2_icon == nullptr){
			std::cerr << "Не могу загрузить картинку: " <<
					SDL_GetError() <<std::endl;
			return 1;
		}

	auto bird_icon = std::shared_ptr <SDL_Texture>(IMG_LoadTexture(ren.get(), "bird.png"), SDL_DestroyTexture);
	if (bird_icon == nullptr){
		std::cerr << "Не могу загрузить картинку: " <<
				SDL_GetError() <<std::endl;
		return 1;
	}

	auto keys = SDL_GetKeyboardState(nullptr);// ключи
	SDL_Event event;
	bool finish = false;
	// Основной цикл программы
	while (not finish) {
		// Цикл обработки событий
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) // Если окно закрыли...
				finish = true;
		}


				SDL_SetRenderDrawColor(ren.get(), 4, 165, 255, 255);
				SDL_RenderClear(ren.get());

				SDL_Rect grass { 0, 330, WIDTH, HEIGHT-330};
				SDL_SetRenderDrawColor(ren.get(), 66, 214, 98, 255);
				SDL_RenderFillRect(ren.get(), &grass);


				constexpr int zabor_x = 60;// забор
				constexpr int zabor_y1 = 150; // высота забора
				constexpr int zabor_y0 = HEIGHT / 2 -zabor_y1 / 2;
				constexpr int dist = zabor_x + 10;
				SDL_SetRenderDrawColor(ren.get(), 179, 95, 0, 255);
				for (int i = 0; i < WIDTH; i += dist){
					SDL_Rect zabor { i, zabor_y0, zabor_x, zabor_y1};
					SDL_RenderFillRect(ren.get(), &zabor);
					for (int j = 0; j < zabor_x/2; j++)
						SDL_RenderDrawLine(ren.get(), zabor_x /2 - j + i, zabor_y0 - zabor_x /2 + j, zabor_x/2 + j + i, zabor_y0 - zabor_x/2 + j);
				}

				constexpr int home_x = 500;	//дом
				constexpr int home_y = 300;
				SDL_Rect home { WIDTH/2-home_x/2, HEIGHT/2 - home_y / 2, home_x, home_y};
				SDL_SetRenderDrawColor(ren.get(), 194, 97, 58, 255);
				SDL_RenderFillRect(ren.get(), &home);


				constexpr int door_x = 150;//двери
				constexpr int door_y = 200;
				constexpr int door_x0 = WIDTH/2 - door_x * 1.5;
				constexpr int door_y0 = HEIGHT/2 + home_y / 2 - door_y;
				SDL_SetRenderDrawColor(ren.get(), 115, 57, 34, 255);
				SDL_Rect door { door_x0, door_y0, door_x, door_y};
				SDL_RenderFillRect(ren.get(), &door);

				SDL_SetRenderDrawColor(ren.get(), 0, 0, 0, 50);//ручка
				constexpr int line = 10;
				SDL_RenderDrawLine(ren.get(), door_x0 + line, door_y0 + door_y / 2 - line/2,
												door_x0 + line, door_y0 + door_y / 2 + line/2);


				constexpr int win_x = int(home_x / 3);//окно
				constexpr int win_y = int(home_y * 0.8);
				constexpr int win_x0 = WIDTH/2 + 20;
				constexpr int win_y0 = HEIGHT / 2 - home_y * 0.35;
				SDL_SetRenderDrawColor(ren.get(), 186, 255, 249, 255);
				SDL_Rect win{ win_x0, win_y0, win_x, win_y};
				SDL_RenderFillRect(ren.get(), &win);

				SDL_SetRenderDrawColor(ren.get(), 0, 0, 0, 255);
				SDL_RenderDrawLine(ren.get(), win_x0, win_y0 + win_y/2,//линия гориз
									win_x0 + win_x, win_y0 + win_y/2);
				SDL_RenderDrawLine(ren.get(), win_x0 + win_x/2, win_y0,//линия вертик
									win_x0 + win_x/2, win_y0 + win_y);


				constexpr int roof_size = 150;//крыша
				SDL_SetRenderDrawColor(ren.get(),69, 34, 21, 255);
				for (int i = 0; i< roof_size; i++)
					SDL_RenderDrawLine(ren.get(), WIDTH/2-i*2, HEIGHT/2 - home_y / 2 - roof_size + i,
												WIDTH/2+i*2,  HEIGHT/2- home_y / 2 - roof_size + i);


				SDL_SetRenderDrawColor(ren.get(), 186, 255, 249, 255);//окно на крыше
				constexpr int smallWin = 30;
				for (int i = smallWin; i>0; i--)
					DrawCircle(ren.get(), WIDTH/2, HEIGHT/2 - home_y / 2 - roof_size / 2, i);
				SDL_SetRenderDrawColor(ren.get(), 0, 0, 0, 255);
				SDL_RenderDrawLine(ren.get(),WIDTH/2, HEIGHT/2 - home_y / 2 - roof_size / 2 - smallWin,
												WIDTH/2, HEIGHT/2 - home_y / 2 - roof_size / 2 + smallWin);//вертик
				SDL_RenderDrawLine(ren.get(),WIDTH/2 - smallWin, HEIGHT/2 - home_y / 2 - roof_size / 2 ,
											WIDTH/2 + smallWin, HEIGHT/2 - home_y / 2 - roof_size / 2 );//гориз


				SDL_SetRenderDrawColor(ren.get(), 64, 92, 52, 255);//трава
				int h1[] = {5, 15, 13, 3};
				DrawGrass(ren.get(),4, 10, HEIGHT - 150, h1);
				int h2[] = {10, 6, 13};
				DrawGrass(ren.get(),3, WIDTH - 100, HEIGHT - 100, h2);
				int h3[] = {12, 7, 5, 10, 4};
				DrawGrass(ren.get(),3, WIDTH - 150, HEIGHT - 200, h3);

				constexpr int leftBorderW = WIDTH/2-home_x/2-cat1_x/2;
				constexpr int rightBorderW = WIDTH/2+home_x/2;
				constexpr int zaborBorder = HEIGHT/2;
				constexpr int upB = HEIGHT/2;
				constexpr int pace = 8;


				if ((keys[SDL_SCANCODE_RIGHT] and ((cat1_x0 <= WIDTH - cat1_x and cat1_x0 >= rightBorderW)
						or cat1_x0 <= leftBorderW
						or (cat1_x0 <= WIDTH - cat1_x and cat1_y0 >= upB)))){
					cat1_x0 += pace;
				}
				if (keys[SDL_SCANCODE_LEFT]and ((cat1_x0 >= 0 and cat1_x0 <= leftBorderW)
						or cat1_x0 >= rightBorderW
						or (cat1_x0 >= 0 and cat1_y0 >= upB))){
					cat1_x0 -= pace;
				}
				if (keys[SDL_SCANCODE_UP] and (cat1_y0 >= upB+pace
						or (cat1_x0 <= leftBorderW and cat1_y0 >= zaborBorder)
						or (cat1_x0 >= rightBorderW and cat1_y0 >= zaborBorder))){
					cat1_y0 -= pace;
				}
				if (keys[SDL_SCANCODE_DOWN] and
						(cat1_y0 <= HEIGHT-cat1_y*.66)){
					cat1_y0 += pace;
				}

				SDL_Rect cat1{cat1_x0, cat1_y0,cat1_x, cat1_y};
				SDL_RenderCopy(ren.get(), cat1_icon.get(), nullptr, &cat1);


				constexpr int bird_x = 600 / 3, bird_y = 348 / 3; //птица

				bird_x0 += bird_dx;
				bird_y0 += bird_dy;
				if(bird_x0 >= WIDTH - bird_x or bird_x0 < 0)
					bird_dx = -bird_dx;
				if(bird_y0 >= HEIGHT/2 or bird_y0 < 0)
					bird_dy = -bird_dy;

				SDL_Rect bird{bird_x0, bird_y0, bird_x, bird_y};
				SDL_RenderCopy(ren.get(), bird_icon.get(),	nullptr, &bird);


				constexpr int cat2_x = 300, cat2_y = 194;//спящий
				constexpr int cat2_x0 = WIDTH - cat2_x - 100, cat2_y0 = HEIGHT - cat2_y;
				SDL_Rect cat2{cat2_x0, cat2_y0, cat2_x, cat2_y};
				SDL_RenderCopy(ren.get(), cat2_icon.get(), nullptr, &cat2);

				/*
				 * Двойная буферизация:
				 * 1 буфер (передний) - буфер, содержимое которого отображается.
				 * 2 буфер (задний) - буфер, в котором осуществляется построение изображения.
				 *
				 */
				SDL_RenderPresent(ren.get());
			}


	std::cout << "Конец работы программы" << std::endl;
	return 0;

}
