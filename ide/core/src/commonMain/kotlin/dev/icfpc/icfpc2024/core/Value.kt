package dev.icfpc.icfpc2024.core

sealed interface Value {
    companion object {
        fun of(source: String): Value? = when {
            source == "." -> null
            source == "A" -> Source.A
            source == "B" -> Source.B
            source == "S" -> Submit
            source.length == 1 && !source.first().isDigit() -> Op.of(source.first())
            else -> runCatching { Num(value = source.toInt()) }.getOrNull()
        }
    }
}

data class Num(
    val value: Int,
): Value {
    override fun toString() = value.toString()
}

sealed interface Source: Value {
    data object A: Source {
        override fun toString() = "A"
    }

    data object B: Source {
        override fun toString() = "B"
    }
}

data object Submit: Value {
    override fun toString() = "S"
}

sealed class Op: Value {
    abstract val opcode: Char

    abstract fun apply(builder: Session.HistoryBuilder, cell: Point)

    class Arrow(
        override val opcode: Char,
        val from: Offset,
        val to: Offset,
    ) : Op() {
        override fun apply(builder: Session.HistoryBuilder, cell: Point) {
            builder[cell + from]?.let { value ->
                builder[cell + to] = value
                builder[cell + from] = null
            }
        }
    }

    class Compute(
        override val opcode: Char,
        val func: (Int, Int) -> Int,
    ) : Op() {
        override fun apply(builder: Session.HistoryBuilder, cell: Point) {
            val first = builder[cell + Offset.LEFT] as? Num ?: return
            val second = builder[cell + Offset.UP] as? Num ?: return

            runCatching { func(first.value, second.value) }.onSuccess { result ->
                builder[cell + Offset.LEFT] = null
                builder[cell + Offset.UP] = null
                builder[cell + Offset.RIGHT] = Num(result)
                builder[cell + Offset.DOWN] = Num(result)
            }
        }
    }

    class Compare(
        override val opcode: Char,
        val expect: Boolean,
    ) : Op() {
        override fun apply(builder: Session.HistoryBuilder, cell: Point) {
            val first = builder[cell + Offset.LEFT]
            val second = builder[cell + Offset.UP]
            if (first !is Num && first !is Op) return
            if (second !is Num && second !is Op) return

            if ((first == second) == expect) {
                builder[cell + Offset.LEFT] = null
                builder[cell + Offset.UP] = null
                builder[cell + Offset.RIGHT] = second
                builder[cell + Offset.DOWN] = first
            }
        }
    }

    class Warp(
        override val opcode: Char,
    ) : Op() {
        override fun apply(builder: Session.HistoryBuilder, cell: Point) {
            val value = builder[cell + Offset.UP]
            if (value !is Num && value !is Op) return
            val dx = builder[cell + Offset.LEFT] as? Num ?: return
            val dy = builder[cell + Offset.RIGHT] as? Num ?: return
            val dt = builder[cell + Offset.DOWN] as? Num ?: return

            builder.warp(
                timeDelta = dt.value,
                point = cell - Offset(x = dx.value, y = dy.value),
                value = value,
            )
        }
    }

    override fun toString() = opcode.toString()

    companion object {
        fun of(char: Char) = when (char) {
            '<' -> Arrow(
                opcode = char,
                from = Offset.RIGHT,
                to = Offset.LEFT,
            )
            '>' -> Arrow(
                opcode = char,
                from = Offset.LEFT,
                to = Offset.RIGHT,
            )
            '^' -> Arrow(
                opcode = char,
                from = Offset.DOWN,
                to = Offset.UP,
            )
            'v' -> Arrow(
                opcode = char,
                from = Offset.UP,
                to = Offset.DOWN,
            )
            '+' -> Compute(
                opcode = char,
                func = Int::plus,
            )
            '-' -> Compute(
                opcode = char,
                func = Int::minus,
            )
            '*' -> Compute(
                opcode = char,
                func = Int::times,
            )
            '/' -> Compute(
                opcode = char,
                func = Int::div,
            )
            '%' -> Compute(
                opcode = char,
                func = Int::rem,
            )
            '=' -> Compare(
                opcode = char,
                expect = true,
            )
            '#' -> Compare(
                opcode = char,
                expect = false,
            )
            '@' -> Warp(
                opcode = char,
            )
            else -> error("Unknown opcode \"$char\"")
        }
    }
}
