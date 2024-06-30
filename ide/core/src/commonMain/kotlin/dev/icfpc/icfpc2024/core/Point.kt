package dev.icfpc.icfpc2024.core

data class Point(
    val x: Int,
    val y: Int,
) {
    operator fun plus(offset: Offset) = Point(
        x = x + offset.point.x,
        y = y + offset.point.y,
    )

    operator fun minus(offset: Offset) = Point(
        x = x - offset.point.x,
        y = y - offset.point.y,
    )

    override fun toString(): String {
        return super.toString()
    }
}

@JvmInline
value class Offset(
    val point: Point,
) {
    constructor(x: Int, y: Int): this(point = Point(x = x, y = y))

    companion object {
        val LEFT = Offset(x = -1, y = 0)
        val RIGHT = Offset(x = 1, y = 0)
        val UP = Offset(x = 0, y = -1)
        val DOWN = Offset(x = 0, y = 1)
    }
}
