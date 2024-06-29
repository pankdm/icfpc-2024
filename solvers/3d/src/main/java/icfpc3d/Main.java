package icfpc3d;

import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.function.BiFunction;
import java.util.stream.Collectors;

public class Main {

    record Coord(int x, int y) {
        public static Coord of(int x, int y) {
            return new Coord(x, y);
        }
        public Coord offset(Offset offset) {
            return new Coord(x + offset.dx(), y + offset.dy());
        }
    }

    record Offset(int dx, int dy) {
        public static final Offset left = new Offset(-1, 0);
        public static final Offset right = new Offset(1, 0);
        public static final Offset up = new Offset(0, -1);
        public static final Offset down = new Offset(0, 1);
    }

    interface Val {
        static Val of(String s) {
            return s.length() == 1 && !Character.isDigit(s.charAt(0))
                ? Op.of(s.charAt(0))
                : Int.of(s);
        }

        default Optional<Int> asInt() {
            return Optional.empty();
        }

        default Optional<Op> asOp() {
            return Optional.empty();
        }

        String toString();
    }

    record Int(BigInteger value) implements Val {
        public static Int of(String s) {
            return new Int(new BigInteger(s));
        }
        public static Int of(BigInteger x) {
            return new Int(x);
        }
        public Optional<Int> asInt() {
            return Optional.of(this);
        }
        public String toString() {
            return value.toString();
        }
        public Integer toInteger() {
            return value.intValue();
        }
    }

    record Op(char opcode) implements Val {
        public static Op of(char op) {
            return new Op(op);
        }
        public Optional<Op> asOp() {
            return Optional.of(this);
        }
        public String toString() {
            return "" + opcode;
        }

        private void arrowOp(Board board, Cell cell, Offset from, Offset to) {
            cell
                .read(board, from)
                .ifPresent(val -> {
                    cell.remove(board, from);
                    cell.write(board, to, val);
                });
        }

        private void mathOp(
            Board board,
            Cell cell,
            BiFunction<BigInteger, BigInteger, BigInteger> op
        ) {
            Optional<BigInteger> x = cell
                .read(board, Offset.left)
                .flatMap(Val::asInt)
                .map(Int::value);
            Optional<BigInteger> y = cell
                .read(board, Offset.up)
                .flatMap(Val::asInt)
                .map(Int::value);
            if (x.isPresent() && y.isPresent()) {
                cell.remove(board, Offset.left);
                cell.remove(board, Offset.up);
                Int result = Int.of(op.apply(x.get(), y.get()));
                cell.write(board, Offset.right, result);
                cell.write(board, Offset.down, result);
            }
        }

        private void equalityOp(Board board, Cell cell, boolean equality) {
            Optional<Val> x = cell.read(board, Offset.left);
            Optional<Val> y = cell.read(board, Offset.up);
            if (x.isPresent() && y.isPresent()) {
                if (x.equals(y) ^ !equality) {
                    cell.remove(board, Offset.left);
                    cell.remove(board, Offset.up);
                    cell.write(board, Offset.right, y.get());
                    cell.write(board, Offset.down, x.get());
                }
            }
        }

        // .  v  .
        // dx @ dy
        // . dt  .
        private void warp(Board board, Cell cell) {
            Optional<Val> v = cell.read(board, Offset.up);
            Optional<Integer> dx = cell
                .read(board, Offset.left)
                .flatMap(Val::asInt)
                .map(Int::toInteger);
            Optional<Integer> dy = cell
                .read(board, Offset.right)
                .flatMap(Val::asInt)
                .map(Int::toInteger);
            Optional<Integer> dt = cell
                .read(board, Offset.down)
                .flatMap(Val::asInt)
                .map(Int::toInteger);
            if (
                v.isPresent() &&
                dx.isPresent() &&
                dy.isPresent() &&
                dt.isPresent()
            ) {
                Coord coord = cell
                    .coord()
                    .offset(new Offset(-dx.get(), -dy.get()));
                board.warp(dt.get(), coord, v.get());
            }
        }

