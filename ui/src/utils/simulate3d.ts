export class Offset {
  dx: number
  dy: number

  constructor(dx: number, dy: number) {
    this.dx = dx
    this.dy = dy
  }

  static get left(): Offset {
    return new Offset(-1, 0)
  }

  static get right(): Offset {
    return new Offset(1, 0)
  }

  static get up(): Offset {
    return new Offset(0, -1)
  }

  static get down(): Offset {
    return new Offset(0, 1)
  }
}

export class Coord {
  x: number
  y: number

  constructor(x: number, y: number) {
    this.x = x
    this.y = y
  }

  static of(x: number, y: number): Coord {
    return new Coord(x, y)
  }

  offset(offset: Offset): Coord {
    return new Coord(this.x + offset.dx, this.y + offset.dy)
  }
}

export class Cell {
  coord: Coord
  val: Val

  constructor(coord: Coord, val: Val) {
    this.coord = coord
    this.val = val.copy()
  }

  copy(): Cell {
    return new Cell(this.coord, this.val)
  }
}

export abstract class Val {
  static of(s: string): Val {
    return s.length === 1 && isNaN(Number(s)) ? Op.of(s.charAt(0)) : Int.of(s)
  }

  abstract asInt(): Int | null

  abstract asOp(): Op | null

  abstract copy(): Val
}

export class Int extends Val {
  value: number

  constructor(value: number) {
    super()
    this.value = value
  }

  copy(): Int {
    return new Int(this.value)
  }

  static of(s: string | number): Int {
    return new Int(parseInt(s as string))
  }

  asInt(): Int {
    return this
  }

  asOp(): Op | null {
    return null
  }

  toString(): string {
    return this.value.toString()
  }

  toInteger(): number {
    return this.value
  }
}

export class Op extends Val {
  opcode: string

  constructor(opcode: string) {
    super()
    this.opcode = opcode
  }

  copy(): Op {
    return new Op(this.opcode)
  }

  static of(op: string) {
    return new Op(op)
  }

  asInt() {
    return null
  }

  asOp() {
    return this
  }

  toString() {
    return this.opcode
  }

  arrowOp(board: BoardStatus, cell: Cell, from: Offset, to: Offset) {
    const val = board.read(cell.coord.offset(from))
    if (val) {
      board.remove(cell.coord.offset(from))
      board.write(cell.coord.offset(to), val.copy())
    }
  }

  mathOp(board: BoardStatus, cell: Cell, op: (x: number, y: number) => number) {
    const x = board.read(cell.coord.offset(Offset.left))?.asInt()
      ?.value as number
    const y = board.read(cell.coord.offset(Offset.up))?.asInt()?.value as number
    if (isFinite(x) && isFinite(y)) {
      board.remove(cell.coord.offset(Offset.left))
      board.remove(cell.coord.offset(Offset.up))
      const result = Int.of(op(x, y))
      board.write(cell.coord.offset(Offset.right), result)
      board.write(cell.coord.offset(Offset.down), result)
    }
  }

  equalityOp(board: BoardStatus, cell: Cell, equality: boolean) {
    const x = board.read(cell.coord.offset(Offset.left))
    const y = board.read(cell.coord.offset(Offset.up))
    if (x && y) {
      if ((x.value === y.value) === equality) {
        board.remove(cell.coord.offset(Offset.left))
        board.remove(cell.coord.offset(Offset.up))
        board.write(cell.coord.offset(Offset.right), y.copy())
        board.write(cell.coord.offset(Offset.down), x.copy())
      }
    }
  }

  warp(board: BoardStatus, cell: Cell) {
    const v = board.read(cell.coord.offset(Offset.up))
    const dx = board.read(cell.coord.offset(Offset.left))?.asInt()?.toInteger()
    const dy = board.read(cell.coord.offset(Offset.right))?.asInt()?.toInteger()
    const dt = board.read(cell.coord.offset(Offset.down))?.asInt()?.toInteger()
    if (v && dx !== undefined && dy !== undefined && dt !== undefined) {
      const coord = cell.coord.offset(new Offset(-dx, -dy))
      board.warp(dt, coord, v)
    }
  }

