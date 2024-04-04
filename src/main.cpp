#include <raylib.h>
#include <iostream>
#include <deque>
#include <raymath.h>

using namespace std;

Color color_1 = {155,188,15,255};
Color color_2 = {139,172,15,255};
Color color_3 = {48,98,48, 255};
Color color_4 = {15,56,15 ,255};


int cellSize = 30;
int foodSize = 15;
int bodySize = 28;
int innerWallsSize = 18;
int cellCount = 25;

// Initialization
int screenWidth = 810;
int screenHeight = 810;
int verticalOffset = 60;
int frameStep = 15;
int frameCount = 0;
int paletteNumber = 1;
bool game_over = false;
bool inputReceived = false;
Color textColor(LIGHTGRAY);

bool ElementInDeque(Vector2 element, deque<Vector2> deque){
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

class Palettes{
  public:
    void Draw(){
      DrawText(TextFormat("Palette:"),60, 810, 20, color_4);
      DrawText(TextFormat("(keyboard numbers)"),450, 810, 12, color_4);
      DrawRectangle(160+((paletteNumber-1)*40),800,30,30,color_3);
      for(int i = 1; i<=7; i++){
        DrawRectangleLinesEx((Rectangle){(float) 160+((i-1)*40), (float) 800, (float) 30, (float) 30}, 3, color_4);
        DrawText(TextFormat("%d",i),172+((i-1)*40), (float) 810, 12, color_4);
      }
    }

    void UpdatePalettes(int paletteNewNumber){
      paletteNumber = paletteNewNumber;
      switch (paletteNumber)
      {
      case 1:
        color_1 = {155,188,15,255};
        color_2 = {139,172,15,255};
        color_3 = {48,98,48, 255};
        color_4 = {15,56,15 ,255};
        break; 
      case 2:
        color_4 = {250, 112, 112,255};
        color_1 = {254, 253, 237,255};
        color_2 = {198, 235, 197, 255};
        color_3 = {161, 195, 152,255};
        break;
      case 3:
        color_1 = {219,175,160,255};
        color_2 = {187,132,147,255};
        color_3 = {112,66,100, 255};
        color_4 = {73,36,62,255};
        break;
      case 4:
        color_1 = {220, 242, 241, 255};
        color_2 = {127, 199, 217, 255};
        color_3 = {54, 84, 134, 255};
        color_4 = {15, 16, 53, 255};
        break;
      case 5:
        color_1 = {9, 38, 53, 255};
        color_2 = {27, 66, 66, 255};
        color_3 = {92, 131, 116, 255};
        color_4 = {158, 200, 185, 255};
        break;
      case 6:
        color_1 = {79, 74, 69, 255};
        color_2 = {108, 95, 91, 255};
        color_4 = {237, 125, 49, 255};
        color_3 = {246, 241, 238, 255};
        break;
      case 7:
        color_1 = {255, 128, 128, 255};
        color_2 = {255, 207, 150, 255};
        color_3 = {205, 250, 219, 255};
        color_4 = {246, 253, 195, 255};
        break;
      }
    }
};

class Borders{
  public:
    void Draw(){
      DrawRectangleLinesEx((Rectangle){(float) 60, (float) 60, (float) screenWidth-120, (float) screenHeight-120}, 3,color_4);
    }
};

class Score{
  public:
    int score = 0;
    void Draw(){
      DrawText(TextFormat("Score: %d",score),60, screenHeight-40, 20, color_4);
    }
};

class Walls{
  public:
    deque<Vector2> positions = {};
    Walls(){
      for(int i=6; (i+6)*cellSize<screenWidth; i++){
        positions.push_back(Vector2{(float)i,6});
        positions.push_back(Vector2{(float)i,20});
      }
    }

    void Draw(){
      for(int i = 0; i < (int) positions.size(); i++){
        int x = positions[i].x;
        int y = positions[i].y;
        DrawRectangleLinesEx((Rectangle){(float) x*cellSize + (cellSize-bodySize)/2, (float) y*cellSize + (cellSize-bodySize)/2, (float) bodySize, (float) bodySize}, 5,color_4);
        DrawRectangleRec((Rectangle){(float) x*cellSize + (cellSize-innerWallsSize)/2, (float) y*cellSize + (cellSize-innerWallsSize)/2, (float) innerWallsSize, (float) innerWallsSize},color_2);
      }
    }
};

class Food {
  public:
    Vector2 position;
    int frameCount;
    bool blinking;

    Food(deque<Vector2> snakeBody, deque<Vector2> walls){
      position = GenerateRandomPosition(snakeBody, walls);
      frameCount = 0;
      blinking = false;
    }
    
    void Draw(){
      if(blinking == false){
        DrawRectangle(position.x * cellSize + (cellSize-foodSize)/2.0, position.y*cellSize + (cellSize-foodSize)/2, foodSize, foodSize, color_4);
      }
      if(frameCount>=30 && frameCount<=40){
        blinking = true;
      }
      if(frameCount>40){
        frameCount = 0;
        blinking = false;
      }
      frameCount++;
    }


    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(2, cellCount - 4);
        float y = GetRandomValue(2, cellCount - 4);
        return Vector2{x, y};
    } 

    Vector2 GenerateRandomPosition(deque<Vector2> snakeBody, deque<Vector2> walls){
      Vector2 position = GenerateRandomCell();
      while(ElementInDeque(position,snakeBody) || ElementInDeque(position,walls)){
        position = GenerateRandomCell();
      }
      return position;
    }
};

