//final project : maze problem using stack DS
//created by 陳椲博 醫工二 110261035
#include <cstdint>
#include <cstddef>
#include <limits>
#include <utility>
#include <iostream>
#include <string>
using namespace std;

constexpr auto seed(){//construct for a random seed
    uint64_t shifted = 0;//unsigned long long is the same as unit64_t
    for( const auto c : __TIME__ ){
        shifted <<= 8;// <<=:left shift operator
        shifted |= c;// |=:booleen operation A=A OR B
    }
    return shifted;
}

struct PCG{ //Permuted congruential generator
    struct pcg32_random_t{uint64_t state=0;  uint64_t inc=seed();};
    pcg32_random_t rng;
    typedef uint32_t result_type;
    
    constexpr result_type operator()(){
        return pcg32_random_r();
    }
    static result_type constexpr min(){
        return numeric_limits<result_type>::min();
    }
    static result_type constexpr max(){
        return numeric_limits<result_type>::min();
    }
private:
    constexpr uint32_t pcg32_random_r(){
        uint64_t oldstate = rng.state;
        rng.state = oldstate * 6364136223846793005ULL + (rng.inc|1);
        uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
        uint32_t rot = oldstate >> 59u;
        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }
};

template<typename T, typename Gen>
constexpr auto distribution(Gen &g, T min, T max)
{
  const auto range = max - min + 1;
  const auto bias_remainder = std::numeric_limits<T>::max() % range;
  const auto unbiased_max = std::numeric_limits<T>::max() - bias_remainder - 1;

  auto r = g();
  for (; r > unbiased_max; r = g());

  return (r % range) + min;
}


struct Cell{
    bool left_open = false;
    bool right_open = false;
    bool up_open = false;
    bool down_open = false;
    bool visited = false;
};

template<typename Data, size_t Cols, size_t Rows>
struct Array2D{ // array overloading
    constexpr static auto rows(){return Rows;}
    constexpr static auto cols(){return Cols;}
    
    constexpr Data &operator()(const size_t col, const size_t row){ //not const data
        return m_data[col+(row*Cols)];
    }
    constexpr const Data &operator()(const size_t col, const size_t row) const{ //const data
        return m_data[col+(row*Cols)];
    }
    Data m_data[Cols*Rows];
};

enum class WallType{ //create the WallType class to represent each cell's condition
    Empty,
    UpperLeft,
    Vertical,
    Horizontal,
    UpperRight,
    LowerLeft,
    LowerRight,
    RightTee,
    LeftTee,
    UpTee,
    DownTee,
    FourWay,
    Up,
    Down,
    Left,
    Right,
    Visited,
    Used
};

template<typename T, size_t Size>
struct Stack{
    T m_data[Size]{};
    size_t pos = 0; //pos is like a "probe" in the array to point out the position
    
    template<typename ... Arg>
    void constexpr emplace_back(Arg &&... arg){ //like "push_back()" in std::vector
        m_data[pos] = T{std::forward<Arg>(arg)...};
        ++pos; //move probe to the next position
    }
    constexpr T pop(){ //move the top of the stack
        --pos;//move porbe backward
        return m_data[pos];
    }
    constexpr bool empty() const{
        return pos == 0; //judge if the stack is empty, witch means that the probe point to position 0
    }
    constexpr std::size_t size() const{
        return pos; //current stack size
    }
};

struct Loc{ // initialize the Location and construct some member function for overloading
    size_t col = 0;
    size_t row = 0;
    
    constexpr Loc(const size_t t_col, const size_t t_row)
    : col(t_col), row(t_row){}
    
    constexpr Loc() = default;
};

template<size_t num_cols, size_t num_rows>
constexpr Array2D<Cell, num_cols, num_rows> make_maze(){ // create a function to create a maze
    PCG pcg;
    Array2D<Cell, num_cols, num_rows> M;
    
    //starting (row, col)
    size_t c = 0;
    size_t r = 0;
    Stack<Loc, num_cols*num_rows> history;
    history.emplace_back(c,r);//history is the stack of the location
    
    //using while loop until there is no history(which means stack history is still have value
    while (!history.empty()) {
        M(c,r).visited = true;
        
        Stack<char, 4> check{};
        
        //check the adjacent cell are valid or not to move
        if(c>0 && M(c-1, r).visited==false){
            check.emplace_back('L');
        }
        if (r>0 && M(c, r-1).visited==false) {
            check.emplace_back('U');
        }
        if (c<num_cols-1 && M(c+1,r).visited==false) {
            check.emplace_back('R');
        }
        if (c<num_rows-1 && M(c,r+1).visited==false) {
            check.emplace_back('D');
        }
        
        if (!check.empty()) { //mark the cell and open
            history.emplace_back(c,r);
            
            for (auto num_pops = distribution(pcg, size_t(0), check.size() - 1); num_pops > 0; --num_pops) {
                check.pop();
            }
            
            switch (check.pop()) {
                case 'L':
                    M(c, r).left_open = true;
                    --c;
                    M(c, r).right_open = true;
                    break;

                case 'U':
                    M(c, r).up_open = true;
                    --r;
                    M(c, r).down_open = true;
                    break;

                case 'R':
                    M(c, r).right_open = true;
                    ++c;
                    M(c, r).left_open = true;
                    break;

                case 'D':
                    M(c, r).down_open = true;
                    ++r;
                    M(c, r).up_open = true;
                    break;
                  }
        }
        else {
              //no valid cells to move to => go back
              const auto last = history.pop();
              c = last.col;
              r = last.row;
        }
    }//end if pop.empty()
    
    M(0,0).left_open = true;
    M(num_cols-1, num_rows-1).right_open = true;
    
    return M;
}

