package dev.icfpc.icfpc2024.core

class Board internal constructor(
    internal val cells: Map<Point, Value> = emptyMap(),
) {
    operator fun get(point: Point) = cells[point]

    class Builder internal constructor(
        cells: Map<Point, Value>
    ) {
        val cells = cells.toMutableMap()
        var a: Int? = null
        var b: Int? = null

        fun withA(a: Int) = apply {
            this.a = a
        }

        fun withB(b: Int) = apply {
            this.b = b
        }

        fun build(): Board {
            check(a != null || cells.none { it.value == Source.A }) { "A is unset" }
            check(b != null || cells.none { it.value == Source.B }) { "B is unset" }

            return Board(
                cells = cells.mapValues { (_, value) ->
                    when (value) {
                        Source.A -> Num(a!!)
                        Source.B -> Num(b!!)
                        else -> value
                    }
                }
            )
        }

        companion object {
            fun fromString(source: String): Builder = fromLines(source.lineSequence())

            fun fromLines(lines: Sequence<String>) : Builder = Builder(
                cells = lines.flatMapIndexed { y, line ->
                    line.split(SPACES_REGEX).mapIndexedNotNull { x, cell ->
                        Value.of(cell)?.let { value ->
                            Point(x = x, y = y) to value
                        }
                    }
                }.toMap()
            )
        }
    }

    override fun toString(): String {
        val minX = cells.keys.minOf { it.x }
        val maxX = cells.keys.maxOf { it.x }
        val minY = cells.keys.minOf { it.y }
        val maxY = cells.keys.maxOf { it.y }

        return buildString((maxY - minY + 1) * (maxX - minX + 1) * 2) {
            (minY..maxY).forEach { y ->
                (minX..maxX).forEach { x ->
                    append(cells[Point(x, y)] ?: ".")
                    if (x != maxX) {
                        append("\t")
                    }
                }
                if (y != maxY) {
                    append("\n")
                }
            }
        }
    }

    companion object {
        val SPACES_REGEX = "\\s+".toRegex()
    }
}