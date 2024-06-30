export class Coord {
  constructor(x, y) {
    this.x = x;
    this.y = y;
  }

  static of(x, y) {
    return new Coord(x, y);
  }

  offset(offset) {
    return new Coord(this.x + offset.dx, this.y + offset.dy);
  }

  copy() {
    return new Coord(this.x, this.y)
  }
}

export class Offset {
  constructor(dx, dy) {
    this.dx = dx;
    this.dy = dy;
  }

  static get left() {
    return new Offset(-1, 0);
  }

  static get right() {
    return new Offset(1, 0);
  }

  static get up() {
    return new Offset(0, -1);
  }

  static get down() {
    return new Offset(0, 1);
  }
}

export class Val {
  static of(s) {
    return s.length === 1 && isNaN(s)
      ? Op.of(s.charAt(0))
      : Int.of(s);
  }

  asInt() {
    return null;
  }

  asOp() {
    return null;
  }

  copy() {
    return Val.of(this.toString())
  }
}

export class Int extends Val {
  constructor(value) {
    super();
    this.value = parseInt(value);
  }

  static of(s) {
    return new Int(s);
  }

  asInt() {
    return this;
  }

  toString() {
    return this.value.toString();
  }

  toInteger() {
    return parseInt(this.value);
  }
}

export class Op extends Val {
  constructor(opcode) {
    super();
    this.opcode = opcode;
  }

  static of(op) {
    return new Op(op);
  }

  asOp() {
    return this;
  }

  toString() {
    return this.opcode;
  }

  arrowOp(board, cell, from, to) {
    const val = cell.read(board, from);
    if (val) {
      cell.remove(board, from);
      cell.write(board, to, val);
    }
  }

  mathOp(board, cell, op) {
    const x = cell.read(board, Offset.left)?.asInt()?.value;
    const y = cell.read(board, Offset.up)?.asInt()?.value;
    if (isFinite(x) && isFinite(y)) {
      cell.remove(board, Offset.left);
      cell.remove(board, Offset.up);
      const result = Int.of(op(x, y));
      cell.write(board, Offset.right, result);
      cell.write(board, Offset.down, result);
    }
  }

  equalityOp(board, cell, equality) {
    const x = cell.read(board, Offset.left);
    const y = cell.read(board, Offset.up);
    if (x && y) {
      if ((x.value === y.value) === equality) {
        cell.remove(board, Offset.left);
        cell.remove(board, Offset.up);
        cell.write(board, Offset.right, y);
        cell.write(board, Offset.down, x);
      }
    }
  }

  warp(board, cell) {
    const v = cell.read(board, Offset.up);
    const dx = cell.read(board, Offset.left)?.asInt()?.toInteger();
    const dy = cell.read(board, Offset.right)?.asInt()?.toInteger();
    const dt = cell.read(board, Offset.down)?.asInt()?.toInteger();
    if (v && dx !== undefined && dy !== undefined && dt !== undefined) {
      const coord = cell.coord.offset(new Offset(-dx, -dy));
      board.warp(dt, coord, v);
    }
  }

  exec(board, cell) {
    switch (this.opcode) {
      case '<':
        this.arrowOp(board, cell, Offset.right, Offset.left);
        break;
      case '>':
        this.arrowOp(board, cell, Offset.left, Offset.right);
        break;
      case '^':
        this.arrowOp(board, cell, Offset.down, Offset.up);
        break;
      case 'v':
        this.arrowOp(board, cell, Offset.up, Offset.down);
        break;
      case '+':
        this.mathOp(board, cell, (a, b) => a + b);
        break;
      case '-':
        this.mathOp(board, cell, (a, b) => a - b);
        break;
      case '*':
        this.mathOp(board, cell, (a, b) => a * b);
        break;
      case '/':
        this.mathOp(board, cell, (a, b) => Math.floor(a / b));
        break;
      case '%':
        this.mathOp(board, cell, (a, b) => a % b);
        break;
      case '=':
        this.equalityOp(board, cell, true);
        break;
      case '#':
        this.equalityOp(board, cell, false);
        break;
      case '@':
        this.warp(board, cell);
        break;
      case 'A':
      case 'B':
        throw new Error("A or B should've been replaced with values after parsing");
    }
  }
}

export class Cell {
  constructor(coord, val) {
    this.coord = coord;
    this.val = val;
  }

  read(board, offset) {
    return board.read(this.coord.offset(offset));
  }

  remove(board, offset) {
    board.remove(this.coord.offset(offset));
  }

  write(board, offset, val) {
    board.write(this.coord.offset(offset), val);
  }

  copy() {
    return new Cell(this.coord.copy(), this.val.copy())
  }
}

