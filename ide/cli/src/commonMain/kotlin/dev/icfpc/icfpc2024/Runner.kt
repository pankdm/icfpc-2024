package dev.icfpc.icfpc2024

import com.github.ajalt.clikt.core.CliktCommand

class Runner : CliktCommand() {
    override fun run() {
        println("Kotlin goes brrr")
    }
}

fun main(args: Array<String>) = Runner().main(argv = args)
