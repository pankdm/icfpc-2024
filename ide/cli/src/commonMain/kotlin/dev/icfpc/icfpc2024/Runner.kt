package dev.icfpc.icfpc2024

import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.arguments.default
import com.github.ajalt.clikt.parameters.arguments.help
import com.github.ajalt.clikt.parameters.types.enum
import com.github.ajalt.clikt.parameters.types.file
import com.github.ajalt.clikt.parameters.types.int
import dev.icfpc.icfpc2024.core.Board
import dev.icfpc.icfpc2024.core.History
import dev.icfpc.icfpc2024.core.Session

class Runner : CliktCommand(
    printHelpOnEmptyArgs = true,
) {
    private val mode by argument().enum<Mode>().help("Emulator mode: run or debug").default(Mode.RUN)
    private val input by argument().file(mustExist = true).help("Input file for 3d task")
    private val a by argument().int().help("Parameter A to be set for runner").default(0)
    private val b by argument().int().help("Parameter B to be set for runner").default(0)

    override fun run() {
        println("Running ${input.canonicalPath} with A=$a and B=$b, mode debug")

        val board = input.useLines { lines -> Board.Builder.fromLines(lines) }
            .withA(a)
            .withB(b)
            .build()

        val session = Session(board)

        while (session.output == null) {
            if (mode == Mode.DEBUG) {
                println(session.currentBoard)
                println()
                readlnOrNull()
            }
            session.tick()
        }

        println("\nFINAL STATS:\n")
        println(session.currentBoard)
        println(session.dumpStats())
    }

    enum class Mode {
        RUN,
        DEBUG,
    }
}

fun main(args: Array<String>) = Runner().main(argv = args)