export class BoardMap extends Map {
  constructor(...args) {
    super(...args)
  }

  has(coord) {
    super.has(`${[coord.x, coord.y]}`)
  }

  set(cell) {
    super.set(`${[cell.coord.x, cell.coord.y]}`, cell)
  }

  get(coord) {
    return super.get(`${[coord.x, coord.y]}`)
  }

  delete(coord) {
    return super.delete(`${[coord.x, coord.y]}`)
  }

  copy() {
    const newMap = new BoardMap()
    Array.from(this.values()).forEach((cell) => {
      newMap.set(cell)
    })
    return newMap
  }
}

export class BoardStatus {
  constructor(t, board, history) {
    this.t = t;
    this.board = board;
    this.snapshot = null;
    this.history = history;
    this.removes = new Set();
    this.writes = new BoardMap();
    this.warped = null;
    this.result = null
  }

  static parse(s) {
    const board = new BoardMap();
    const rows = s.trim().split('\n');
    for (let i = 0; i < rows.length; i++) {
      const row = rows[i];
      const cells = row.trim().split(/\s+/);
      for (let j = 0; j < cells.length; j++) {
        const cell = cells[j];
        if (cell === '.') {
          continue;
        }
        const coord = Coord.of(j, i);
        board.set(new Cell(coord, Val.of(cell)));
      }
    }
    const history = new History();
    const result = new BoardStatus(1, board, history);
    history.append(result);
    // result.saveSnapshot()
    return result;
  }

  setA(a) {
    this.replace('A', a);
  }

  setB(b) {
    this.replace('B', b);
  }

  replace(op, value) {
    Array.from(this.board.values())
      .filter(cell => cell.val.opcode && cell.val.opcode === op)
      .forEach(cell => cell.val = Int.of('' + value))
  }

  read(coord) {
    return this.board.get(coord)?.val || null;
  }

  remove(coord) {
    this.removes.add(coord);
  }

  write(coord, val) {
    if (this.writes.has(coord)) {
      throw new Error(`Illegal update: ${this.writes.get(coord)} to ${new Cell(coord, val)}`);
    }
    this.writes.set(new Cell(coord, val));
  }

  warp(dt, coord, val) {
    const newTime = this.t - dt;
    if (!this.warped) {
      const old = this.history.get(newTime);
      this.warped = new BoardStatus(newTime, old.board, this.history);
    } else if (this.warped.t !== newTime) {
      throw new Error(`attempt to warp to two different times: ${this.warped.t} and ${newTime}`);
    }

    const cell = new Cell(coord, val);
    this.warped.board.set(cell);

    if (this.warped.writes.get(coord) && this.warped.writes.get(coord) !== cell) {
      throw new Error(`attempt to warp two different values: ${this.warped.writes.get(coord)} and ${cell}`);
    }
  }

  next() {
    this.board.forEach((cell, coord) => {
      cell.val.asOp()?.exec(this, cell);
    });

    // console.log('>>> compute next board')

    // const nextBoard = new BoardMap();
    // Array.from(this.board.values()).forEach(cell => nextBoard.set(cell.coord, cell))

    const nextBoard = this.board.copy()

    // console.log('nextBoard', nextBoard)
    // console.log('removes', this.removes)
    this.removes.forEach(coord => nextBoard.delete(coord));
    // console.log('nextBoard', nextBoard)

    const results = Array.from(nextBoard.values())
      .filter(cell => cell.val.opcode === 'S')
      .map(cell => cell.coord)
      .map(c => this.writes.get(c))
      .filter(val => val)
      .map(cell => cell.val);

    // console.log('results', results)

    if (results.length > 1) {
      throw new Error(`More than one result: ${results}`);
    }
    results.forEach(result => this.history.submit(result));

    if (this.warped) {
      this.warped.writes.clear();
      this.history.warp(this.warped.t, this.warped);
      return this.warped;
    }

    // console.log('writes', this.writes)
    nextBoard.forEach((cell, coord) => {
      this.writes.forEach((writeCell) => {
        nextBoard.set(writeCell);
      });
    });
    // console.log('nextBoard', nextBoard)
    // console.log('<<< compute next board')

    const newBoard = new BoardStatus(this.t + 1, nextBoard, this.history);
    this.history.append(newBoard);
    if (results[0]) {
      newBoard.result = results[0] || null
    }
    // newBoard.saveSnapshot()
    return newBoard;
  }

  getHistory() {
    return this.history;
  }