  exec(board: BoardStatus, cell: Cell) {
    switch (this.opcode) {
      case '<':
        this.arrowOp(board, cell, Offset.right, Offset.left)
        break
      case '>':
        this.arrowOp(board, cell, Offset.left, Offset.right)
        break
      case '^':
        this.arrowOp(board, cell, Offset.down, Offset.up)
        break
      case 'v':
        this.arrowOp(board, cell, Offset.up, Offset.down)
        break
      case '+':
        this.mathOp(board, cell, (a, b) => a + b)
        break
      case '-':
        this.mathOp(board, cell, (a, b) => a - b)
        break
      case '*':
        this.mathOp(board, cell, (a, b) => a * b)
        break
      case '/':
        this.mathOp(board, cell, (a, b) => Math.floor(a / b))
        break
      case '%':
        this.mathOp(board, cell, (a, b) => a % b)
        break
      case '=':
        this.equalityOp(board, cell, true)
        break
      case '#':
        this.equalityOp(board, cell, false)
        break
      case '@':
        this.warp(board, cell)
        break
      case 'A':
      case 'B':
        throw new Error(
          "A or B should've been replaced with values after parsing"
        )
    }
  }
}

export class BoardMap extends Map {
  constructor(...args: any[]) {
    super(...args)
  }

  has(coord: Coord) {
    return super.has(`${[coord.x, coord.y]}`)
  }

  set(cell: Cell) {
    return super.set(`${[cell.coord.x, cell.coord.y]}`, cell)
  }

  get(coord: Coord | [number, number]) {
    if (Array.isArray(coord)) {
      return super.get(`${[...coord]}`)
    }
    return super.get(`${[coord.x, coord.y]}`)
  }

  delete(coord: Coord) {
    return super.delete(`${[coord.x, coord.y]}`)
  }

  copy() {
    const newMap = new BoardMap()
    Array.from(this.values()).forEach((cell) => {
      newMap.set(cell.copy())
    })
    return newMap
  }

  toString() {
    let sb = ''

    const coords = Array.from(this.values()).map((cell) => cell.coord)
    const minX = Math.min(0, ...coords.map((coord) => coord.x))
    const maxX = Math.max(...coords.map((coord) => coord.x))
    const minY = Math.min(0, ...coords.map((coord) => coord.y))
    const maxY = Math.max(...coords.map((coord) => coord.y))
    for (let y = minY; y <= maxY; y++) {
      for (let x = minX; x <= maxX; x++) {
        const cell = this.get(Coord.of(x, y))
        sb += cell ? cell.val.toString() : '.'
        sb += ' '
      }
      sb += '\n'
    }
    return sb
  }
}

export class BoardStatus {
  t: number
  board: BoardMap
  history: History
  removes: Set<Coord>
  writes: BoardMap
  warped: BoardStatus | null
  result: Val | null

  constructor(t: number, board: BoardMap, history: History) {
    this.t = t
    this.board = board
    this.history = history
    this.removes = new Set()
    this.writes = new BoardMap()
    this.warped = null
    this.result = null
  }

  static parse(s: string) {
    const board = new BoardMap()
    const rows = s.trim().split('\n')
    for (let i = 0; i < rows.length; i++) {
      const row = rows[i]
      const cells = row.trim().split(/\s+/)
      for (let j = 0; j < cells.length; j++) {
        const cell = cells[j]
        if (cell === '.') {
          continue
        }
        const coord = Coord.of(j, i)
        board.set(new Cell(coord, Val.of(cell)))
      }
    }
    const history = new History()
    const result = new BoardStatus(1, board, history)
    history.append(result)
    return result
  }

  setA(a: number) {
    this.replace('A', a)
  }

  setB(b: number) {
    this.replace('B', b)
  }

  replace(op: string, value: number) {
    const cells = Array.from<Cell>(this.board.values()).filter(
      (c) => (c.val as Op).opcode === op
    )
    for (const cell of cells) {
      this.board.set(new Cell(cell.coord, Int.of(value)))
    }
  }

  read(coord: Coord): Val | null {
    return this.board.get(coord)?.val || null
  }

  remove(coord: Coord) {
    this.removes.add(coord)
  }

  write(coord: Coord, val: Val) {
    if (this.writes.has(coord)) {
      throw new Error(
        `Illegal update: ${this.writes.get(coord)} to ${new Cell(coord, val)}`
      )
    }
    this.writes.set(new Cell(coord, val))
  }

