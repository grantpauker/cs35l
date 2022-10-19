import React from "react";
import ReactDOM from "react-dom/client";
import "./index.css";
function Square(props) {
  let color = "white";
  if (props.tryingToMove) {
    color = "lightgreen";
  } else if (props.isAdjacent) {
    color = "lightpink";
  }
  return (
    <button
      className="square"
      onClick={props.onClick}
      style={{ backgroundColor: color }}
    >
      {props.value}
    </button>
  );
}

class Board extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      squares: Array(9).fill(null),
      xIsNext: true,
      turnCount: 0,
      selectedIndex: -1,
      adjacentSquares: Array(9).fill(false),
    };
  }
  handleClick(i) {
    const squares = this.state.squares.slice();

    if (calculateWinner(squares)) {
      return;
    }

    let currentLetter = this.state.xIsNext ? "X" : "O";
    if (this.state.turnCount < 6 && squares[i] == null) {
      squares[i] = currentLetter;
      this.setState({
        squares: squares,
        xIsNext: !this.state.xIsNext,
        turnCount: this.state.turnCount + 1,
        selectedIndex: -1,
        adjacentSquares: Array(9).fill(false),
      });
    } else if (this.state.turnCount >= 6) {
      if (squares[i] === currentLetter) {
        let pieceInCenter =
          (squares[4] === "X" && this.state.xIsNext) ||
          (squares[4] === "O" && !this.state.xIsNext);
        this.setState({
          squares: squares,
          xIsNext: this.state.xIsNext,
          turnCount: this.state.turnCount + 1,
          selectedIndex: i,
          adjacentSquares: this.findAdjacent(i, pieceInCenter && i !== 4), // require a win if there is a piece in the center
        });
      } else if (this.state.adjacentSquares[i]) {
        squares[i] = currentLetter;
        squares[this.state.selectedIndex] = null;
        this.setState({
          squares: squares,
          xIsNext: !this.state.xIsNext,
          turnCount: this.state.turnCount + 1,
          selectedIndex: -1,
          adjacentSquares: Array(9).fill(false),
        });
      }
    }
  }

  findAdjacent(i, requireWin = false) {
    const squares = this.state.squares.slice();
    if (squares[i] == null) {
      return null;
    }

    const adjacencyMap = [
      [1, 3, 4],
      [0, 3, 4, 5, 2],
      [1, 4, 5],
      [0, 1, 4, 6, 7],
      [0, 1, 2, 3, 5, 6, 7, 8],
      [1, 2, 4, 7, 8],
      [3, 4, 7],
      [3, 4, 5, 6, 8],
      [4, 5, 7],
    ];

    let adjacentSquares = Array(9).fill(false);
    for (const adjacent of adjacencyMap[i]) {
      if (squares[adjacent] != null) {
        continue;
      }
      if (requireWin) {
        let tmpSquares = [...squares];
        tmpSquares[adjacent] = squares[i];
        tmpSquares[i] = null;
        if (calculateWinner(tmpSquares) != null) {
          adjacentSquares[adjacent] = true;
        }
      } else {
        adjacentSquares[adjacent] = true;
      }
    }
    return adjacentSquares;
  }

  renderSquare(i) {
    return (
      <Square
        value={this.state.squares[i]}
        onClick={() => this.handleClick(i)}
        tryingToMove={this.state.selectedIndex === i}
        isAdjacent={this.state.adjacentSquares[i]}
      />
    );
  }

  render() {
    const winner = calculateWinner(this.state.squares);
    let status;
    if (winner) {
      status = "Winner: " + winner;
    } else {
      status = "Next player: " + (this.state.xIsNext ? "X" : "O");
    }

    return (
      <div>
        <div className="status">{status}</div>
        <div className="board-row">
          {this.renderSquare(0)}
          {this.renderSquare(1)}
          {this.renderSquare(2)}
        </div>
        <div className="board-row">
          {this.renderSquare(3)}
          {this.renderSquare(4)}
          {this.renderSquare(5)}
        </div>
        <div className="board-row">
          {this.renderSquare(6)}
          {this.renderSquare(7)}
          {this.renderSquare(8)}
        </div>
      </div>
    );
  }
}

class Game extends React.Component {
  render() {
    return (
      <div className="game">
        <div className="game-board">
          <Board />
        </div>
        <div className="game-info">
          <div>{/* status */}</div>
          <ol>{/* TODO */}</ol>
        </div>
      </div>
    );
  }
}

// ========================================

const root = ReactDOM.createRoot(document.getElementById("root"));
root.render(<Game />);

function calculateWinner(squares) {
  const lines = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6],
  ];
  for (let i = 0; i < lines.length; i++) {
    const [a, b, c] = lines[i];
    if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
      return squares[a];
    }
  }
  return null;
}