  getBoundingRect() {
    const coords = Array.from(this.board.values()).map(cell => cell.coord);
    const minX = Math.min(...coords.map(coord => coord.x));
    const maxX = Math.max(...coords.map(coord => coord.x));
    const minY = Math.min(...coords.map(coord => coord.y));
    const maxY = Math.max(...coords.map(coord => coord.y));
    return {
      minX, minY, maxX, maxY
    }
  }

  // saveSnapshot() {
  //   // const coords = Array.from(this.board.values()).map(cell => cell.coord);
  //   // const minX = Math.min(...coords.map(coord => coord.x));
  //   // const maxX = Math.max(...coords.map(coord => coord.x));
  //   // const minY = Math.min(...coords.map(coord => coord.y));
  //   // const maxY = Math.max(...coords.map(coord => coord.y));

  //   this.snapshot = new BoardMap()
  //   Array.from(this.board.values()).forEach(cell =>
  //     this.snapshot.set(
  //       Coord.of(cell.coord.x, cell.coord.y),
  //       new Cell(
  //         Coord.of(cell.coord.x, cell.coord.y),
  //         Val.of(cell.value.toString())
  //       )
  //     )
  //   );

  //   // for (let y = minY; y <= maxY; y++) {
  //   //   for (let x = minX; x <= maxX; x++) {
  //   //     const cell = this.board.get(Coord.of(x, y));
  //   //     snapshot.push({
  //   //       x: cell? cell.val.toString() : '.')
  //   //   }
  //   // }
  //   // this.snapshot = snapshot
  // }

  // snapshotToString() {
  //   const minX = Math.min(...this.snapshot.map(c => c.x));
  //   const maxX = Math.max(...this.snapshot.map(c => c.x));
  //   const minY = Math.min(...this.snapshot.map(c => c.y));
  //   const maxY = Math.max(...this.snapshot.map(c => c.y));

  //   let sb = ''
  //   for (let y = minY; y <= maxY; y++) {
  //     for (let x = minX; x <= maxX; x++) {
  //       const cell = this.board.get(Coord.of(x, y));
  //       snapshot.push({
  //         x: cell? cell.val.toString() : '.')
  //     }
  //   }
  //   for (const row of this.snapshot) {
  //     for (const item of row) {
  //       sb += item + ' ';
  //     }
  //     sb += '\n';
  //   }
  //   return sb
  // }

  boardToString() {
    let sb = ''

    const coords = Array.from(this.board.values()).map(cell => cell.coord);
    const minX = Math.min(0, ...coords.map(coord => coord.x));
    const maxX = Math.max(...coords.map(coord => coord.x));
    const minY = Math.min(0, ...coords.map(coord => coord.y));
    const maxY = Math.max(...coords.map(coord => coord.y));
    for (let y = minY; y <= maxY; y++) {
      for (let x = minX; x <= maxX; x++) {
        const cell = this.board.get(Coord.of(x, y));
        sb += cell ? cell.val.toString() : '.'
        sb += ' '
      }
      sb += '\n'
    }
    return sb
  }

  toString() {
    let sb = `Time: ${this.t}\n`;
    const result = this.history.result;
    if (result) {
      sb += `Result: ${result}\n`;
    }

    sb += this.boardToString()

    return sb;
  }
}

export class History {
  constructor() {
    this.history = [];
    this.result = null;
  }

  submit(result) {
    if (this.result) {
      throw new Error('Result has already been submitted');
    }
    this.result = result;
  }

  append(board) {
    this.history.push(board);
  }

  warp(t, warped) {
    this.history = this.history.slice(0, t - 1);
    this.history.push(warped);
  }

  get(t) {
    return this.history[t - 1];
  }
}

export const simulate = (inputBoard, A, B, maxIterations=1e3) => {
  try {
    let board = BoardStatus.parse(inputBoard);
    board.setA(A);
    board.setB(B);
    let historicalBoards = []
    const maxTime = 20;
    let iterationsLeft = maxIterations
    while (!board.history.result && board.t <= maxTime && iterationsLeft > 0) {
      console.log(board.toString());
      console.log('--------------');
      historicalBoards.push(board)
      board = board.next();
      iterationsLeft -= 1
    }

    console.log(`Result: ${board.history.result}`);
    return { finalBoard: board, historicalBoards }
  } catch (t) {
    console.error('oopsie', t);
  }
}

export const example = `
 .  .  .  .  .  .  .  *
 .  .  .  .  .  .  ^  S
 .  .  .  .  1  >  .  .
 .  .  <  A  >  .  *  .
 1  = -1  +  .  .  .  .
 .  .  .  .  .  2  @  3
-6  @  7  v  .  .  2  .
 .  2  .  .  .  .  .  .
 .  .  0  @  5  .  .  .
 .  .  .  2  .  .  .  .
`

export const test = () => simulate(example, 10, 0)
