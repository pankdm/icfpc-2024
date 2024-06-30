package dev.icfpc.icfpc2024

import com.github.ajalt.clikt.core.CliktCommand
import com.github.ajalt.clikt.parameters.arguments.argument
import com.github.ajalt.clikt.parameters.arguments.default
import com.github.ajalt.clikt.parameters.arguments.help
import com.github.ajalt.clikt.parameters.types.file
import com.github.ajalt.clikt.parameters.types.int

class Runner : CliktCommand(
    printHelpOnEmptyArgs = true,
) {
    private val input by argument().file(mustExist = true).help("Input file for 3d task")
    private val a by argument().int().help("Parameter A to be set for runner").default(0)
    private val b by argument().int().help("Parameter B to be set for runner").default(0)

    override fun run() {
        println("Running ${input.canonicalPath} with A=$a and B=$b")
    }
}

fun main(args: Array<String>) = Runner().main(argv = args)
