package dev.icfpc.icfpc2024.core

data class Point(
    val x: Int,
    val y: Int,
) {
    operator fun plus(offset: Offset) = Point(
        x = x + offset.point.x,
        y = y + offset.point.y,
    )
}

@JvmInline
value class Offset(
    val point: Point,
)