        void exec(final Board board, final Cell cell) {
            // `<`, `>`, `^`, `v`, `+`, `-`, `*`, `/`, `%`, `@`, `=`, `#`, `S`, `A`, `B`
            switch (opcode) {
                case '<':
                    arrowOp(board, cell, Offset.right, Offset.left);
                    break;
                case '>':
                    arrowOp(board, cell, Offset.left, Offset.right);
                    break;
                case '^':
                    arrowOp(board, cell, Offset.down, Offset.up);
                    break;
                case 'v':
                    arrowOp(board, cell, Offset.up, Offset.down);
                    break;
                case '+':
                    mathOp(board, cell, BigInteger::add);
                    break;
                case '-':
                    mathOp(board, cell, BigInteger::subtract);
                    break;
                case '*':
                    mathOp(board, cell, BigInteger::multiply);
                    break;
                case '/':
                    mathOp(board, cell, BigInteger::divide);
                    break;
                case '%':
                    mathOp(board, cell, BigInteger::remainder);
                    break;
                case '=':
                    equalityOp(board, cell, true);
                    break;
                case '#':
                    equalityOp(board, cell, false);
                    break;
                case '@':
                    warp(board, cell);
                    break;
                case 'A', 'B':
                    throw new IllegalStateException(
                        "A or B should've been replaced with values after parsing"
                    );
            }
        }
    }

    record Cell(Coord coord, Val val) {
        private Optional<Val> read(Board board, Offset offset) {
            return board.read(coord.offset(offset));
        }
        private void remove(Board board, Offset offset) {
            board.remove(coord.offset(offset));
        }
        private void write(Board board, Offset offset, Val val) {
            board.write(coord.offset(offset), val);
        }
    }

    static class Board {

        private History history;

        private final int t;
        private final Map<Coord, Cell> board;
        private final Set<Coord> removes = new HashSet<>();
        private final Map<Coord, Cell> writes = new HashMap<>();

        private Board warped;

        public static Board parse(String s) {
            Map<Coord, Cell> board = new HashMap<>();
            String[] rows = s.split("\n");
            for (int i = 0; i < rows.length; i++) {
                String row = rows[i];
                String[] cells = row.split("\\s+");
                for (int j = 0; j < cells.length; j++) {
                    String cell = cells[j];
                    if (cell.equals(".")) {
                        continue;
                    }
                    Coord coord = Coord.of(j, i);
                    board.put(coord, new Cell(coord, Val.of(cell)));
                }
            }
            History history = new History();
            Board result = new Board(1, board, history);
            history.append(result);
            return result;
        }

        private Board(int t, Map<Coord, Cell> board, History history) {
            this.t = t;
            this.board = board;
            this.history = history;
        }

        public void setA(int a) {
            replace('A', a);
        }

        public void setB(int b) {
            replace('B', b);
        }

        public void replace(char op, int value) {
            Set<Coord> coords = board
                .values()
                .stream()
                .filter(cell -> cell.val.equals(Op.of(op)))
                .map(Cell::coord)
                .collect(Collectors.toSet());
            coords.forEach(c -> board.put(c, new Cell(c, Int.of("" + value))));
        }

        public Optional<Val> read(Coord coord) {
            return Optional.ofNullable(board.get(coord)).map(Cell::val);
        }

        public void remove(Coord coord) {
            removes.add(coord);
        }

        public void write(Coord coord, Val val) {
            Cell existing = writes.get(coord);
            Cell update = new Cell(coord, val);
            if (existing != null) {
                throw new IllegalStateException(
                    "Illegal update: " + existing + " to " + update
                );
            }
            writes.put(coord, update);
        }

        public void warp(Integer dt, Coord coord, Val val) {
            int newTime = t - dt;
            if (warped == null) {
                Board old = history.get(newTime);
                warped = new Board(newTime, old.board, history);
            } else if (warped.t != newTime) {
                throw new IllegalStateException(
                    "attempt to warp to two different times: " +
                    warped.t +
                    " and " +
                    newTime
                );
            }

            Cell cell = new Cell(coord, val);
            warped.board.put(coord, new Cell(coord, val));

            if (warped.writes.getOrDefault(coord, cell) != cell) {
                throw new IllegalStateException(
                    "attempt to warp two different values: " +
                    warped.writes.get(coord) +
                    " and " +
                    cell
                );
            }
        }