  warp(dt: number, coord: Coord, val: Val) {
    const newTime = this.t - dt
    if (!this.warped) {
      const old = this.history.get(newTime)
      this.warped = new BoardStatus(newTime, old.board.copy(), this.history)
    } else if (this.warped.t !== newTime) {
      throw new Error(
        `attempt to warp to two different times: ${this.warped.t} and ${newTime}`
      )
    }

    const cell = new Cell(coord, val)
    this.warped.board.set(cell)

    if (
      this.warped.writes.get(coord) &&
      this.warped.writes.get(coord) !== cell
    ) {
      throw new Error(
        `attempt to warp two different values: ${this.warped.writes.get(
          coord
        )} and ${cell}`
      )
    }
  }

  copy() {
    return new BoardStatus(this.t, this.board.copy(), this.history)
  }

  next() {
    this.board.forEach((cell) => {
      cell.val.asOp()?.exec(this, cell)
    })

    const nextBoard = this.board.copy()

    this.removes.forEach((coord) => nextBoard.delete(coord))

    const results = Array.from<Cell>(nextBoard.values())
      .filter((cell) => (cell.val as Op).opcode === 'S')
      .map((cell) => cell.coord)
      .map((c) => this.writes.get(c) as Cell)
      .filter((val) => val)
      .map((cell) => cell.val)

    if (results.length > 1) {
      throw new Error(`More than one result: ${results}`)
    }
    results.forEach((result) => this.history.submit(result))

    if (this.warped) {
      this.warped.writes.clear()
      this.history.warp(this.warped.t, this.warped)
      return this.warped
    }

    this.writes.forEach((writeCell) => {
      nextBoard.set(writeCell)
    })

    const nextBoardStatus = new BoardStatus(this.t + 1, nextBoard, this.history)
    this.history.append(nextBoardStatus)
    if (results[0]) {
      nextBoardStatus.result = results[0] || null
    }
    return nextBoardStatus
  }

  getHistory(): History {
    return this.history
  }

  getBoundingRect() {
    const coords = Array.from(this.board.values()).map((cell) => cell.coord)
    const minX = Math.min(...coords.map((coord) => coord.x))
    const maxX = Math.max(...coords.map((coord) => coord.x))
    const minY = Math.min(...coords.map((coord) => coord.y))
    const maxY = Math.max(...coords.map((coord) => coord.y))
    return [minX, minY, maxX, maxY]
  }

  toString() {
    let sb = `Time: ${this.t}\n`
    const result = this.history.result
    if (result) {
      sb += `Result: ${result}\n`
    }

    sb += this.board.toString()

    return sb
  }
}

export class History {
  history: BoardStatus[]
  result: Val | null

  constructor() {
    this.history = []
    this.result = null
  }

  submit(result: Val) {
    if (this.result) {
      throw new Error('Result has already been submitted')
    }
    this.result = result
  }

  append(board: BoardStatus) {
    this.history.push(board)
  }

  warp(t: number, warped: BoardStatus) {
    this.history = this.history.slice(0, t - 1)
    this.history.push(warped)
  }

  get(t: number) {
    return this.history[t - 1]
  }
}

export const simulate = (
  inputBoard: string,
  A: number,
  B: number,
  maxIterations = 1e3
) => {
  let board = BoardStatus.parse(inputBoard)
  let historicalBoards = [board.copy()]
  let [minX, minY, maxX, maxY] = board.getBoundingRect()
  let maxT = 1
  try {
    board.setA(A)
    board.setB(B)
    const maxTime = 20
    let iterationsLeft = maxIterations
    while (!board.history.result && board.t <= maxTime && iterationsLeft > 0) {
      console.log(board.toString())
      console.log('--------------')
      historicalBoards.push(board)
      board = board.next()

      maxT = Math.max(board.t, maxT)
      const [newMinX, newMinY, newMaxX, newMaxY] = board.getBoundingRect()
      minX = Math.min(minX, newMinX)
      minY = Math.min(minY, newMinY)
      maxX = Math.max(maxX, newMaxX)
      maxY = Math.max(maxY, newMaxY)

      iterationsLeft -= 1
    }

    console.log(`Result: ${board.history.result}`)
  } catch (t) {
    console.error('oopsie', t)
    throw t
  }
  return {
    finalBoard: board,
    historicalBoards,
    spaceUsed: {
      minX,
      maxX,
      minY,
      maxY,
      x: maxX - minX + 1,
      y: maxY - minY + 1,
      ticks: maxT,
    },
  }
}

export const example = `
 .  .  .  .  .  .  .  .
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
