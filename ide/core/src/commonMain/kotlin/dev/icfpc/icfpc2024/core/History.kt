package dev.icfpc.icfpc2024.core

class History private constructor(
    val boards: List<Board>,
    val output: Int? = null,
) {
    constructor(board: Board): this(listOf(board))

    val current: Board
        get() = boards.last()

    val time
        get() = boards.size

    operator fun get(time: Int) = boards[time]

    fun tick(): History {
        val builder = Builder(this)

        current.cells.forEach { (point, value) ->
            if (value is Op) {
                value.apply(builder = builder, cell = point)
            }
        }

        return builder.build()
    }

    override fun toString(): String {
        return boards.joinToString("\n\n===\n\n")
    }

    class Builder(
        private val from: History,
    ) {
        private val cells = from.current.cells

        private val writes = mutableMapOf<Point, Value>()
        private val removes = mutableSetOf<Point>()
        private val warps = mutableMapOf<Point, Value>()
        private var warpDelta: Int? = null
        private var output: Int? = null

        operator fun get(point: Point) = cells[point]

        operator fun set(point: Point, value: Value?) {
            if (this[point] == Submit) {
                check(output == null) { "Output was already set to $output" }
                requireNotNull(value) { "Output shouldn't be null" }
                require(value is Num) { "Output should be num" }

                output = value.value
            }

            // No need to apply anything if we're outputting values or warping
            if (output != null) return
            if (warps.isNotEmpty()) return

            if (value != null) {
                require(writes.getOrPut(point) { value } == value) {
                    "Trying to write different values to point $point"
                }
            } else {
                removes += point
            }
        }

        fun warp(timeDelta: Int, point: Point, value: Value) {
            // No need to warp if we're outputting values
            if (output != null) return

            require(timeDelta > 0) { "Expect to warp at least one tick into past" }
            require(timeDelta < from.time) { "Can't warp past Big Bang" }
            require(warpDelta == null || warpDelta == timeDelta) {
                "Unable to warp to $warpDelta and $timeDelta at the same time"
            }
            warpDelta = timeDelta
            require(warps.getOrPut(point) { value } == value) {
                "Trying to write different values to point $point when warping"
            }
        }

        fun build(): History {
            val warpDelta = warpDelta
            return when {
                output != null -> {
                    History(
                        boards = from.boards,
                        output = output,
                    )
                }
                warpDelta != null -> {
                    val sourceBoard = from.boards.asReversed()[warpDelta]
                    val newBoard = Board(
                        cells = sourceBoard.cells + warps,
                    )
                    History(
                        boards = from.boards.dropLast(warpDelta + 1) + newBoard
                    )
                }
                else -> {
                    val newBoard = Board(
                        cells = from.current.cells.toMutableMap().apply {
                            this -= removes
                            this += writes
                        }
                    )
                    History(
                        boards = from.boards + newBoard
                    )
                }
            }
        }
    }
}
