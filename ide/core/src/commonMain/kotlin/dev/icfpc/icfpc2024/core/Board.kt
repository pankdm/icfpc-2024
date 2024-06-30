package dev.icfpc.icfpc2024.core

class Board(
    private val cells: Map<Point, Val> = emptyMap(),
) {
    constructor(
        source: String,
    ): this(
        cells = source.lineSequence().flatMapIndexed() { y, line ->
            line.split(SPACES_REGEX).mapIndexedNotNull { x, cell ->
                 Val.of(cell)?.let { Point(x = x, y = y) to it }
            }
        }.toMap()
    )

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
                append("\n")
            }
        }
    }

    companion object {
        val SPACES_REGEX = "\\s+".toRegex()
    }
}