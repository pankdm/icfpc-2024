plugins {
    alias(libs.plugins.kotlin.multiplatform)
}

kotlin {
    jvmToolchain(17)

    jvm()
    
    sourceSets {
        commonMain.dependencies {

        }
    }
}

