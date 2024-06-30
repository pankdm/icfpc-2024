package dev.icfpc.icfpc2024.core

sealed interface Val {
    companion object {
        fun of(source: String): Val? = when {
            source == "." -> null
            source.length == 1 && !source.first().isDigit() -> Op(source.first())
            else -> runCatching { Num(value = source.toInt()) }.getOrNull()
        }
    }
}

data class Num(
    val value: Int,
): Val {
    override fun toString() = value.toString()
}

data class Op(
    val opcode: Char,
): Val {
    override fun toString() = opcode.toString()
}