class Snake {
  public:
    
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    Vector2 direction = {1,0};
    Vector2 tail_pos = {body[body.size()-1].x, body[body.size()-1].y};

    void Draw(){
      for(int i = 0; i < (int) body.size(); i++){
        int x = body[i].x;
        int y = body[i].y;
        DrawRectangleRounded((Rectangle){(float) x*cellSize + (cellSize-bodySize)/2, (float) y*cellSize + (cellSize-bodySize)/2, (float) bodySize, (float) bodySize}, 0.5, 1, color_4);
      }
    }
    
    void Update(){
      
      tail_pos = {body[body.size()-1].x, body[body.size()-1].y};

      int last_x = body[0].x; int last_y = body[0].y;
      body[0].x += direction.x; body[0].y += direction.y;
      for(int i=1; i< (int) body.size(); i++){
        int actual_x = body[i].x; int actual_y = body[i].y;
        body[i].x = last_x; body[i].y = last_y;
        last_x = actual_x; last_y = actual_y;
      }
      inputReceived = false;
    }

    void Grow(){
      body.push_back(tail_pos);
    }

    void UpdateDir(Vector2 dir){
      direction = dir;
    }

    void Restart(){
      body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
      direction = {1,0};
      tail_pos = {body[body.size()-1].x, body[body.size()-1].y};
    }
};  

class Game {
  public:

    Snake snake = Snake();
    Walls walls = Walls();
    Food food = Food(snake.body, walls.positions);
    Score score = Score();
    Borders border = Borders();
    Palettes palette = Palettes();
    
    void Draw(){
      food.Draw();
      snake.Draw();
      walls.Draw();
      border.Draw();
      score.Draw();
      palette.Draw();
    }

    void Update(){
      snake.Update();
    }

    void Restart(){
      snake.Restart();
      food.position = food.GenerateRandomPosition(snake.body,walls.positions);
    }
};

int main() {
    
    InitWindow(screenWidth, screenHeight+verticalOffset, "Snake");
    SetTargetFPS(60);

    Game game = Game();

    // Main game loop
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(color_1);
        
        DrawText("Snake",screenWidth/2-75, 8, 50, color_4); 

        if(game_over == true){
          DrawText("Game Over",screenWidth/2-75, screenHeight/2-30, 36, color_4); 
        }

        if(inputReceived == false){
          if (game.snake.direction.x == 0){
            if (IsKeyPressed(KEY_D)) {game.snake.UpdateDir({1,0}); inputReceived =true;}
            if (IsKeyPressed(KEY_A)) {game.snake.UpdateDir({-1,0}); inputReceived =true;}
          }
          else{
            if (IsKeyPressed(KEY_W)) {game.snake.UpdateDir({0,-1}); inputReceived =true;}
            if (IsKeyPressed(KEY_S)) {game.snake.UpdateDir({0,1}); inputReceived =true;}
          }
        }

        if(IsKeyPressed(KEY_ONE)) game.palette.UpdatePalettes(1);
        if(IsKeyPressed(KEY_TWO)) game.palette.UpdatePalettes(2);
        if(IsKeyPressed(KEY_THREE)) game.palette.UpdatePalettes(3);
        if(IsKeyPressed(KEY_FOUR)) game.palette.UpdatePalettes(4);
        if(IsKeyPressed(KEY_FIVE)) game.palette.UpdatePalettes(5);
        if(IsKeyPressed(KEY_SIX)) game.palette.UpdatePalettes(6);
        if(IsKeyPressed(KEY_SEVEN)) game.palette.UpdatePalettes(7);

        if (Vector2Equals(game.snake.body[0],game.food.position)){
          game.food.position = game.food.GenerateRandomPosition(game.snake.body,game.walls.positions);
          game.snake.Grow();
          game.score.score +=1;
        }

        for(int i = 1; i < (int) game.snake.body.size(); i++){
          if(Vector2Equals(game.snake.body[0], game.snake.body[i])){
            game_over = true;
          } 
        }

        if (IsKeyPressed(KEY_R) && game_over == true){
          game.Restart();
          game_over=false;
        }

        for(int i=0; i < (int) game.walls.positions.size(); i++){
          if(Vector2Equals(game.snake.body[0],game.walls.positions[i])){
            game_over=true;
          }
        }
        
        if(frameCount>=frameStep){
          if(game.snake.body[0].x<=2 && game.snake.direction.x == -1) game_over=true;
          if((game.snake.body[0].x+3) * cellSize >= screenWidth && game.snake.direction.x == 1) game_over=true;
          if(game.snake.body[0].y<=2 && game.snake.direction.y == -1) game_over=true;
          if((game.snake.body[0].y+3) * cellSize >= screenHeight && game.snake.direction.y == 1) game_over=true;
          if(game_over==false){
            game.Update();
            frameCount = 0;
          }
        }
        
        frameCount++;
        game.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
