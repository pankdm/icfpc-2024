plugins {
    alias(libs.plugins.kotlin.multiplatform)
    application
}

kotlin {
    jvmToolchain(17)
    jvm {
        withJava()
    }

    sourceSets {
        commonMain.dependencies {
            implementation(libs.clikt)
            implementation(projects.core)
        }
    }
}

application {
    mainClass.set("dev.icfpc.icfpc2024.RunnerKt")
}
