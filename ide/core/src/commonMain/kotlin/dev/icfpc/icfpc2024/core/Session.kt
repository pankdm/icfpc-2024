package dev.icfpc.icfpc2024.core

class Session(
    board: Board,
) {
    private var histories = ArrayDeque(
        listOf(History(board))
    )

    var pointer = 0

    val currentHistory: History
        get() = histories[pointer]

    val currentBoard: Board
        get() = currentHistory.current

    var ticks = 0
    var minX = board.minX
    var maxX = board.maxX
    var minY = board.minY
    var maxY = board.maxY
    var minT = 1
    var maxT = 1
    var output: Int? = null

    fun back() {
        check(pointer > 0) { "Unable to rewind past 0" }
        pointer--
    }

    fun forward() {
        if (pointer == histories.lastIndex) {
            tick()
        } else {
            pointer++
        }
    }

    fun tick() {
        check(pointer == histories.lastIndex) { "You've already computed next state, use \"forward\" instead" }
        if (output != null) return

        val builder = HistoryBuilder(currentHistory)

        currentBoard.cells.forEach { (point, value) ->
            if (value is Op) {
                value.apply(builder = builder, cell = point)
            }
        }

        ticks++
        if (builder.output != null) {
            output = builder.output
        } else {
            val newHistory = builder.build()
            histories += newHistory
            pointer = histories.lastIndex
            minX = minOf(minX, currentBoard.minX)
            maxX = maxOf(maxX, currentBoard.maxX)
            minY = minOf(minY, currentBoard.minY)
            maxY = maxOf(maxY, currentBoard.maxY)
            maxT = maxOf(maxT, currentHistory.time)
        }
    }

    fun gc() {
        histories = ArrayDeque(listOf(histories.last()))
        pointer = 0
    }

    fun dumpStats(): String = """
        Output: ${output ?: "Unknown"}
        Total ticks: $ticks
        X: [$minX .. $maxX]
        Y: [$minY .. $maxY]
        T: [$minT .. $maxT]
        Volume: ${(maxX - minX + 1) * (maxY - minY + 1) * (maxT - minT + 1)}
    """.trimIndent()

    class HistoryBuilder(
        private val from: History,
    ) {
        private val cells = from.current.cells

        private val writes = mutableMapOf<Point, Value>()
        private val removes = mutableSetOf<Point>()
        private val warps = mutableMapOf<Point, Value>()
        private var warpDelta: Int? = null
        private var _output: Int? = null
        val output: Int?
            get() = _output


        operator fun get(point: Point) = cells[point]

        operator fun set(point: Point, value: Value?) {
            if (this[point] == Submit) {
                check(output == null) { "Output was already set to $output" }
                requireNotNull(value) { "Output shouldn't be null" }
                require(value is Num) { "Output should be num" }

                _output = value.value

                removes.clear()
                writes.clear()
                warps.clear()
            }

            // No need to apply anything if we're outputting values or warping
            if (output != null) return
            if (warps.isNotEmpty()) return

            if (value != null) {
                require(point !in writes) { "Trying to write different values to point $point" }
                writes[point] = value
            } else {
                removes += point
            }
        }

        fun warp(source: Point, timeDelta: Int, point: Point, value: Value) {
            // No need to warp if we're outputting values
            if (output != null) return

            require(timeDelta > 0) { "Expect to warp at least one tick into past for cell $source" }
            require(timeDelta < from.time) { "Can't warp past Big Bang for cell $source" }
            require(warpDelta == null || warpDelta == timeDelta) {
                "Unable to warp to $warpDelta and $timeDelta at the same time for cell $source"
            }
            require(warps.getOrPut(point) { value } == value) {
                "Trying to write different values to point $point when warping for cell $source"
            }

            warpDelta = timeDelta
            writes.clear()
            removes.clear()
        }

        fun build(): History {
            val warpDelta = warpDelta
            return History(
                boards = when {
                    output != null -> from.boards
                    warpDelta != null -> {
                        val sourceBoard = from.boards.asReversed()[warpDelta]
                        val newBoard = Board(
                            cells = sourceBoard.cells + warps,
                        )
                        from.boards.dropLast(warpDelta + 1) + newBoard
                    }

                    else -> {
                        from.boards + Board(
                            cells = from.current.cells.toMutableMap().apply {
                                this -= removes
                                this += writes
                            }
                        )
                    }
                }
            )
        }
    }
}