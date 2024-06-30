package dev.icfpc.icfpc2024.core

/**
 * @param boards all boards in the current history that you could manipulate / warp to.
 * Note that you should use [Session] to manipulate multiple histories for debugging, undo/redo, etc.
 */
class History internal constructor(
    val boards: List<Board>,
) {
    constructor(board: Board): this(listOf(board))

    val current: Board
        get() = boards.last()

    val time
        get() = boards.size

    operator fun get(time: Int) = boards[time]

    override fun toString(): String {
        return boards.joinToString(separator = "\n\n---===---\n\n")
    }
}