template<size_t num_cols, size_t num_rows> //create wall boundary and reveal it
constexpr Array2D<WallType, num_cols*2+1, num_rows*2+1> render_maze(const Array2D<Cell, num_cols, num_rows> &maze_data){
    Array2D<WallType, num_cols*2+1, num_rows*2+1> result{};


    for (size_t col = 0; col < num_cols; ++col){
        for (size_t row = 0; row < num_rows; ++row){
            const auto render_col = col * 2 + 1;
            const auto render_row = row * 2 + 1;

            const auto &cell = maze_data(col, row);

            // upper
            if (!cell.up_open) { result(render_col,render_row-1) = WallType::Horizontal; }

            // left
            if (!cell.left_open) { result(render_col-1,render_row) = WallType::Vertical; }

            // right
            if (!cell.right_open) { result(render_col+1,render_row) = WallType::Vertical; }

            // lower
            if (!cell.down_open) { result(render_col,render_row+1) = WallType::Horizontal; }
        }
    }
    for (size_t col = 0; col < result.cols(); col += 2){
        for (size_t row = 0; row < result.rows(); row += 2)
        {
          const auto up     = (row == 0)?false:result(col, row-1) != WallType::Empty;
          const auto left   = (col == 0)?false:result(col-1, row) != WallType::Empty;
          const auto right  = (col == num_cols * 2)?false:result(col+1, row) != WallType::Empty;
          const auto down   = (row == num_rows * 2)?false:result(col, row+1) != WallType::Empty;

          if (up && right && down && left) {
            result(col, row) = WallType::FourWay;
          }
          if (up && right && down && !left) {
            result(col, row) = WallType::RightTee;
          }
          if (up && right && !down && left) {
            result(col, row) = WallType::UpTee;
          }
          if (up && !right && down && left) {
            result(col, row) = WallType::LeftTee;
          }
          if (!up && right && down && left) {
            result(col, row) = WallType::DownTee;
          }

          if (up && right && !down && !left) {
            result(col, row) = WallType::LowerLeft;
          }
          if (up && !right && !down && left) {
            result(col, row) = WallType::LowerRight;
          }
          if (!up && !right && down && left) {
            result(col, row) = WallType::UpperRight;
          }
          if (!up && right && down && !left) {
            result(col, row) = WallType::UpperLeft;
          }
          if (!up && right && !down && left) {
            result(col, row) = WallType::Horizontal;
          }
          if (up && !right && down && !left) {
            result(col, row) = WallType::Vertical;
          }
          if (up && !right && !down && !left) {
            result(col, row) = WallType::Up;
          }
          if (!up && right && !down && !left) {
            result(col, row) = WallType::Right;
          }
          if (!up && !right && down && !left) {
            result(col, row) = WallType::Down;
          }
          if (!up && !right && !down && left) {
            result(col, row) = WallType::Left;
          }
        }
      }
    
    return result;
}
/*-------------------------Maze created-------------------------*/
/*-------------------------solve the maze-------------------------*/
template<typename T>
constexpr auto solve(T maze)
{
  constexpr auto num_cols = maze.cols();
  constexpr auto num_rows = maze.rows();

  size_t row = 1; //entry
  size_t col = 0;

  Stack<Loc, num_cols * num_rows> history;

  history.emplace_back(col, row);
  while (row != maze.rows() - 2 || col != maze.cols() - 2)
  {
    maze(col, row) = WallType::Visited;

    //if the adjacent cells are valid for moving
    if (col < num_cols-1 && maze(col+1, row) == WallType::Empty) {
      ++col;
      history.emplace_back(col, row);
    } else if (row < num_rows-1 && maze(col, row+1) == WallType::Empty) {
      ++row;
      history.emplace_back(col, row);
    } else if (col > 0 && maze(col-1, row) == WallType::Empty) {
      --col;
      history.emplace_back(col, row);
    } else if (row > 0 && maze(col, row-1) == WallType::Empty) {
      --row;
      history.emplace_back(col, row);
    } else {
        //no valid cells to move to => go back to possible-moving cell
      const auto last = history.pop();
      col = last.col;
      row = last.row;
    }
  }

  while (!history.empty())
  {
    const auto last = history.pop();
    maze(last.col, last.row) = WallType::Used;
  }

  return maze;
}
/*-------------------------Solve-------------------------*/
int main()//main func
{
    //If the question is in Professor's PowerPoint is 15*15
    constexpr const size_t num_cols = 15;
    constexpr const size_t num_rows = 15;

    auto maze = make_maze<num_cols, num_rows>();
    auto rendered_maze = solve(render_maze(maze));

    for (std::size_t row = 0; row < num_rows*2+1; ++row){
        for (std::size_t col = 0; col < num_cols*2+1; ++col){
            const auto square = [&](){
            switch (rendered_maze(col, row)) {
                case WallType::Empty:      return " ";
                case WallType::UpperLeft:  return "┌";
                case WallType::Vertical:   return "│";
                case WallType::Horizontal: return "─";
                case WallType::UpperRight: return "┐";
                case WallType::LowerLeft:  return "└";
                case WallType::LowerRight: return "┘";
                case WallType::RightTee:   return "├";
                case WallType::LeftTee:    return "┤";
                case WallType::UpTee:      return "┴";
                case WallType::DownTee:    return "┬";
                case WallType::FourWay:    return "┼";
                case WallType::Up:         return "╵";
                case WallType::Down:       return "╷";
                case WallType::Left:       return "╴";
                case WallType::Right:      return "╶";
                case WallType::Visited:    return "·";
                case WallType::Used:       return "*";
                default: throw 0;
            }
        }();

          std::cout << square;
        }
        std::cout << '\n';
    }
} //end main

// Reference:
// I.<https:/ /en.wikipedia.org/wiki/Permuted_congruential_generator>
