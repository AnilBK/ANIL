#include <stdio.h>
#include <stdlib.h>

// https://github.com/raysan5/raylib/wiki/Working-on-Windows
// Open  C:\raylib\w64devkit\w64devkit.exe
// ~ $ cd "E:\ANIL\examples\raylib"
// ~ $ gcc -o snake.exe snake_generated.c -lraylib -lgdi32 -lwinmm
// ~ $ snake

// IMPORTS //

// STRUCT_DEFINATIONS //

///*///
import raylib
import Random

struct Food{rlVector2 position};
namespace Food
function __init__(p_position : rlVector2)
  this.position = p_position
endfunction
endnamespace

struct Snake{rlVector2 position, rlVector2 speed, Vector<rlVector2> body};
namespace Snake
function __init__(p_position : rlVector2, p_speed : rlVector2)
    this.position = p_position  
    this.speed = p_speed
    this.body.__init__(10)
endfunction

function __del__()
   this.body.__del__()
endfunction

function is_touching_itself() -> bool:
    for i in range(1..this.body.len()){
        let body_pos = this.body[i]
        if this.position == body_pos{
            return true
        }
    }
    return false
endfunction

function ate_food(p_food : Food) -> bool:
  return this.position == p_food.position
endfunction

function move_body()
    for i in range(1..=this.body.len()-1,-1){
        let left_pos = this.body[i - 1]
        this.body[i] = left_pos
    }
endfunction

function is_out_of_bounds(GRID_WIDTH : int, GRID_HEIGHT : int) -> bool:
    if this.position.get_x_int() < 0{
        return true
    } else if this.position.get_x_int() >= GRID_WIDTH{
        return true
    } else if this.position.get_y_int() < 0{
        return true
    } else if this.position.get_y_int() >= GRID_HEIGHT{
        return true
    }
    return false
endfunction
endnamespace

function new_food_spawn(snake : Snake, grid_width : int, grid_height : int) -> rlVector2:
    let rng = Random{};
    
    while true {
        let new_x = rng.randrange(grid_width)
        let new_y = rng.randrange(grid_height)
        let new_pos = rlVector2{new_x, new_y};

        if new_pos in snake.body{
        
        }else{
            return new_pos
        }
    }

    // Infinite loop above or should never reach here.
    let res = rlVector2{0, 0};
    return res;
endfunction
///*///

int main() {

  // clang-format off

  ///*///  main()
  import Vector
  import String

  let rlRED      = rlColor{ 230, 41, 55, 255 };  
  let rlGREEN    = rlColor{ 0, 228, 48, 255 };   
  let rlRAYWHITE = rlColor{ 245, 245, 245, 255 };
  let rlBLACK    = rlColor{ 0, 0, 0, 255 };      

  let rlKEY_UP : int = 265
  let rlKEY_DOWN : int = 264    
  let rlKEY_RIGHT : int = 262
  let rlKEY_LEFT : int = 263 
  let rlKEY_R : int = 82  

  let CELL_SIZE : int = 20
  let GRID_WIDTH : int = 25
  let GRID_HEIGHT : int = 25

  let w_x : int = GRID_WIDTH * CELL_SIZE
  let w_y : int = GRID_HEIGHT * CELL_SIZE
  
  let mid_x : int = GRID_WIDTH / 2
  let mid_y : int = GRID_HEIGHT / 2

  let pos = rlVector2{mid_x, mid_y};
  let speed = rlVector2{1, 0};

  let snake = Snake{pos, speed};
  snake.body.push(pos)
  
  let new_food_pos = new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT)
  let food = Food{new_food_pos};

  let gameOver : bool = false

  let score : int = 0
  let scoreText = "Score: 0        "
  //                       ^^^^^^^^^ These spaces act as a buffer where sprintf 
  //                                 can write the score digits.

  let rl = raylib{};
  rl.InitWindow(w_x, w_y, "Snake Game")
  rl.SetTargetFPS(10)

  while rl.WindowShouldOpen(){
    rl.BeginDrawing()

    if gameOver{
        rl.ClearBackground(rlRAYWHITE)
        rl.DrawText("Game Over! Press R to Restart", 100, 200, 20, rlRED)
        if rl.IsKeyPressed(rlKEY_R){
            gameOver = False
            
            snake.speed.set_int(1, 0)
            snake.position.set_int(mid_x, mid_y)

            snake.body.clear()
            snake.body.push(snake.position)

            food.position = new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT)

            score = 0
            let score_ptr = scoreText.c_str()
            sprintf(score_ptr, "Score: %d", score);
        }
    }else{
        rl.ClearBackground(rlBLACK)

        // Update game.
        if rl.IsKeyPressed(rlKEY_UP){
            if snake.speed.get_y() == 0{
                snake.speed.set_int(0, -1)
            }
        }
        if rl.IsKeyPressed(rlKEY_DOWN){
            if snake.speed.get_y() == 0{
                snake.speed.set_int(0, 1)
            }
        }
        if rl.IsKeyPressed(rlKEY_LEFT){
            if snake.speed.get_x() == 0{
                snake.speed.set_int(-1, 0)
            }
        }
        if rl.IsKeyPressed(rlKEY_RIGHT){
            if snake.speed.get_x() == 0{
                snake.speed.set_int(1, 0)
            }
        }

        snake.move_body()

        // Update head position.
        snake.position.translate_i(snake.speed)
        snake.body[0] = snake.position

        // Check for wall collisions.
        if snake.is_out_of_bounds(GRID_WIDTH, GRID_HEIGHT){
            gameOver = True
        }

        if snake.is_touching_itself(){
            gameOver = True
        }

        if snake.ate_food(food){
            let last_pos = snake.body[-1]
            last_pos.__add__(snake.speed)

            snake.body.push(last_pos)

            food.position = new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT)

            score = score + 1
            let score_ptr = scoreText.c_str()
            sprintf(score_ptr, "Score: %d", score);
        }

        // Draw food.
        let fx = food.position.get_x_int()
        fx = fx * CELL_SIZE
        let fy = food.position.get_y_int()
        fy = fy * CELL_SIZE

        rl.DrawRectangle(fx, fy, CELL_SIZE, CELL_SIZE, rlRED)
        
        // Draw snake
        for body in snake.body{
            let x = body.get_x_int()
            x = x * CELL_SIZE
            let y = body.get_y_int()
            y = y * CELL_SIZE

            rl.DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, rlGREEN)
        }
    }

    rl.DrawText(scoreText, 10, 20, 20, rlRED)

    rl.EndDrawing()
  }

  rl.CloseWindow()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}