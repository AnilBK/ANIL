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

let rng = Random{};

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

function is_touching_itself() -> bool:
    let body_ref = this.body

    for body_pos in body_ref[1:]{
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
    let x = this.position.get_x_int()
    let y = this.position.get_y_int()

    if x < 0{
        return true
    } else if x >= GRID_WIDTH{
        return true
    } else if y < 0{
        return true
    } else if y >= GRID_HEIGHT{
        return true
    }
    return false
endfunction
endnamespace

function new_food_spawn(snake : Snake, grid_width : int, grid_height : int) -> rlVector2:
    while true {
        let new_x = rng.randrange(grid_width)
        let new_y = rng.randrange(grid_height)
        let new_pos = rlVector2{new_x, new_y};

        if new_pos not in snake.body{
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
  let next_speed = rlVector2{1, 0};

  let snake = Snake{pos, speed};
  snake.body.push(pos)
  
  let new_food_pos = new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT)
  let food = Food{new_food_pos};

  let gameOver : bool = false

  let score : int = 0
  let scoreText = "Score: 0"

  let rl = raylib{};
  rl.InitWindow(w_x, w_y, "Snake Game")
  rl.SetTargetFPS(60)

  let move_timer : float = 0.0
  let move_delay : float = 0.1
  let input_handled_this_tick : bool = False

  while rl.WindowShouldOpen(){
    rl.BeginDrawing()
    rl.ClearBackground(rlBLACK)

    let frame_time : float = rl.GetFrameTime()

    if !gameOver{
        // Buffer ONLY 1 valid command per internal clock tick (stops immediate
        // self-reverse suicide input mapping)
        if !input_handled_this_tick{
            if rl.IsKeyPressed(rlKEY_UP){
                if snake.speed.get_y() == 0{
                    next_speed.set_int(0, -1)
                }
                input_handled_this_tick = True
            } else if rl.IsKeyPressed(rlKEY_DOWN){
                if snake.speed.get_y() == 0{
                    next_speed.set_int(0, 1)
                }
                input_handled_this_tick = True
            } else if rl.IsKeyPressed(rlKEY_LEFT){
                if snake.speed.get_x() == 0{
                    next_speed.set_int(-1, 0)
                }
                input_handled_this_tick = True
            } else if rl.IsKeyPressed(rlKEY_RIGHT){
                if snake.speed.get_x() == 0{
                    next_speed.set_int(1, 0)
                }
                input_handled_this_tick = True
            }
        }

        move_timer += frame_time;

        if move_timer >= move_delay{
            move_timer -= move_delay;
            input_handled_this_tick = False

            snake.speed = next_speed

            let tail_pos = snake.body[-1]
            snake.move_body()

            # Update head position.
            snake.position.translate_i(snake.speed)
            snake.body[0] = snake.position

            if snake.is_out_of_bounds(GRID_WIDTH, GRID_HEIGHT){
                gameOver = True
            } else if snake.is_touching_itself(){
                gameOver = True
            } else if snake.ate_food(food){
                snake.body.push(tail_pos)

                food.position = new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT)

                score = score + 1
                scoreText.format("Score: %d", score)
            }
        }

    }

    // Always keep drawing visual environment (allows seeing exactly where and
    // how player died instead of wiping screen clear)

    // Draw food.
    let fx : int = food.position.get_x_int() * CELL_SIZE
    let fy : int = food.position.get_y_int() * CELL_SIZE

    rl.DrawRectangle(fx, fy, CELL_SIZE, CELL_SIZE, rlRED)
    
    // Draw snake
    for body in snake.body{
        let x : int = body.get_x_int() * CELL_SIZE
        let y : int = body.get_y_int() * CELL_SIZE

        rl.DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, rlGREEN)
    }

    rl.DrawText(scoreText, 10, 20, 20, rlRED)

    if gameOver{
        rl.DrawText("Game Over! Press R to Restart", 100, 200, 20, rlRAYWHITE)
        
        if rl.IsKeyPressed(rlKEY_R){
            gameOver = False
            
            snake.speed.set_int(1, 0)
            next_speed.set_int(1, 0)

            snake.position.set_int(mid_x, mid_y)
            snake.body.clear()
            snake.body.push(snake.position)

            food.position = new_food_spawn(snake, GRID_WIDTH, GRID_HEIGHT)

            score = 0
            scoreText = "Score: 0"
            move_timer = 0.0;
            input_handled_this_tick = False
        }
    }

    rl.EndDrawing()
  }

  rl.CloseWindow()

  // DESTRUCTOR_CODE //
  ///*///
  // clang-format on

  return 0;
}