        public Board next() {
            // execute all ops
            board.forEach(
                (coord, cell) ->
                    cell.val().asOp().ifPresent(op -> op.exec(this, cell))
            );

            // create new board using previous board state
            Map<Coord, Cell> next = new HashMap<>(board);
            // execute all removes
            removes.forEach(next::remove);

            Set<Val> results = next
                .values()
                .stream()
                .filter(cell -> cell.val.equals(Op.of('S')))
                .map(Cell::coord)
                .map(c -> Optional.ofNullable(writes.get(c)))
                .flatMap(Optional::stream)
                .map(Cell::val)
                .collect(Collectors.toSet());

            // A board can contain multiple submit operators but if more than one are overwritten at the same time, the simulation will crash.
            if (results.size() > 1) {
                throw new IllegalStateException(
                    "More than one result: " + results
                );
            }
            results.forEach(history::submit);

            if (warped != null) {
                warped.writes.clear();
                history.warp(warped.t, warped);
                return warped;
            }

            // execute all writes
            next.putAll(writes);
            Board nextBoard = new Board(t + 1, next, history);

            history.append(nextBoard);
            return nextBoard;
        }

        public History getHistory() {
            return history;
        }

        @Override
        public String toString() {
            StringBuilder sb = new StringBuilder();
            sb.append("Time: ").append(t).append("\n");
            Val result = history.getResult();
            Optional.ofNullable(result).ifPresent(
                r -> sb.append("Result: ").append(result).append("\n")
            );

            int minX = board
                .values()
                .stream()
                .map(cell -> cell.coord().x)
                .min(Integer::compareTo)
                .orElseThrow();
            int maxX = board
                .values()
                .stream()
                .map(cell -> cell.coord().x)
                .max(Integer::compareTo)
                .orElseThrow();
            int minY = board
                .values()
                .stream()
                .map(cell -> cell.coord().y())
                .min(Integer::compareTo)
                .orElseThrow();
            int maxY = board
                .values()
                .stream()
                .map(cell -> cell.coord().y())
                .max(Integer::compareTo)
                .orElseThrow();
            for (int y = minY; y <= maxY; y++) {
                for (int x = minX; x <= maxX; x++) {
                    Cell c = board.get(Coord.of(x, y));
                    sb.append(
                        Optional.ofNullable(c)
                            .map(Cell::val)
                            .map(Val::toString)
                            .orElse(".")
                    );
                    sb.append(" ");
                }
                sb.append("\n");
            }
            return sb.toString();
        }
    }

    static class History {

        private List<Board> history = new ArrayList<>();
        private Val result;

        public History() {}

        public void submit(Val result) {
            if (this.result != null) {
                throw new IllegalStateException(
                    "Result has already been submitted"
                );
            }
            this.result = result;
        }

        public void append(Board board) {
            history.add(board);
        }

        public void warp(int t, Board warped) {
            history = history.subList(0, t - 1);
            history.add(warped);
        }

        public Board get(int t) {
            return history.get(t - 1);
        }

        public Val getResult() {
            return result;
        }

        public List<Board> getHistory() {
            return history;
        }
    }

    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: icfpc.Main input.txt a b");
            System.exit(-1);
        }
        String filePath = args[0];
        int A = Integer.valueOf(args[1]);
        int B = Integer.valueOf(args[2]);

        try {
            byte[] bytes = Files.readAllBytes(Paths.get(filePath));
            String fileContent = new String(bytes);

            Board b = Board.parse(fileContent);
            b.setA(A);
            b.setB(B);
            int maxTime = 20;
            while (b.history.getResult() == null && b.t <= maxTime) {
                System.out.println(b);
                System.out.println("--------------");
                b = b.next();
            }

            System.out.println("Result: " + b.history.getResult());
        } catch (Throwable t) {
            System.err.println("oopsie " + t.toString());
        }
    }
}
